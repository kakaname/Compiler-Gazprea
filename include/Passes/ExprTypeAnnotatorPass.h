//
// Created by rajan on 11/10/22.
//


#pragma once


#include "Types/Type.h"
#include "VisitorPass.h"
#include "PassManager.h"

struct ExprTypeAnnotatorPass : VisitorPass<ExprTypeAnnotatorPass, Type*> {
    using AnnotationT = Type*;
    Type*  visitOutStream(OutStream *OutStream);
    Type*  visitInStream(InStream *InStream);

    void runOnAST(ASTPassManager &Manager, ASTNodeT &Root);

    ASTPassManager *PM;
};