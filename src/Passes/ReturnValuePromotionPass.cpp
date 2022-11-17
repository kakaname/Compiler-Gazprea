//
// Created by dhanrajbir on 16/11/22.
//

#include "Passes/ReturnValuePromotionPass.h"
#include "Passes/ExprTypeAnnotatorPass.h"

void ReturnValuePromotionPass::visitFunctionDef(FunctionDef *Def) {
    ReturnTy = Def->getRetTy();
    visit(Def->getBlock());
}

void ReturnValuePromotionPass::visitProcedureDef(ProcedureDef *Def) {
    ReturnTy = Def->getRetTy();
    visit(Def->getBlock());
}

void ReturnValuePromotionPass::visitReturn(Return *Ret) {
    if (!ReturnTy) {
        if (!isa<NoOp>(Ret->getReturnExpr()))
            throw NoReturnError(Ret);
        return;
    }

    auto RetExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(
            Ret->getReturnExpr());
    if (RetExprTy->isSameTypeAs(ReturnTy))
        return;

    if(!RetExprTy->canPromoteTo(ReturnTy))
        throw FunctionReturnTypeError(Ret, ReturnTy->getTypeName(), RetExprTy->getTypeName());

    auto Cast = wrapWithCastTo(Ret->getReturnExpr(), ReturnTy);
    Ret->setReturnExpr(Cast);
}

TypeCast *ReturnValuePromotionPass::wrapWithCastTo(ASTNodeT *Expr, const Type *Ty) const {
    auto Cast = PM->Builder.build<TypeCast>();
    Cast->setExpr(Expr);
    Cast->setTargetType(Ty);
    PM->setAnnotation<ExprTypeAnnotatorPass>(Cast, Ty);
    return Cast;
}
