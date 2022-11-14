//
// Created by rajan on 11/07/22.
//

#include "Passes/ExprTypeAnnotatorPass.h"

const Type* ExprTypeAnnotatorPass::visitOutStream(OutStream *OutStream) {

}

const Type* ExprTypeAnnotatorPass::visitInStream(InStream *InStream) {

}

void ExprTypeAnnotatorPass::runOnAST(ASTPassManager &Manager, ASTNodeT *Root) {
    PM = &Manager;
    visit(Root);
}

const Type *ExprTypeAnnotatorPass::visitLogicalOp(ComparisonOp *Op) {
    auto LeftExpr = Op->getLeftExpr();
    auto RightExpr = Op->getRightExpr();
    auto LeftType = visit(LeftExpr);
    auto RightType = visit(RightExpr);

    assert(LeftType->isValidForLogicalOps())
}

const Type *ExprTypeAnnotatorPass::visitArithmeticOp(ArithmeticOp *Op) {
    auto LeftExpr = Op->getLeftExpr();
    auto RightExpr = Op->getRightExpr();
    auto LeftType = visit(LeftExpr);
    auto RightType = visit(RightExpr);

    assert(LeftType->isValidForArithOps() && "Left type does not"
                                             " support arithmetic ops");

    assert(RightType->isValidForArithOps() && "Right type does not "
                                              "support arithmetic ops");


    // If both operands are the same type.
    if (LeftType->isSameTypeAs(RightType)) {
        auto ExprType = PM->TypeReg.getConstTypeOf(LeftType);
        PM->setAnnotation<ExprTypeAnnotatorPass>(
                Op, ExprType);
        return ExprType;
    }

    // Both operands are not the same type. The only implicit cast is
    // from integers to reals.
    if (isa<IntegerTy>(LeftType) && isa<RealTy>(RightType)) {
        auto CastNode = PM->Builder.build<TypeCast>();
        auto TargetType = PM->TypeReg.getRealTy();
        LeftExpr->getParent()->replaceChildWith(LeftExpr, CastNode);
        CastNode->setExpr(LeftExpr);
        CastNode->setTargetType(TargetType);
        PM->setAnnotation<ExprTypeAnnotatorPass>(CastNode, TargetType);
        return TargetType;
    }

    if (isa<RealTy>(LeftType) && isa<IntegerTy>(RightType)) {
        auto CastNode = PM->Builder.build<TypeCast>();
        auto TargetType = PM->TypeReg.getRealTy();
        RightExpr->getParent()->replaceChildWith(RightExpr, CastNode);
        CastNode->setExpr(RightExpr);
        CastNode->setTargetType(TargetType);
        PM->setAnnotation<ExprTypeAnnotatorPass>(CastNode, TargetType);
        return TargetType;
    }

    throw std::runtime_error("incorrect types.");
}

const Type *ExprTypeAnnotatorPass::visitIdentifier(Identifier *Ident) const {
    assert(Ident->getIdentType() && "Identifier type unknown");
    PM->setAnnotation<ExprTypeAnnotatorPass>(Ident, Ident->getIdentType());
    return Ident->getIdentType();
}
