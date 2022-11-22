//
// Created by 陈 on 2022-11-14.
//

#include "Passes/Checks/ExplicitCastCheckPass.h"
#include "Types/Type.h"
#include "AST/ASTNodes.h"

void ExplicitCastCheckPass::visitExplicitCast(ExplicitCast *ExplicitCast) {
    auto ExprType = PM->getAnnotation<ExprTypeAnnotatorPass>(ExplicitCast->getExpr());
    auto TargetType = ExplicitCast->TargetType;

    if (!ExprType->canCastTo(TargetType))
        throw CastError(ExplicitCast, ExprType->getTypeName(), TargetType->getTypeName());
}
