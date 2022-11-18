

//
// Created by rajan on 11/10/22.
//


#pragma once


#include "Types/Type.h"
#include "VisitorPass.h"
#include "PassManager.h"
#include "ConvertIdentMemberAccessToIdxPass.h"

struct ExprTypeAnnotatorPass : VisitorPass<ExprTypeAnnotatorPass, const Type*> {
    using AnnotationT = const Type*;

    const Type *visitArithmeticOp(ArithmeticOp *Op);
    const Type *visitComparisonOp(ComparisonOp *Op);
    const Type *visitLogicalOp(LogicalOp *Op);
    const Type *visitUnaryOp(UnaryOp *Op);
    const Type *visitIdentifier(Identifier *Ident) const;
    const Type *visitMemberAccess(MemberAccess *MAccess);
    const Type *visitMemberReference(MemberReference *Ref);
    const Type *visitIdentReference(IdentReference *Ref);
    const Type *visitFunctionCall(FunctionCall *Call);
    const Type *visitIntLiteral(IntLiteral *Int) const;
    const Type *visitRealLiteral(RealLiteral *Real) const ;
    const Type *visitTupleLiteral(TupleLiteral *TupLit);
    const Type *visitTypeCast(TypeCast *Cast);
    const Type *visitExplicitCast(ExplicitCast *Cast);
    const Type *visitNullLiteral(NullLiteral *Null);
    const Type *visitIdentityLiteral(IdentityLiteral *Identity);
    const Type *visitBoolLiteral(BoolLiteral *Bool);
    const Type *visitCharLiteral(CharLiteral *Char);


    void setOpaqueTyCastTargetTy(const Type *Ty) {
        OpaqueTyCastTarget = Ty;
    }

    void annotate(ASTNodeT *Node, const Type *Ty) const {
        PM->setAnnotation<ExprTypeAnnotatorPass>(Node, Ty);
    }

    void annotateWithConst(ASTNodeT *Node, const Type *Ty) const {
        annotate(Node, PM->TypeReg.getConstTypeOf(Ty));
    }

    TypeCast *wrapWithCastTo(ASTNodeT *Expr, const Type *Target) const;


    void runOnAST(ASTPassManager &Manager, ASTNodeT *Root);

    explicit ExprTypeAnnotatorPass() = default;

    ASTPassManager *PM{};
    const Type *OpaqueTyCastTarget{nullptr};
};