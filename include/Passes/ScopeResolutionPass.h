//
// Created by dhanrajbir on 11/11/22.
//

#ifndef GAZPREABASE_SCOPERESOLUTIONPASS_H
#define GAZPREABASE_SCOPERESOLUTIONPASS_H

#include <map>
#include <string>

#include "PassManager.h"
#include "Symbol/Symbol.h"
#include "VisitorPass.h"
#include "ExprTypeAnnotatorPass.h"


using llvm::isa;
using llvm::dyn_cast;

using std::map;
using std::string;


struct ScopeTreeNode: public TreeNode, ResourceIdMixin<ScopeTreeNode> {
    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNode::N_ScopeTreeNode;
    }

    map<string, const Symbol*> SymbolsInScope;

    void declareInScope(const string& Name, const Symbol *Sym) {
        auto Res = SymbolsInScope.find(Name);
        if (Res != SymbolsInScope.end())
            throw std::runtime_error("Tried to redeclare when already in scope");

        SymbolsInScope.insert({Name, Sym});
    }

    const Symbol *resolve(const string &Name) {
        auto Res = SymbolsInScope.find(Name);
        if (Res != SymbolsInScope.end())
            return Res->second;

        if (!getParent())
            return nullptr;

        auto Parent = dyn_cast<ScopeTreeNode>(getParent());
        assert(Parent && "ScopeTreeNode has a non ScopeTreeNode parent?");
        return Parent->resolve(Name);
    }

    ScopeTreeNode(): TreeNode(TreeNodeKind::N_ScopeTreeNode) {}
};


struct ScopeResolutionPass : VisitorPass<ScopeResolutionPass, void> {


    ScopeTreeNode *CurrentScope;
    ASTPassManager *PM;

    ExprTypeAnnotatorPass ExprAnnotator;

    void runOnAST(ASTPassManager &PManager, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "ScopeResolutionPass should run on the"
                                      " entire program");
        PM = &PManager;
        auto &GlobalScope = PM->getResource<ScopeTreeNode>();
        CurrentScope = &GlobalScope;
        visit(Root);
    }

    void visitDeclaration(Declaration *Decl) {
        visit(Decl->getInitExpr());
        // The type must be inferred.
        if (!Decl->getIdentType()) {
            ExprAnnotator.runOnAST(*PM, Decl->getInitExpr());
            auto ExprType = PM->getAnnotation<ExprTypeAnnotatorPass>(Decl->getInitExpr());
            Decl->setIdentType(ExprType);
            Decl->getIdentifier()->setIdentType(ExprType);
        }
        auto Sym = PM->SymTable.defineObject(
                Decl->getIdentifier()->getName(), Decl->getIdentType());
        Decl->getIdentifier()->setReferred(Sym);
        CurrentScope->declareInScope(Decl->getIdentifier()->getName(), Sym);
    }

    void visitIdentifier(Identifier *Ident) const {
        auto Resolved = CurrentScope->resolve(Ident->getName());
        if (!Resolved)
            throw std::runtime_error("Symbol for identifier " + Ident->getName() + " not found");
        Ident->setIdentType(Resolved->getSymbolType());
        Ident->setReferred(Resolved);
    }

    void visitConditionalLoop(ConditionalLoop *Loop) {
        auto NewScope = PM->Builder.build<ScopeTreeNode>();
        CurrentScope->addChild(NewScope);
        CurrentScope = NewScope;
        visit(Loop->getBlock());
        CurrentScope = cast<ScopeTreeNode>(NewScope->getParent());
    }

    void visitConditional(Conditional *Cond) {
        auto NewScope = PM->Builder.build<ScopeTreeNode>();
        CurrentScope->addChild(NewScope);
        visit(Cond->getBlock());
        CurrentScope = cast<ScopeTreeNode>(NewScope->getParent());
    };

    void visitConditionalElse(ConditionalElse *CondWithElse) {
        auto NewScope = PM->Builder.build<ScopeTreeNode>();
        CurrentScope->addChild(NewScope);
        visit(CondWithElse->getIfBlock());
        CurrentScope = cast<ScopeTreeNode>(NewScope->getParent());

        NewScope = PM->Builder.build<ScopeTreeNode>();
        CurrentScope->addChild(NewScope);
        visit(CondWithElse->getElseBlock());
        CurrentScope = cast<ScopeTreeNode>(NewScope->getParent());
    };

    void visitProcedureDef(ProcedureDef *Def) {
        auto ParamList = Def->getParamList();
        auto FunctionParamScope = PM->Builder.build<ScopeTreeNode>();
        CurrentScope->addChild(FunctionParamScope);
        for (auto *Param : *ParamList) {
            auto ParamIdent = dyn_cast<Identifier>(Param);
            assert(ParamIdent);
            assert(ParamIdent->getIdentType());
            auto ParamSym = PM->SymTable.defineObject(
                    ParamIdent->getName(), ParamIdent->getIdentType());
            FunctionParamScope->declareInScope(ParamIdent->getName(), ParamSym);
        }
        auto FuncBodyScope = PM->Builder.build<ScopeTreeNode>();
        FunctionParamScope->addChild(FuncBodyScope);
        CurrentScope = FuncBodyScope;
        visit(Def->getBlock());
        CurrentScope = dyn_cast<ScopeTreeNode>(FunctionParamScope->getParent());
        assert(CurrentScope);
    }

    void visitFunctionDef(FunctionDef *Def) {
        auto ParamList = Def->getParamList();
        auto FunctionParamScope = PM->Builder.build<ScopeTreeNode>();
        vector<const Type *> ParamTypes;
        CurrentScope->addChild(FunctionParamScope);
        for (auto *Param : *ParamList) {
            auto ParamIdent = dyn_cast<Identifier>(Param);
            assert(ParamIdent);
            assert(ParamIdent->getIdentType());
            auto ParamSym = PM->SymTable.defineObject(
                    ParamIdent->getName(), ParamIdent->getIdentType());
            FunctionParamScope->declareInScope(ParamIdent->getName(), ParamSym);
            ParamTypes.emplace_back(ParamIdent->getIdentType());
        }
        auto FuncBodyScope = PM->Builder.build<ScopeTreeNode>();
        FunctionParamScope->addChild(FuncBodyScope);
        CurrentScope = FuncBodyScope;
        visit(Def->getBlock());
        CurrentScope = dyn_cast<ScopeTreeNode>(FunctionParamScope->getParent());
        assert(CurrentScope);

        auto FuncName = Def->getIdentifier()->getName();
        auto FuncTy = PM->TypeReg.getFunctionType(ParamTypes, Def->getRetTy());

        auto Resolved = CurrentScope->resolve(FuncName);
        if (Resolved) {
            assert(Resolved->getSymbolType()->isSameTypeAs(FuncTy) &&
            "Differing declaration and definition");
            return;
        }

        auto FuncSym = PM->SymTable.defineObject(FuncName, FuncTy);
        CurrentScope->declareInScope(FuncName, FuncSym);
    }

    void visitFunctionDecl(FunctionDecl *Decl) {
        auto FuncName = Decl->getIdentifier()->getName();
        auto FuncTy = PM->TypeReg.getFunctionType(
                Decl->getParamTypes(), Decl->getRetType());
        auto FuncSym = PM->SymTable.defineObject(FuncName, FuncTy);
        Decl->getIdentifier()->setIdentType(FuncTy);
        CurrentScope->declareInScope(FuncName, FuncSym);
    }

    void visitMemberAccess(MemberAccess *Access) {
        visit(Access->getIdentifier());
    }
};

#endif //GAZPREABASE_SCOPERESOLUTIONPASS_H
