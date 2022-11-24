//
// Created by dhanrajbir on 16/11/22.
//


#include "Passes/Transformations/SimplifyTupleCasting.h"
#include "Passes/Transformations/SubExpressionCacheSet.h"

void SimplifyTupleCasting::visitTypeCast(TypeCast *Cast) {
    visit(Cast->getExpr());
    auto TargetTy = dyn_cast<TupleTy>(Cast->getTargetType());
    if (!TargetTy)
        return;

    auto Literal = PM->Builder.build<TupleLiteral>();
    PM->getResource<SubExpressionCacheSet>().addCachedNode(Cast->getExpr());
    Literal->copyCtx(Cast);
    for (int I = 0; I < TargetTy->getNumOfMembers(); I++) {
        auto MemCastTarget = TargetTy->getMemberTypeAt(I);
        auto MemExpr = buildMemberAccess(Cast->getExpr(), I+1);
        auto Casted = wrapWithCastTo(MemExpr, MemCastTarget);
        Literal->addChild(Casted);
    }
    PM->setAnnotation<ExprTypeAnnotatorPass>(Literal, TargetTy);
    Cast->getParent()->replaceChildWith(Cast, Literal);
}

void SimplifyTupleCasting::visitExplicitCast(ExplicitCast *Cast) {
    visit(Cast->getExpr());
    auto TargetTy = dyn_cast<TupleTy>(Cast->getTargetType());
    if (!TargetTy)
        return;

    auto Literal = PM->Builder.build<TupleLiteral>();
    PM->getResource<SubExpressionCacheSet>().addCachedNode(Cast->getExpr());

    Literal->copyCtx(Cast);
    for (int I = 0; I < TargetTy->getNumOfMembers(); I++) {
        auto MemCastTarget = TargetTy->getMemberTypeAt(I);
        auto MemExpr = buildMemberAccess(Cast->getExpr(), I+1);
        auto Casted = wrapWithCastTo(MemExpr, MemCastTarget);
        Literal->addChild(Casted);
    }
    PM->setAnnotation<ExprTypeAnnotatorPass>(Literal, TargetTy);
    Cast->getParent()->replaceChildWith(Cast, Literal);
}

TypeCast *SimplifyTupleCasting::wrapWithCastTo(ASTNodeT *Expr, const Type *Target) const {
    auto Cast = PM->Builder.build<TypeCast>();
    Cast->copyCtx(Expr);
    Cast->setExpr(Expr);
    Cast->setTargetType(Target);
    PM->setAnnotation<ExprTypeAnnotatorPass>(Cast, Target);
    return Cast;
}

MemberAccess *SimplifyTupleCasting::buildMemberAccess(ASTNodeT *BaseExpr, int Idx) const {
    auto IntLit = PM->Builder.build<IntLiteral>();
    IntLit->copyCtx(BaseExpr);
    IntLit->setIntVal(Idx);

    auto Access = PM->Builder.build<MemberAccess>();
    Access->copyCtx(BaseExpr);
    Access->setExpr(BaseExpr);
    Access->setMemberExpr(IntLit);

    return Access;
}
