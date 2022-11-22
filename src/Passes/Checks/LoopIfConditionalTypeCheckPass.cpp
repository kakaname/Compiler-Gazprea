//
// Created by Chen on 2022-11-18.
//

#include "Passes/Checks/LoopIfConditionalTypeCheckPass.h"

void LoopIfConditionalTypeCheckPass::visitConditional(Conditional *Cond) {
    bool IsSameType = PM->getAnnotation<ExprTypeAnnotatorPass>(Cond->getConditional())->getKind() == Type::T_Bool;

    assert(IsSameType && "The condition type of if-stat is not bool");
}

void LoopIfConditionalTypeCheckPass::visitConditionalLoop(ConditionalLoop *Loop) {
    bool IsSameType = PM->getAnnotation<ExprTypeAnnotatorPass>(Loop->getConditional())->getKind() == Type::T_Bool;

    assert(IsSameType && "The condition type of loop is not bool");
}
