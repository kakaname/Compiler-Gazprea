//
// Created by Chen on 2022-12-08.
//


#include "Passes/BuildAST/AddingFreeNodesForContinueBreak.h"

void AddingFreeNodesForContinueBreak::visitInfiniteLoop(InfiniteLoop *Loop) {
    vector<Identifier *> LocalFreedIdentifiers;
    for (auto *Child : *Loop->getBlock()) {
        if (dyn_cast<Declaration>(Child)) {
            // An identifier type of vector or matrix has to be freed
            auto Declar = dyn_cast<Declaration>(Child);
            auto TypeKind = Declar->getIdentType()->getKind();
            if (TypeKind == Type::T_Vector || TypeKind == Type::T_Matrix) {
                LocalFreedIdentifiers.push_back(Declar->getIdentifier());
            }
        }
    }
    FreedIdentifiers.push_back(LocalFreedIdentifiers);
    visit(Loop->getBlock());
    FreedIdentifiers.pop_back();
}

void AddingFreeNodesForContinueBreak::visitConditionalLoop(ConditionalLoop *Loop) {
    vector<Identifier *> LocalFreedIdentifiers;
    for (auto *Child : *Loop->getBlock()) {
        if (dyn_cast<Declaration>(Child)) {
            // An identifier type of vector or matrix has to be freed
            auto Declar = dyn_cast<Declaration>(Child);
            auto TypeKind = Declar->getIdentType()->getKind();
            if (TypeKind == Type::T_Vector || TypeKind == Type::T_Matrix) {
                LocalFreedIdentifiers.push_back(Declar->getIdentifier());
            }
        }
    }
    FreedIdentifiers.push_back(LocalFreedIdentifiers);
    visit(Loop->getBlock());
    FreedIdentifiers.pop_back();
}

void AddingFreeNodesForContinueBreak::visitDomainLoop(DomainLoop *Loop) {
    vector<Identifier *> LocalFreedIdentifiers;
    for (auto *Child : *Loop->getBody()) {
        if (dyn_cast<Declaration>(Child)) {
            // An identifier type of vector or matrix has to be freed
            auto Declar = dyn_cast<Declaration>(Child);
            auto TypeKind = Declar->getIdentType()->getKind();
            if (TypeKind == Type::T_Vector || TypeKind == Type::T_Matrix) {
                LocalFreedIdentifiers.push_back(Declar->getIdentifier());
            }
        }
    }
    FreedIdentifiers.push_back(LocalFreedIdentifiers);
    visit(Loop->getBody());
    FreedIdentifiers.pop_back();
}

void AddingFreeNodesForContinueBreak::visitBreak(Break *Break) {
    auto FreeN = PM->Builder.build<FreeNode>();
    for (auto Node : FreedIdentifiers.back()) {
        addFreedIdentifier(FreeN, Node);
    }
    Break->getParent()->insertChildBefore(Break, FreeN);
}

void AddingFreeNodesForContinueBreak::visitContinue(Continue *Continue) {
    if(FreedIdentifiers.empty())
        return;
    auto FreeN = PM->Builder.build<FreeNode>();
    for (auto Node : FreedIdentifiers.back()) {
        addFreedIdentifier(FreeN, Node);
    }
    Continue->getParent()->insertChildBefore(Continue, FreeN);
}


void AddingFreeNodesForContinueBreak::addFreedIdentifier(FreeNode *FreeNode, Identifier *Ident) {
    auto NewIdent = PM->Builder.build<Identifier>();
    NewIdent->setName(Ident->getName());
    NewIdent->setReferred(Ident->getReferred());
    NewIdent->setIdentType(Ident->getIdentType());
    FreeNode->addChild(NewIdent);
}