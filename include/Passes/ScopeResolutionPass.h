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

// TODO confirm that all children are visited

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
            assert(ExprType && "Cannot infer declaration type");
            assert(!isa<NullTy>(ExprType) && !isa<IdentityTy>(ExprType) &&
                    "Cannot infer identity or null type." );
            if (!Decl->IsConst)
                ExprType = PM->TypeReg.getVarTypeOf(ExprType);
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
        assert(Resolved->getSymbolType() && "Null symbol type?");
        Ident->setIdentType(Resolved->getSymbolType());
        Ident->setReferred(Resolved);
    }

    void visitConditionalLoop(ConditionalLoop *Loop) {
        visit(Loop->getConditional());
        auto NewScope = PM->Builder.build<ScopeTreeNode>();
        CurrentScope->addChild(NewScope);
        CurrentScope = NewScope;
        visit(Loop->getBlock());
        CurrentScope = cast<ScopeTreeNode>(NewScope->getParent());
    }

    void visitConditional(Conditional *Cond) {
        visit(Cond->getConditional());
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

        // Add a scope for the parameters of the function.
        auto ProcParamScope = PM->Builder.build<ScopeTreeNode>();
        CurrentScope->addChild(ProcParamScope);
        CurrentScope = ProcParamScope;
        // Iterate over the identifiers that are the function parameters
        // and add them to the scope.
        for (auto *Param : *ParamList) {
            auto ParamIdent = dyn_cast<Identifier>(Param);
            assert(ParamIdent);
            auto ParamName = ParamIdent->getName();
            auto ParamType = ParamIdent->getIdentType();
            assert(ParamType);

            // Declare it in scope.
            auto ParamSym = PM->SymTable.defineObject(ParamName, ParamType);
            ParamIdent->setReferred(ParamSym);
            ProcParamScope->declareInScope(ParamIdent->getName(), ParamSym);
        }
        // Add another scope for the function body.
        visit(Def->getBlock());

        // Go back to the original scope.
        CurrentScope = cast<ScopeTreeNode>(ProcParamScope->getParent());

        auto ProcName = Def->getIdentifier()->getName();
        auto ProcTy = Def->getIdentifier()->getIdentType();
        auto Resolved = CurrentScope->resolve(ProcName);
        if (Resolved) {
            assert(Resolved->getSymbolType()->isSameTypeAs(ProcTy) &&
                   "Differing declaration and definition");
            return;
        }

        auto ProcSym = PM->SymTable.defineObject(ProcName, ProcTy);
        CurrentScope->declareInScope(ProcName, ProcSym);
    }

    void visitFunctionDef(FunctionDef *Def) {
        auto ParamList = Def->getParamList();

        // Add a scope for the parameters of the function.
        auto FuncParamScope = PM->Builder.build<ScopeTreeNode>();
        CurrentScope->addChild(FuncParamScope);
        CurrentScope = FuncParamScope;

        // Iterate over the identifiers that are the function parameters
        // and add them to the scope.
        for (auto *Param : *ParamList) {
            auto ParamIdent = dyn_cast<Identifier>(Param);
            assert(ParamIdent);
            auto ParamName = ParamIdent->getName();
            auto ParamType = ParamIdent->getIdentType();
            assert(ParamType);

            // Declare it in scope.
            auto ParamSym = PM->SymTable.defineObject(ParamName, ParamType);
            ParamIdent->setReferred(ParamSym);
            FuncParamScope->declareInScope(ParamIdent->getName(), ParamSym);
        }
        visit(Def->getBlock());

        // Go back to the original scope.
        CurrentScope = cast<ScopeTreeNode>(FuncParamScope->getParent());

        auto FuncName = Def->getIdentifier()->getName();
        auto FuncTy = Def->getIdentifier()->getIdentType();
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
        auto FuncTy = Decl->getIdentifier()->getIdentType();
        auto FuncSym = PM->SymTable.defineObject(FuncName, FuncTy);
        CurrentScope->declareInScope(FuncName, FuncSym);
    }

    void visitProcedureDecl(ProcedureDecl *Decl) {
        auto ProcName = Decl->getIdentifier()->getName();
        auto ProcTy = Decl->getIdentifier()->getIdentType();
        auto ProcSym = PM->SymTable.defineObject(ProcName, ProcTy);
        CurrentScope->declareInScope(ProcName, ProcSym);
    }

    void visitMemberAccess(MemberAccess *Access) {
        visit(Access->getExpr());
    }

    void visitBlock(Block *Blk) {
        auto BlkScope = PM->Builder.build<ScopeTreeNode>();
        CurrentScope->addChild(BlkScope);
        CurrentScope = BlkScope;
        for (auto Child : *Blk)
            visit(Child);
        CurrentScope = cast<ScopeTreeNode>(CurrentScope->getParent());
    }
};

#endif //GAZPREABASE_SCOPERESOLUTIONPASS_H
