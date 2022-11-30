//
// Created by Chen on 2022-11-29.
//

#include "Passes/Transformations/VectorScalarOpTransformationPass.h"

void VectorScalarOpTransformationPass::visitComparisonOp(ComparisonOp *Op) {

    auto LeftExpr = Op->getLeftExpr();
    auto RightExpr = Op->getRightExpr();

    auto LeftExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(LeftExpr);
    auto RightExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(RightExpr);


    if (isa<VectorTy>(LeftExprTy) && isScalarTy(RightExprTy)) {
        auto LeftVectorTy = dyn_cast<VectorTy>(LeftExprTy);

        // Create the Generator Node
        auto Gen = PM->Builder.build<Generator>();

        // Declare a new symbol
        auto NewSym = PM->SymTable.defineObject("I", LeftVectorTy->getInnerTy());

        auto DomainVar = PM->Builder.build<Identifier>();
        DomainVar->setReferred(NewSym);
        Gen->setDomainVariable(DomainVar);

        auto ExprDomainVar = PM->Builder.build<Identifier>();
        ExprDomainVar->setReferred(NewSym);

        auto GenExpr = PM->Builder.build<ComparisonOp>();
        GenExpr->setOp(Op->getOpKind());
        GenExpr->setLeftExpr(ExprDomainVar);
        GenExpr->setRightExpr(RightExpr);

        Op->getParent()->replaceChildWith(Op, Gen);
    }

    if (isScalarTy(LeftExprTy) && isa<VectorTy>(RightExprTy)) {
        auto RightVectorTy = dyn_cast<VectorTy>(RightExprTy);

        // Create the Generator Node
        auto Gen = PM->Builder.build<Generator>();

        // Declare a new symbol
        auto NewSym = PM->SymTable.defineObject("I", RightVectorTy->getInnerTy());

        auto DomainVar = PM->Builder.build<Identifier>();
        DomainVar->setReferred(NewSym);
        Gen->setDomainVariable(DomainVar);

        auto ExprDomainVar = PM->Builder.build<Identifier>();
        ExprDomainVar->setReferred(NewSym);

        auto GenExpr = PM->Builder.build<ComparisonOp>();
        GenExpr->setOp(Op->getOpKind());
        GenExpr->setLeftExpr(ExprDomainVar);
        GenExpr->setRightExpr(LeftExpr);

        Op->getParent()->replaceChildWith(Op, Gen);
    }

}

void VectorScalarOpTransformationPass::visitArithmeticOp(ArithmeticOp *Op) {

    auto LeftExpr = Op->getLeftExpr();
    auto RightExpr = Op->getRightExpr();

    auto LeftExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(LeftExpr);
    auto RightExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(RightExpr);


    if (isa<VectorTy>(LeftExprTy) && isScalarTy(RightExprTy)) {
        auto LeftVectorTy = dyn_cast<VectorTy>(LeftExprTy);

        // Create the Generator Node
        auto Gen = PM->Builder.build<Generator>();

        // Declare a new symbol
        auto NewSym = PM->SymTable.defineObject("I", LeftVectorTy->getInnerTy());

        auto DomainVar = PM->Builder.build<Identifier>();
        DomainVar->setReferred(NewSym);
        Gen->setDomainVariable(DomainVar);

        auto ExprDomainVar = PM->Builder.build<Identifier>();
        ExprDomainVar->setReferred(NewSym);

        auto GenExpr = PM->Builder.build<ArithmeticOp>();
        GenExpr->setOp(Op->getOpKind());
        GenExpr->setLeftExpr(ExprDomainVar);
        GenExpr->setRightExpr(RightExpr);

        Op->getParent()->replaceChildWith(Op, Gen);
    }

    if (isScalarTy(LeftExprTy) && isa<VectorTy>(RightExprTy)) {
        auto RightVectorTy = dyn_cast<VectorTy>(RightExprTy);

        // Create the Generator Node
        auto Gen = PM->Builder.build<Generator>();

        // Declare a new symbol
        auto NewSym = PM->SymTable.defineObject("I", RightVectorTy->getInnerTy());

        auto DomainVar = PM->Builder.build<Identifier>();
        DomainVar->setReferred(NewSym);
        Gen->setDomainVariable(DomainVar);

        auto ExprDomainVar = PM->Builder.build<Identifier>();
        ExprDomainVar->setReferred(NewSym);

        auto GenExpr = PM->Builder.build<ArithmeticOp>();
        GenExpr->setOp(Op->getOpKind());
        GenExpr->setLeftExpr(ExprDomainVar);
        GenExpr->setRightExpr(LeftExpr);

        Op->getParent()->replaceChildWith(Op, Gen);
    }
}

void VectorScalarOpTransformationPass::visitLogicalOp(LogicalOp *Op) {
    auto LeftExpr = Op->getLeftExpr();
    auto RightExpr = Op->getRightExpr();

    auto LeftExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(LeftExpr);
    auto RightExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(RightExpr);


    if (isa<VectorTy>(LeftExprTy) && isScalarTy(RightExprTy)) {
        auto LeftVectorTy = dyn_cast<VectorTy>(LeftExprTy);

        // Create the Generator Node
        auto Gen = PM->Builder.build<Generator>();

        // Declare a new symbol
        auto NewSym = PM->SymTable.defineObject("I", LeftVectorTy->getInnerTy());

        auto DomainVar = PM->Builder.build<Identifier>();
        DomainVar->setReferred(NewSym);
        Gen->setDomainVariable(DomainVar);

        auto ExprDomainVar = PM->Builder.build<Identifier>();
        ExprDomainVar->setReferred(NewSym);

        auto GenExpr = PM->Builder.build<LogicalOp>();
        GenExpr->setOp(Op->getOpKind());
        GenExpr->setLeftExpr(ExprDomainVar);
        GenExpr->setRightExpr(RightExpr);

        Op->getParent()->replaceChildWith(Op, Gen);
    }

    if (isScalarTy(LeftExprTy) && isa<VectorTy>(RightExprTy)) {
        auto RightVectorTy = dyn_cast<VectorTy>(RightExprTy);

        // Create the Generator Node
        auto Gen = PM->Builder.build<Generator>();

        // Declare a new symbol
        auto NewSym = PM->SymTable.defineObject("I", RightVectorTy->getInnerTy());

        auto DomainVar = PM->Builder.build<Identifier>();
        DomainVar->setReferred(NewSym);
        Gen->setDomainVariable(DomainVar);

        auto ExprDomainVar = PM->Builder.build<Identifier>();
        ExprDomainVar->setReferred(NewSym);

        auto GenExpr = PM->Builder.build<LogicalOp>();
        GenExpr->setOp(Op->getOpKind());
        GenExpr->setLeftExpr(ExprDomainVar);
        GenExpr->setRightExpr(LeftExpr);

        Op->getParent()->replaceChildWith(Op, Gen);
    }
}

bool VectorScalarOpTransformationPass::isScalarTy(const Type *Ty) {
    return isa<IntegerTy>(Ty) || isa<RealTy>(Ty) || isa<BoolTy>(Ty);
}
