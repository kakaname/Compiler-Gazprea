//
// Created by 陈 on 2022-11-15.
//

#include <string>
#include "Passes/BuildAST/NullIdentityTypeCastPass.h"

void NullIdentityTypeCastPass::visitTypeCast(TypeCast *Cast) {
    visit(Cast->getExpr());
    auto ExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Cast->getExpr());

    // Don't care if the type we are casting isn't null or identity.
    if (!isa<NullTy>(ExprTy) && !isa<IdentityTy>(ExprTy))
        return;

    bool IsNull = isa<NullTy>(ExprTy);
    auto IsTupleTy = isa<TupleTy>(Cast->getTargetType());
    auto IsIntervalTy = isa<IntervalTy>(Cast->getTargetType());

    if (IsTupleTy) {
        auto TupleLit = PM->Builder.build<TupleLiteral>();
        TupleLit->copyCtx(Cast);

        auto TupleType = cast<TupleTy>(Cast->getTargetType());
        size_t NumOfMembers = TupleType->getNumOfMembers();
        for (size_t I = 0; I < NumOfMembers; I++) {
            auto MemberType = TupleType->getMemberTypeAt(I);
            auto ExprAtPos = getScalarLiteral(MemberType, IsNull);
            ExprAtPos->copyCtx(Cast);
            TupleLit->setExprAtPos(ExprAtPos, I);
        }
        Cast->getParent()->replaceChildWith(Cast, TupleLit);
        PM->setAnnotation<ExprTypeAnnotatorPass>(TupleLit, Cast->getTargetType());
        return;
    } else if (IsIntervalTy) {
        auto IntervalLit = PM->Builder.build<Interval>();
        IntervalLit->copyCtx(Cast);

        auto IntervalType = cast<IntervalTy>(Cast->getTargetType());
        auto LowerBound = getScalarLiteral(IntervalType, IsNull);
        LowerBound->copyCtx(Cast);
        auto UpperBound = getScalarLiteral(IntervalType, IsNull);
        UpperBound->copyCtx(Cast);
        IntervalLit->setLowerExpr(LowerBound);
        IntervalLit->setUpperExpr(UpperBound);
        Cast->getParent()->replaceChildWith(Cast, IntervalLit);
        PM->setAnnotation<ExprTypeAnnotatorPass>(IntervalLit, Cast->getTargetType());
        return;
    }
    // Scalar Types
    auto NewLit = getScalarLiteral(Cast->TargetType, IsNull);
    NewLit->copyCtx(Cast);
    Cast->getParent()->replaceChildWith(Cast, NewLit);
    PM->setAnnotation<ExprTypeAnnotatorPass>(NewLit, Cast->getTargetType());
}

ASTNodeT *NullIdentityTypeCastPass::getScalarLiteral(Type* TargetTy, bool IsNull) {
    switch(TargetTy->getKind()) {
        case Type::T_Bool:
        {
            auto BoolLit = PM->Builder.build<BoolLiteral>();
            if (!IsNull)
                BoolLit->setTrue();
            return BoolLit;
        }
        case Type::T_Char:
        {
            auto CharLit = PM->Builder.build<CharLiteral>();
            IsNull ? CharLit->setCharacter(0x00) : CharLit->setCharacter(0x01);
            return CharLit;
        }
        case Type::T_Int:
        case Type::T_Interval:
        {
            auto IntLit = PM->Builder.build<IntLiteral>();
            IntLit->setIntVal(!IsNull);
            return IntLit;
        }
        case Type::T_Real: {
            auto RealLit = PM->Builder.build<RealLiteral>();
            if (IsNull)
                RealLit->setVal("0.0");
            else
                RealLit->setVal("1.0");
            return RealLit;
        }
        case Type::T_Matrix:
        case Type::T_Vector: {
            auto Lit = getScalarLiteral(TypeRegistry::getInnerTyFromComposite(TargetTy), IsNull);
            return wrapWithCastTo(Lit, TargetTy);
        }
        default:
            assert(false);
    }
}

TypeCast *NullIdentityTypeCastPass::wrapWithCastTo(ASTNodeT *Expr, Type *TargetType) {
    auto Cast = PM->Builder.build<TypeCast>();
    Cast->copyCtx(Expr);
    Cast->setExpr(Expr);
    Cast->setTargetType(TargetType);
    PM->setAnnotation<ExprTypeAnnotatorPass>(Cast, TargetType);
    return Cast;
}
