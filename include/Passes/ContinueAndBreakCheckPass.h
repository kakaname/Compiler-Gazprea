//
// Created by rajan on 11/10/22.
//


#pragma once
#include "VisitorPass.h"
#include "PassManager.h"

struct ContinueAndBreakCheckPass : VisitorPass<ContinueAndBreakCheckPass, void> {
    using AnnotationT = TreeNode*;

    static void visitBreak(Break *Break);
    static void visitContinue(Continue *Continue);
    void runOnAST(ASTPassManager &P, ASTNodeT *Root) {
        PM = &P;
        visit(Root);
    };

    ASTPassManager *PM;

};