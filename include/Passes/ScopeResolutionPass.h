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

    void runOnAST(ASTPassManager &PManager, ASTNodeT &Root) {
        assert(isa<Program>(&Root) && "ScopeResolutionPass should run on the"
                                      " entire program");
        PM = &PManager;
        auto &GlobalScope = PM->getResource<ScopeTreeNode>();
        CurrentScope = &GlobalScope;
        visit(&Root);
    }

    void visitDeclaration(Declaration *Decl) const {
        auto Ty = PM->SymTable.defineObject(
                Decl->getIdentifier()->getName(), Decl->getIdentType());
        Decl->getIdentifier()->setReferred(Ty);
    }

    void visitIdentifier(Identifier *Ident) const {
        auto Resolved = CurrentScope->resolve(Ident->getName());
        if (!Resolved)
            throw std::runtime_error("Not found");
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
};

#endif //GAZPREABASE_SCOPERESOLUTIONPASS_H
