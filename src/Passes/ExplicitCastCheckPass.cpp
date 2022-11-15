//
// Created by 陈 on 2022-11-14.
//

#include "Passes/ExplicitCastCheckPass.h"
#include "Types/Type.h"
#include "AST/ASTNodes.h"

void ExplicitCastCheckPass::visitExplicitCast(ExplicitCast *ExplicitCast) {
    auto ExprType = dyn_cast<TupleTy>(PM->getAnnotation<ExprTypeAnnotatorPass>(ExplicitCast->getExpr()));
    auto TargetType = dyn_cast<TupleTy>(ExplicitCast->TargetType);

    assert(ExprType->canCastTo(TargetType) && "Explicit Type Casting was invalid");
}
