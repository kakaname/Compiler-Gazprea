//
// Created by dhanrajbir on 15/11/22.
//

#include "Passes/CallableArgumentTypeCheckingPass.h"
#include "Passes/ExprTypeAnnotatorPass.h"

void CallableArgumentTypeCheckingPass::visitFunctionCall(FunctionCall *Call) {
    visit(Call->getArgsList());
    auto CalleeType = Call->getIdentifier()->getIdentType();
    if (auto FuncTy = dyn_cast<FunctionTy>(CalleeType))
        return checkFuncCall(Call, FuncTy);

    if (auto ProcTy = dyn_cast<ProcedureTy>(CalleeType))
        return checkProcCall(Call, ProcTy);

    assert(false && "Calling a non-callable type?");
}

void CallableArgumentTypeCheckingPass::checkProcCall(FunctionCall *Call, const ProcedureTy *Ty) {
    assert(Call->getArgsList()->numOfChildren() == Ty->getNumOfArgs() &&
    "Incorrect number of arguments");

    for (int I = 0; I < Ty->getNumOfArgs(); I++) {
        auto ParamType = Ty->getParamTypeAt(I);
        auto Expr = Call->getArgsList()->getExprAtPos(I);
        auto ExprType = PM->getAnnotation<ExprTypeAnnotatorPass>(Expr);
        if (!ParamType->isConst()) {
            assert(isa<Identifier>(Expr) ||
                    isa<MemberAccess>(Expr)
                    && "Only lvalues may bind to var argument types.");
            assert(!ExprType->isConst() && "incorrect argument type.");
            continue;
        }

        if (ParamType->isSameTypeAs(ExprType))
            continue;

        assert(ExprType->canPromoteTo(ParamType) && "Invalid type for argument.");
        auto Cast = wrapWithCastTo(Expr, ParamType);
        Call->getArgsList()->setExprAtPos(Cast, I);
    }

}

TypeCast *CallableArgumentTypeCheckingPass::wrapWithCastTo(ASTNodeT *Expr, const Type *Ty) const {
    auto Cast = PM->Builder.build<TypeCast>();
    Cast->setExpr(Expr);
    Cast->setTargetType(Ty);
    PM->setAnnotation<ExprTypeAnnotatorPass>(Cast, Ty);
    return Cast;
}

void CallableArgumentTypeCheckingPass::checkFuncCall(FunctionCall *Call, const FunctionTy *Ty) {
    assert(Call->getArgsList()->numOfChildren() == Ty->getNumOfArgs() &&
           "Incorrect number of arguments");

    for (int I = 0; I < Ty->getNumOfArgs(); I++) {
        auto ParamType = Ty->getParamTypeAt(I);
        auto Expr = Call->getArgsList()->getExprAtPos(I);
        auto ExprType = PM->getAnnotation<ExprTypeAnnotatorPass>(Expr);

        if (ParamType->isSameTypeAs(ExprType))
            continue;

        assert(ExprType->canPromoteTo(ParamType) && "Invalid type for argument.");
        auto Cast = wrapWithCastTo(Expr, ParamType);
        Call->getArgsList()->setExprAtPos(Cast, I);
    }
}
