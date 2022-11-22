//
// Created by 陈 on 2022-11-14.
//

#include "Passes/Transformations/TupleNotEqualTransformationPass.h"

void TupleNotEqualTransformationPass::visitLogicalOp(LogicalOp *Op) {
    auto LeftExpr = Op->getLeftExpr();
    auto RightExpr = Op->getRightExpr();

    // We only care about NEQ Operation on two tuples in this pass
    if (!isa<TupleTy>(PM->getAnnotation<ExprTypeAnnotatorPass>(LeftExpr)) ||
        !isa<TupleTy>(PM->getAnnotation<ExprTypeAnnotatorPass>(RightExpr)) ||
        Op->getOpKind() != LogicalOp::NEQ) {
        return;
    }
    // change (tuple != tuple) to not(tuple == tuple)
    auto NotOp = PM->Builder.build<UnaryOp>();
    NotOp->copyCtx(Op);
    NotOp->setOp(UnaryOp::NOT);

    Op->setOp(LogicalOp::EQ);
    Op->getParent()->replaceChildWith(Op, NotOp);
    NotOp->setExpr(Op);
}
