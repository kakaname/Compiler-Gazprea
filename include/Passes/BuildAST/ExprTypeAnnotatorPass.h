

//
// Created by rajan on 11/10/22.
//


#pragma once


#include "Types/Type.h"
#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"
#include "Passes/BuildAST/ConvertIdentMemberAccessToIdxPass.h"
#include "Common/MatchBoolPair.h"

struct ExprTypeAnnotatorPass : VisitorPass<ExprTypeAnnotatorPass, Type*> {
    using AnnotationT = Type*;

    Type *visitArithmeticOp(ArithmeticOp *Op);
    Type *visitComparisonOp(ComparisonOp *Op);
    Type *visitLogicalOp(LogicalOp *Op);
    Type *visitUnaryOp(UnaryOp *Op);
    Type *visitIdentifier(Identifier *Ident) const;
    Type *visitMemberAccess(MemberAccess *MAccess);
    Type *visitIndex(Index *Idx);
    Type *visitIndexReference(IndexReference *IdxRef);
    Type *visitMemberReference(MemberReference *Ref);
    Type *visitIdentReference(IdentReference *Ref);
    Type *visitFunctionCall(FunctionCall *Call);
    Type *visitProcedureCall(ProcedureCall *Call);
    Type *visitIntLiteral(IntLiteral *Int) const;
    Type *visitRealLiteral(RealLiteral *Real) const ;
    Type *visitTupleLiteral(TupleLiteral *TupLit);
    Type *visitVectorLiteral(VectorLiteral *VecLit);
    Type *visitTypeCast(TypeCast *Cast);
    Type *visitExplicitCast(ExplicitCast *Cast);
    Type *visitNullLiteral(NullLiteral *Null);
    Type *visitIdentityLiteral(IdentityLiteral *Identity);
    Type *visitBoolLiteral(BoolLiteral *Bool);
    Type *visitCharLiteral(CharLiteral *Char);
    Type *visitInterval(Interval *Int);
    Type *visitByOp(ByOp *By);
    Type *visitDotProduct(DotProduct *Dot);
    Type *visitConcat(Concat *Concat);
    Type *visitOutStream(OutStream *Out);
    Type *visitConditionalLoop(ConditionalLoop *Loop);
    Type *visitConditional(Conditional *Cond);
    Type *visitConditionalElse(ConditionalElse *Cond);


    void setOpaqueTyCastTargetTy(Type *Ty) {
        OpaqueTyCastTarget = Ty;
    }

    void annotate(ASTNodeT *Node, Type *Ty) const {
        PM->setAnnotation<ExprTypeAnnotatorPass>(Node, Ty);
    }

    void annotateWithConst(ASTNodeT *Node, Type *Ty) const {
        annotate(Node, PM->TypeReg.getConstTypeOf(Ty));
    }

    TypeCast *wrapWithCastTo(ASTNodeT *Expr, Type *Target) const;

    static Type *getWiderType(Type *Ty1, Type *Ty2);

    static bool isTypeSizeKnown(Type *Ty);

    IntLiteral *getIntLiteralWithVal(long Val);

    ArithmeticOp *getAddOpBetween(ASTNodeT* N1, ASTNodeT* N2);

    void visitTypeSizeExpressions(Type*);

    void runOnAST(ASTPassManager &Manager, ASTNodeT *Root);

    explicit ExprTypeAnnotatorPass() = default;

    static bool isInferredSizedType(Type *Ty);
    void copyOverCompositeSizeTypes(Type *Src, Type *Dest);

    ASTPassManager *PM{};
    TypeRegistry *TypeReg{};
    Type *OpaqueTyCastTarget{nullptr};
};