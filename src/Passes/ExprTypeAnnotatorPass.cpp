//
// Created by rajan on 11/07/22.
//

#include "Passes/ExprTypeAnnotatorPass.h"


void ExprTypeAnnotatorPass::runOnAST(ASTPassManager &Manager, ASTNodeT *Root) {
    PM = &Manager;
    visit(Root);
}

const Type *ExprTypeAnnotatorPass::visitComparisonOp(ComparisonOp *Op) {
    auto LeftExpr = Op->getLeftExpr();
    auto RightExpr = Op->getRightExpr();
    auto LeftType = visit(LeftExpr);
    auto RightType = visit(RightExpr);

    assert(LeftType->isValidForComparisonOp() && "Left type does not "
                                                 "support comparison ops");
    assert(RightType->isValidForComparisonOp() && "Right type does not "
                                                  "support comparison ops");

    if (isa<IntegerTy>(LeftType) && isa<RealTy>(RightType)) {
        auto Cast = wrapWithCastToReal(LeftExpr);
        Op->setLeftExpr(Cast);
        PM->setAnnotation<ExprTypeAnnotatorPass>(Op, Cast->getTargetType());
        return Cast->getTargetType();
    }

    if (isa<RealTy>(LeftType) && isa<IntegerTy>(RightType)) {
        auto Cast = wrapWithCastToReal(RightExpr);
        Op->setRightExpr(Cast);
        PM->setAnnotation<ExprTypeAnnotatorPass>(Op, Cast->getTargetType());
        return Cast->getTargetType();
    }

    PM->setAnnotation<ExprTypeAnnotatorPass>(Op, PM->TypeReg.getBooleanTy());
    return PM->TypeReg.getBooleanTy();
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
        auto Cast = wrapWithCastToReal(LeftExpr);
        Op->setLeftExpr(Cast);
        PM->setAnnotation<ExprTypeAnnotatorPass>(Op, Cast->getTargetType());
        return Cast->getTargetType();
    }

    if (isa<RealTy>(LeftType) && isa<IntegerTy>(RightType)) {
        auto Cast = wrapWithCastToReal(RightExpr);
        Op->setRightExpr(Cast);
        PM->setAnnotation<ExprTypeAnnotatorPass>(Op, Cast->getTargetType());
        return Cast->getTargetType();
    }

    throw std::runtime_error("incorrect types.");
}

const Type *ExprTypeAnnotatorPass::visitIdentifier(Identifier *Ident) const {
    assert(Ident->getIdentType() && "Identifier type unknown");
    PM->setAnnotation<ExprTypeAnnotatorPass>(Ident, Ident->getIdentType());
    return Ident->getIdentType();
}

const Type *ExprTypeAnnotatorPass::visitLogicalOp(LogicalOp *Op) {
    auto LeftExpr = Op->getLeftExpr();
    auto RightExpr = Op->getRightExpr();
    auto LeftType = visit(LeftExpr);
    auto RightType = visit(RightExpr);

    if (Op->getOpKind() == LogicalOp::EQ || Op->getOpKind() == LogicalOp::NEQ) {
        assert(LeftType->isValidForEq() && "Left type does not support");
        assert(RightType->isValidForEq() && "Right type does not support");

        if (isa<IntegerTy>(LeftType) && isa<RealTy>(RightType)) {
            auto Cast = wrapWithCastToReal(LeftExpr);
            Op->setLeftExpr(Cast);
            LeftType = Cast->getTargetType();
        }

        if (isa<RealTy>(LeftType) && isa<IntegerTy>(RightType)) {
            auto Cast = wrapWithCastToReal(RightExpr);
            Op->setRightExpr(Cast);
            RightType = Cast->getTargetType();
        }

        assert(LeftType->isSameTypeAs(RightType) && "Cannot compare incompatible types");
        PM->setAnnotation<ExprTypeAnnotatorPass>(Op, PM->TypeReg.getBooleanTy());
        return PM->TypeReg.getBooleanTy();

    } else {
        // All other "logical ops" are only supported for booleans.
        assert(isa<BoolTy>(LeftType) && "Left type must be boolean");
        assert(isa<BoolTy>(RightType) && "Right type must be boolean");
        PM->setAnnotation<ExprTypeAnnotatorPass>(Op, PM->TypeReg.getBooleanTy());
        return PM->TypeReg.getBooleanTy();
    }
}

TypeCast *ExprTypeAnnotatorPass::wrapWithCastToReal(ASTNodeT *Expr) const {
    auto Cast = PM->Builder.build<TypeCast>();
    auto TargetType = PM->TypeReg.getRealTy();
    Cast->setExpr(Expr);
    Cast->setTargetType(TargetType);
    PM->setAnnotation<ExprTypeAnnotatorPass>(Cast, TargetType);
    return Cast;
}

const Type *ExprTypeAnnotatorPass::visitUnaryOp(UnaryOp *Op) {
    auto ChildType = visit(Op->getExpr());
    if (Op->getOpKind() == UnaryOp::NOT)
        assert(ChildType->isValidForUnaryNot() && "Type does not support not");
    else
        assert(ChildType->isValidForUnaryAddOrSub() && "Type does not "
                                                       "support unary add or sub");

    PM->setAnnotation<ExprTypeAnnotatorPass>(Op, PM->TypeReg.getBooleanTy());
    return PM->TypeReg.getBooleanTy();
}

const Type *ExprTypeAnnotatorPass::visitMemberAccess(MemberAccess *MAccess) {
    visit(MAccess->getExpr());
    visit(MAccess->getMemberExpr());
    auto IdentType = MAccess->getExpr()->getIdentType();
    assert(IdentType && "Type not assigned to identifier.");
    auto Tuple = dyn_cast<TupleTy>(IdentType);
    assert(Tuple && "Only identifier that are of type tuple maybe have their members accessed");
    auto MemberIdx = dyn_cast<IntLiteral>(MAccess->getMemberExpr());
    assert(MemberIdx && "Only member accesses with integer literals "
                        "should have reached this place.");

    assert((int32_t) Tuple->getNumOfMembers() <= MemberIdx->getVal() && "Invalid index to access a member");
    auto ResultTy = Tuple->getMemberTypeAt(MemberIdx->getVal());
    PM->setAnnotation<ExprTypeAnnotatorPass>(MAccess, ResultTy);
    return ResultTy;
}

const Type *ExprTypeAnnotatorPass::visitFunctionCall(FunctionCall *Call) {
    visit(Call->getArgsList());
    auto IdentTy = Call->getIdentifier()->getIdentType();
    assert(IdentTy && "Ident type not set for function call");
    auto FuncTy = dyn_cast<FunctionTy>(IdentTy);
    assert(FuncTy && "Only functions may be called");
    auto RetTy = FuncTy->getRetType();
    PM->setAnnotation<ExprTypeAnnotatorPass>(Call, RetTy);
    return RetTy;
}

const Type *ExprTypeAnnotatorPass::visitIntLiteral(IntLiteral *Int) {
    PM->setAnnotation<ExprTypeAnnotatorPass>(Int, PM->TypeReg.getIntegerTy());
    return PM->TypeReg.getIntegerTy();
}

const Type *ExprTypeAnnotatorPass::visitRealLiteral(RealLiteral *Real) {
    PM->setAnnotation<ExprTypeAnnotatorPass>(Real, PM->TypeReg.getRealTy());
    return PM->TypeReg.getRealTy();
}
