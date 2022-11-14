//
// Created by rajan on 11/10/22.
//


#pragma once


#include "Types/Type.h"
#include "VisitorPass.h"
#include "PassManager.h"
#include "ScopeResolutionPass.h"

struct ExprTypeAnnotatorPass : VisitorPass<ExprTypeAnnotatorPass, const Type*> {
    using AnnotationT = const Type*;

    const Type *visitArithmeticOp(ArithmeticOp *Op);
    const Type *visitComparisonOp(ComparisonOp *Op);
    const Type *visitLogicalOp(LogicalOp *Op);
    const Type *visitUnaryOp(UnaryOp *Op);

    TypeCast *wrapWithCastToReal(ASTNodeT *Expr) const;

    const Type *visitIdentifier(Identifier *Ident) const;

    void runOnAST(ASTPassManager &Manager, ASTNodeT *Root);

    explicit ExprTypeAnnotatorPass() {}

    ASTPassManager *PM;
};