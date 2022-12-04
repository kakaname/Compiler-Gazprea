//
// Created by Chen on 2022-12-01.
//

#include "Passes/Transformations/VectorScalarAppendTransformationPass.h"

void VectorScalarAppendTransformationPass::visitComparisonOp(ComparisonOp *Op) {
    auto LeftExpr = Op->getLeftExpr();
    auto RightExpr = Op->getRightExpr();

    auto LeftExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(LeftExpr);
    auto RightExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(RightExpr);

    // The only case we care about
    if (isa<VectorTy>(LeftExprTy) && isScalarTy(RightExprTy)) {
        // Add a VectorLiteral
        auto VectorLit = PM->Builder.build<VectorLiteral>();

        VectorLit->vecSize = 1;
        VectorLit->MemType = RightExprTy;
        VectorLit->addChild(RightExpr);

        Op->setRightExpr(VectorLit);
    }
}

void VectorScalarAppendTransformationPass::visitArithmeticOp(ArithmeticOp *Op) {
    auto LeftExpr = Op->getLeftExpr();
    auto RightExpr = Op->getRightExpr();

    auto LeftExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(LeftExpr);
    auto RightExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(RightExpr);

    // The only case we care about
    if (isa<VectorTy>(LeftExprTy) && isScalarTy(RightExprTy)) {
        // Add a VectorLiteral
        auto VectorLit = PM->Builder.build<VectorLiteral>();

        VectorLit->vecSize = 1;
        VectorLit->MemType = RightExprTy;
        VectorLit->addChild(RightExpr);

        Op->setRightExpr(VectorLit);
    }
}

void VectorScalarAppendTransformationPass::visitLogicalOp(LogicalOp *Op) {
    auto LeftExpr = Op->getLeftExpr();
    auto RightExpr = Op->getRightExpr();

    auto LeftExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(LeftExpr);
    auto RightExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(RightExpr);

    // The only case we care about
    if (isa<VectorTy>(LeftExprTy) && isScalarTy(RightExprTy)) {
        // Add a VectorLiteral
        auto VectorLit = PM->Builder.build<VectorLiteral>();

        VectorLit->vecSize = 1;
        VectorLit->MemType = RightExprTy;
        VectorLit->addChild(RightExpr);

        Op->setRightExpr(VectorLit);
    }
}

bool VectorScalarAppendTransformationPass::isScalarTy(const Type *Ty) {
    return isa<IntegerTy>(Ty) || isa<RealTy>(Ty) || isa<BoolTy>(Ty);
}
