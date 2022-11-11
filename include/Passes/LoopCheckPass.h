//
// Created by rajan on 11/10/22.
//


#pragma once
#include "VisitorPass.h"
#include "PassManager.h"

struct LoopCheckPass : VisitorPass<LoopCheckPass, void> {
    using AnnotationT = TreeNode*;

    void visitInfiniteLoop(InfiniteLoop *Loop);
    void visitConditionalLoop(ConditionalLoop *Loop);
    void visitBreak(Break *Break);
    void visitContinue(Continue *Continue);
    void runOnAST(ASTPassManager &PManager, ASTNodeT &Root);

    ASTPassManager *PM;

};