//
// Created by dhanrajbir on 15/11/22.
//

#include "Passes/CallableArgumentTypeCheckingPass.h"
#include "Passes/ExprTypeAnnotatorPass.h"

void CallableArgumentTypeCheckingPass::visitFunctionCall(FunctionCall *Call) {
    visit(Call->getArgsList());
    auto Ty = cast<FunctionTy>(Call->getIdentifier()->getIdentType());
    if (Call->getArgsList()->numOfChildren() != Ty->getNumOfArgs())
        throw ArgumentCountError(
                Call, Call->getIdentifier()->getName(), Ty->getNumOfArgs(),
                Call->getArgsList()->numOfChildren());

    for (int I = 0; I < Ty->getNumOfArgs(); I++) {
        auto ParamType = Ty->getParamTypeAt(I);
        auto Expr = Call->getArgsList()->getExprAtPos(I);
        auto ExprType = PM->getAnnotation<ExprTypeAnnotatorPass>(Expr);

        if (ParamType->isSameTypeAs(ExprType))
            continue;

        if (!ExprType->canPromoteTo(ParamType))
            throw ScalarPromotionError(Call, ExprType->getTypeName(), ParamType->getTypeName());

        auto Cast = wrapWithCastTo(Expr, ParamType);
        Call->getArgsList()->setExprAtPos(Cast, I);
    }
}

TypeCast *CallableArgumentTypeCheckingPass::wrapWithCastTo(ASTNodeT *Expr, const Type *Ty) const {
    auto Cast = PM->Builder.build<TypeCast>();
    Cast->copyCtx(Expr);
    Cast->setExpr(Expr);
    Cast->setTargetType(Ty);
    PM->setAnnotation<ExprTypeAnnotatorPass>(Cast, Ty);
    return Cast;
}


void CallableArgumentTypeCheckingPass::visitProcedureCall(ProcedureCall *Call) {
    visit(Call->getArgsList());
    auto Ty = cast<ProcedureTy>(Call->getIdentifier()->getIdentType());

    if (Call->getArgsList()->numOfChildren() != Ty->getNumOfArgs())
        throw ArgumentCountError(Call, Call->getIdentifier()->getName(), Ty->getNumOfArgs(), Call->getArgsList()->numOfChildren());

    for (int I = 0; I < Ty->getNumOfArgs(); I++) {
        auto ParamType = Ty->getParamTypeAt(I);
        auto Expr = Call->getArgsList()->getExprAtPos(I);
        auto ExprType = PM->getAnnotation<ExprTypeAnnotatorPass>(Expr);
        if (!ParamType->isConst()) {
            if(!isa<Identifier>(Expr) && !isa<MemberAccess>(Expr))
                throw VariableArgumentError(Call, I + 1, Call->getIdentifier()->getName());

            if (!ParamType->isSameTypeAs(ExprType))
                throw VariableArgumentError(Call, I+1, Call->getIdentifier()->getName());

            if (ExprType->isConst())
                throw ConstantArgumentError(Call, I + 1, Call->getIdentifier()->getName());

            continue;
        }

        if (ParamType->isSameTypeAs(ExprType))
            continue;

        if (!ExprType->canPromoteTo(ParamType))
            throw ScalarPromotionError(Call, ExprType->getTypeName(), ParamType->getTypeName());

        auto Cast = wrapWithCastTo(Expr, ParamType);
        Call->getArgsList()->setExprAtPos(Cast, I);
    }
}
