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
    Return *ReturnStat = nullptr;
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
            ReturnStat = dyn_cast<Return>(Child);
        }
        else if (auto ContinueStat = dyn_cast<Continue>(Child)) {

        }
        else if (auto BreakStat = dyn_cast<Break>(Child)) {

        }
    }

    auto FreeN = PM->Builder.build<FreeNode>();

    if (HasReturn) {
        for (auto Node : FuncFreedIdentifiers) {
            addFreedIdentifier(FreeN, Node);
        }
        auto ReturnResultTy = PM->getAnnotation<ExprTypeAnnotatorPass>(ReturnStat->getReturnExpr());

        // A new variable
        auto ReturnResultSym = PM->SymTable.defineObject("", ReturnResultTy);
        auto ReturnResultIdent = PM->Builder.build<Identifier>();
        ReturnResultIdent->setIdentType(ReturnResultTy);
        ReturnResultIdent->setReferred(ReturnResultSym);

        // A new declaration
        auto ReturnResultDeclar = PM->Builder.build<Declaration>();
        ReturnResultDeclar->setIdentType(ReturnResultTy);
        ReturnResultDeclar->setIdent(ReturnResultIdent);
        ReturnResultDeclar->setInitExpr(ReturnStat->getReturnExpr());

        Blk->insertChildBefore(ReturnResultDeclar, FreeN);
        // Add the Free Node before the return statement
        Blk->insertChildBefore(ReturnStat, FreeN);
        // change the expression of the return statement
        ReturnStat->setReturnExpr(ReturnResultIdent);
    }
    else {
        for (auto Node : LocalFreedIdentifiers) {
            addFreedIdentifier(FreeN, Node);
        }
        Blk->addChild(FreeN);   // Add the Free Node to the end of the block
    }

    for (int I = 0; I < NumOfFreedIdentifiers; I++) {
        FuncFreedIdentifiers.pop_back();
    }
}


void AddingFreeNodesPass::addFreedIdentifier(FreeNode *FreeNode, Identifier *Ident) {
    auto NewIdent = PM->Builder.build<Identifier>();
    NewIdent->setName(Ident->getName());
    NewIdent->setReferred(Ident->getReferred());
    NewIdent->setIdentType(Ident->getIdentType());
    FreeNode->addChild(FreeNode);
}
