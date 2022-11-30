//
// Created by Chen on 2022-11-29.
//

#include "Passes/Transformations/VectorScalarOpTransformationPass.h"

void VectorScalarOpTransformationPass::visitComparisonOp(ComparisonOp *Op) {
    /*
    auto LeftExpr = Op->getLeftExpr();
    auto RightExpr = Op->getRightExpr();

    auto LeftExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(LeftExpr);
    auto RightExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(RightExpr);

    if (isa<VectorTy>(LeftExprTy) && isa<IntegerTy>(RightExprTy)) {
        auto VectorLit = PM->Builder.build<VectorLiteral>();

        auto LeftVectorTy = dyn_cast<VectorTy>(LeftExprTy);
        VectorLit->MemType = LeftVectorTy->getInnerTy();
        VectorLit->vecSize = LeftVectorTy->getSize();

        for (int I = 0; I < VectorLit->vecSize; I++) {
            VectorLit->addChild(RightExpr);
        }

        Op->setRightExpr(VectorLit);
        return;
    }

    if (isa<IntegerTy>(LeftExprTy) && isa<VectorTy>(RightExprTy)) {
        auto VectorLit = PM->Builder.build<VectorLiteral>();

        auto RightVectorTy = dyn_cast<VectorTy>(RightExprTy);
        VectorLit->MemType = RightVectorTy->getInnerTy();
        VectorLit->vecSize = RightVectorTy->getSize();

        for (int I = 0; I < VectorLit->vecSize; I++) {
            VectorLit->addChild(LeftExpr);
        }

        Op->setLeftExpr(VectorLit);
        return;
    }
     */
}

void VectorScalarOpTransformationPass::visitArithmeticOp(ArithmeticOp *Op) {

}

void VectorScalarOpTransformationPass::visitLogicalOp(LogicalOp *Op) {

}
