//
// Created by rajan on 11/10/22.
//


#pragma once


#include "Types/Type.h"
#include "VisitorPass.h"
#include "PassManager.h"
#include "ScopeResolutionPass.h"

/*
struct ExprTypeAnnotatorPass : VisitorPass<ExprTypeAnnotatorPass, const Type*> {
    using AnnotationT = const Type*;

    const Type *visitArithmeticOp(ArithmeticOp *Op);
    const Type *visitLogicalOp(ComparisonOp *Op);
    const Type

    const Type *visitOutStream(OutStream *OutStream);
    const Type *visitInStream(InStream *InStream);

    const Type *visitIdentifier(Identifier *Ident) const;

    void runOnAST(ASTPassManager &Manager, ASTNodeT *Root);

    explicit ExprTypeAnnotatorPass() {}

    ASTPassManager *PM;
};
 */