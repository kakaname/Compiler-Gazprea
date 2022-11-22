//
// Created by rajan on 11/10/22.
//


#pragma once
#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"

struct ContinueAndBreakCheckPass : VisitorPass<ContinueAndBreakCheckPass, void> {
    static void visitBreak(Break *Break);
    static void visitContinue(Continue *Continue);
    void runOnAST(ASTPassManager &P, ASTNodeT *Root) {
        PM = &P;
        visit(Root);
    };

    ASTPassManager *PM;

};