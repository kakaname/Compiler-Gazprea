//
// Created by Chen on 2022-11-28.
//

#include "Passes/BuildAST/AddingFreeNodesPass.h"

void AddingFreeNodesPass::visitFunctionDef(FunctionDef *FuncDef) {
    // Get all the parameters type of vector or matrix
    for(auto Child : *FuncDef->getParamList()) {
        auto Parameter = dyn_cast<Identifier>(Child);
        auto TypeKind = Parameter->getIdentType()->getKind();
        if(TypeKind == Type::T_Vector || TypeKind == Type::T_Matrix) {
            FuncFreedIdentifiers.push_back(Parameter);
        }
    }
    // Visit the block
    visit(FuncDef->getBlock());
    // Clear the FuncFreeNodes
    FuncFreedIdentifiers.clear();
}

void AddingFreeNodesPass::visitProcedureDef(ProcedureDef *ProcedureDef) {
    // Get all the parameters type of vector or matrix
    for(auto Child : *ProcedureDef->getParamList()) {
        auto Parameter = dyn_cast<Identifier>(Child);
        auto OpKind = Parameter->getIdentType()->getKind();
        if(OpKind == Type::T_Vector || OpKind == Type::T_Matrix) {
            FuncFreedIdentifiers.push_back(Parameter);
        }
    }
    // Visit the block
    visit(ProcedureDef->getBlock());
    // Clear the FuncFreeNodes
    FuncFreedIdentifiers.clear();
}

void AddingFreeNodesPass::visitBlock(Block *Blk) {
    bool HasReturn = false;
    vector<Identifier *> LocalFreedIdentifiers;
    int NumOfFreedIdentifiers = 0;

    for (auto *Child : *Blk) {
        if (dyn_cast<Declaration>(Child)) {
            // An identifier type of vector or matrix has to be freed
            auto Declar = dyn_cast<Declaration>(Child);
            auto TypeKind = Declar->getIdentType()->getKind();
            if (TypeKind == Type::T_Vector || TypeKind == Type::T_Matrix) {
                LocalFreedIdentifiers.push_back(Declar->getIdentifier());
                FuncFreedIdentifiers.push_back(Declar->getIdentifier());
                NumOfFreedIdentifiers++;
            }
        }
        else if (dyn_cast<Block>(Child))
            visit(Child);
        else if (dyn_cast<Return>(Child)) {
            HasReturn = true;
        }
    }

    auto FreeN = PM->Builder.build<FreeNode>();

    if (HasReturn) {
        for (auto Node : FuncFreedIdentifiers) {
            FreeN->addFreedIdentifier(Node);
        }
    }
    else {
        for (auto Node : LocalFreedIdentifiers) {
            FreeN->addFreedIdentifier(Node);
        }
    }
    
    for (int I = 0; I < NumOfFreedIdentifiers; I++) {
        FuncFreedIdentifiers.pop_back();
    }
}
