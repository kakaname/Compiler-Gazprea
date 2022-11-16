

//
// Created by rajan on 11/10/22.
//


#pragma once


#include "Types/Type.h"
#include "VisitorPass.h"
#include "PassManager.h"

struct ExprTypeAnnotatorPass : VisitorPass<ExprTypeAnnotatorPass, const Type*> {
    using AnnotationT = const Type*;

    const Type *visitArithmeticOp(ArithmeticOp *Op);
    const Type *visitComparisonOp(ComparisonOp *Op);
    const Type *visitLogicalOp(LogicalOp *Op);
    const Type *visitUnaryOp(UnaryOp *Op);
    const Type *visitIdentifier(Identifier *Ident) const;
    const Type *visitMemberAccess(MemberAccess *MAccess);
    const Type *visitFunctionCall(FunctionCall *Call);
    const Type *visitIntLiteral(IntLiteral *Int);
    const Type *visitRealLiteral(RealLiteral *Real);
    const Type *visitTupleLiteral(TupleLiteral *TupLit);
    const Type *visitTypeCast(TypeCast *Cast);
    const Type *visitExplicitCast(ExplicitCast *Cast);
    const Type *visitNullLiteral(NullLiteral *Null);
    const Type *visitIdentityLiteral(IdentityLiteral *Identity);



    TypeCast *wrapWithCastToReal(ASTNodeT *Expr) const;



    void runOnAST(ASTPassManager &Manager, ASTNodeT *Root);

    explicit ExprTypeAnnotatorPass() {}

    ASTPassManager *PM;
};