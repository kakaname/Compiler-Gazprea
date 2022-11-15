//
// Created by 陈 on 2022-11-14.
//

#include "Passes/TupleExplicitTypeCastPass.h"

void TupleExplicitTypeCastPass::visitExplicitCast(ExplicitCast *ExplicitCast) {


    // If it is not a tuple explicit casting, then we ignore it
    if(!isa<TupleTy>(PM->getAnnotation<ExprTypeAnnotatorPass>(ExplicitCast->getExpr())))
        return;

    assert(isa<TupleTy>(ExplicitCast->TargetType) && "Explicit Type Casting was invalid");


    auto ExprType = dyn_cast<TupleTy>(PM->getAnnotation<ExprTypeAnnotatorPass>(ExplicitCast->getExpr()));
    auto TargetType = dyn_cast<TupleTy>(ExplicitCast->TargetType);

    assert(ExprType->getNumOfMembers() == TargetType->getNumOfMembers() && "The size of tuples didn't match");

}
