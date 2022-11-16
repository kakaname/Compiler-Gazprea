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


    if (isa<NullTy>(LeftType)) {
        assert(!isa<NullTy>(RightType) && "Operation between null types.");
        auto Cast = wrapWithCastTo(LeftExpr, RightType);
        Op->setLeftExpr(Cast);
        LeftType = RightType;
    }

    if (isa<NullTy>(RightType)) {
        assert(!isa<NullTy>(LeftType) && "Operation between null types.");
        auto Cast = wrapWithCastTo(RightExpr, LeftType);
        Op->setRightExpr(Cast);
        RightType = LeftType;
    }

    assert(LeftType->isValidForComparisonOp() && "Left type does not "
                                                 "support comparison ops");
    assert(RightType->isValidForComparisonOp() && "Right type does not "
                                                  "support comparison ops");

    if (LeftType->isSameTypeAs(RightType)) {
        PM->setAnnotation<ExprTypeAnnotatorPass>(Op, PM->TypeReg.getBooleanTy());
        return PM->TypeReg.getBooleanTy();
    }

    if (LeftType->canPromoteTo(RightType)) {
        auto Cast = wrapWithCastTo(LeftExpr, RightType);
        Op->setLeftExpr(Cast);
        PM->setAnnotation<ExprTypeAnnotatorPass>(Op, PM->TypeReg.getBooleanTy());
        return PM->TypeReg.getBooleanTy();
    }

    if (RightType->canPromoteTo(LeftType)) {
        auto Cast = wrapWithCastTo(RightExpr, LeftType);
        Op->setRightExpr(Cast);
        PM->setAnnotation<ExprTypeAnnotatorPass>(Op, PM->TypeReg.getBooleanTy());
        return PM->TypeReg.getBooleanTy();
    }

    assert(false && "Comparison between incompatible types");
}

const Type *ExprTypeAnnotatorPass::visitArithmeticOp(ArithmeticOp *Op) {
    auto LeftExpr = Op->getLeftExpr();
    auto RightExpr = Op->getRightExpr();
    auto LeftType = visit(LeftExpr);
    auto RightType = visit(RightExpr);


    if (isa<NullTy>(LeftType)) {
        assert(!isa<NullTy>(RightType) && "Operation between null types.");
        auto Cast = wrapWithCastTo(LeftExpr, RightType);
        Op->setLeftExpr(Cast);
        LeftType = RightType;
    }

    if (isa<NullTy>(RightType)) {
        assert(!isa<NullTy>(LeftType) && "Operation between null types.");
        auto Cast = wrapWithCastTo(RightExpr, LeftType);
        Op->setRightExpr(Cast);
        RightType = LeftType;
    }

    assert(LeftType->isValidForArithOps() && "Left type does not"
                                             " support arithmetic ops");

    assert(RightType->isValidForArithOps() && "Right type does not "
                                              "support arithmetic ops");


    if (LeftType->isSameTypeAs(RightType)) {
        PM->setAnnotation<ExprTypeAnnotatorPass>(Op, LeftType);
        return LeftType;
    }

    if (LeftType->canPromoteTo(RightType)) {
        auto Cast = wrapWithCastTo(LeftExpr, RightType);
        Op->setLeftExpr(Cast);
        PM->setAnnotation<ExprTypeAnnotatorPass>(Op, RightType);
        return RightType;
    }

    if (RightType->canPromoteTo(LeftType)) {
        auto Cast = wrapWithCastTo(RightExpr, LeftType);
        Op->setRightExpr(Cast);
        PM->setAnnotation<ExprTypeAnnotatorPass>(Op, LeftType);
        return LeftType;
    }

    assert(false && "Comparison between incompatible types");
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

    if (isa<NullTy>(LeftType)) {
        assert(!isa<NullTy>(RightType) && "Operation between null types.");
        auto Cast = wrapWithCastTo(LeftExpr, RightType);
        Op->setLeftExpr(Cast);
        LeftType = RightType;
    }

    if (isa<NullTy>(RightType)) {
        assert(!isa<NullTy>(LeftType) && "Operation between null types.");
        auto Cast = wrapWithCastTo(RightExpr, LeftType);
        Op->setRightExpr(Cast);
        RightType = LeftType;
    }

    if (Op->getOpKind() == LogicalOp::EQ || Op->getOpKind() == LogicalOp::NEQ) {
        assert(LeftType->isValidForEq() && "Left type does not support");
        assert(RightType->isValidForEq() && "Right type does not support");

        if (LeftType->isSameTypeAs(RightType)) {
            PM->setAnnotation<ExprTypeAnnotatorPass>(Op, PM->TypeReg.getBooleanTy());
            return PM->TypeReg.getBooleanTy();
        }

        if (LeftType->canPromoteTo(RightType)) {
            auto Cast = wrapWithCastTo(LeftExpr, RightType);
            Op->setLeftExpr(Cast);
            PM->setAnnotation<ExprTypeAnnotatorPass>(Op, PM->TypeReg.getBooleanTy());
            return PM->TypeReg.getBooleanTy();
        }

        if (RightType->canPromoteTo(LeftType)) {
            auto Cast = wrapWithCastTo(RightExpr, LeftType);
            Op->setRightExpr(Cast);
            PM->setAnnotation<ExprTypeAnnotatorPass>(Op, PM->TypeReg.getBooleanTy());
            return PM->TypeReg.getBooleanTy();
        }

        //Check tuple promotion
        if (isa<TupleTy>(LeftType) && isa<TupleTy>(RightType)) {
            assert((LeftType->isSameTypeAs(RightType) || LeftType->canPromoteTo(RightType)
                    || RightType->canPromoteTo(LeftType)) && "Cannot compare incompatible tuple types");
            if (LeftType->canPromoteTo(RightType)) {
                auto Cast = PM->Builder.build<TypeCast>();
                auto TupleType = cast<TupleTy>(RightType);

                Cast->setExpr(LeftExpr);
                Cast->setTargetType(TupleType);
                Op->setLeftExpr(Cast);
                LeftType = Cast->getTargetType();
            }

            if (RightType->canPromoteTo(LeftType)) {
                auto Cast = PM->Builder.build<TypeCast>();
                auto TupleType = cast<TupleTy>(LeftType);

                Cast->setExpr(RightExpr);
                Cast->setTargetType(TupleType);
                Op->setRightExpr(Cast);
                RightType = Cast->getTargetType();
            }
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

TypeCast *ExprTypeAnnotatorPass::wrapWithCastTo(ASTNodeT *Expr, const Type *TargetType) const {
    auto Cast = PM->Builder.build<TypeCast>();
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
    auto Ident = dyn_cast<Identifier>(MAccess->getExpr());
    assert(Ident && "Only lvalues may have their members accessed");
    auto IdentType = Ident->getIdentType();
    
    assert(IdentType && "Type not assigned to identifier.");
    auto Tuple = dyn_cast<TupleTy>(IdentType);
    assert(Tuple && "Only identifier that are of type tuple maybe have their members accessed");
    auto MemberIdx = dyn_cast<IntLiteral>(MAccess->getMemberExpr());
    assert(MemberIdx && "Only member accesses with integer literals "
                        "should have reached this place.");

    assert((int32_t) Tuple->getNumOfMembers() >= MemberIdx->getVal()
                && "Invalid index to access a member");
    auto ResultTy = Tuple->getMemberTypeAt(MemberIdx->getVal() - 1);
    PM->setAnnotation<ExprTypeAnnotatorPass>(MAccess, ResultTy);
    return ResultTy;
}

const Type *ExprTypeAnnotatorPass::visitTupleLiteral(TupleLiteral *TupLit) {
    vector<const Type*> ChildTypes;
    for (auto *ChildExpr : *TupLit) {
        auto ChildTy = visit(ChildExpr);
        ChildTypes.emplace_back(ChildTy);
    }
    auto TupleTy = PM->TypeReg.getTupleType(ChildTypes, map<string, int>());
    PM->setAnnotation<ExprTypeAnnotatorPass>(TupLit, TupleTy);
    return TupleTy;
}

const Type *ExprTypeAnnotatorPass::visitFunctionCall(FunctionCall *Call) {
    visit(Call->getArgsList());
    auto IdentTy = Call->getIdentifier()->getIdentType();
    assert(IdentTy && "Ident type not set for function call");
    assert(IdentTy->isCallable() && "Tried call an non-callable type.");
    if (auto FuncTy = dyn_cast<FunctionTy>(IdentTy)) {
        auto RetTy = FuncTy->getRetType();
        PM->setAnnotation<ExprTypeAnnotatorPass>(Call, RetTy);
        return RetTy;
    };
    auto ProcTy = cast<ProcedureTy>(IdentTy);
    auto RetTy = ProcTy->getRetTy();
    PM->setAnnotation<ExprTypeAnnotatorPass>(Call, RetTy);
    return RetTy;
}

const Type *ExprTypeAnnotatorPass::visitIntLiteral(IntLiteral *Int) const {
    PM->setAnnotation<ExprTypeAnnotatorPass>(Int, PM->TypeReg.getIntegerTy());
    return PM->TypeReg.getIntegerTy();
}

const Type *ExprTypeAnnotatorPass::visitRealLiteral(RealLiteral *Real) const {
    PM->setAnnotation<ExprTypeAnnotatorPass>(Real, PM->TypeReg.getRealTy());
    return PM->TypeReg.getRealTy();
}

const Type *ExprTypeAnnotatorPass::visitExplicitCast(ExplicitCast *Cast) {
    visit(Cast->getExpr());
    PM->setAnnotation<ExprTypeAnnotatorPass>(Cast, Cast->getTargetType());
    return Cast->getTargetType();
}

const Type *ExprTypeAnnotatorPass::visitNullLiteral(NullLiteral *Null) {
    PM->setAnnotation<ExprTypeAnnotatorPass>(Null, PM->TypeReg.getNullTy());
    return PM->TypeReg.getNullTy();
}

const Type *ExprTypeAnnotatorPass::visitIdentityLiteral(IdentityLiteral *ID) {
    PM->setAnnotation<ExprTypeAnnotatorPass>(ID, PM->TypeReg.getIdentityTy());
    return PM->TypeReg.getIdentityTy();
}

const Type *ExprTypeAnnotatorPass::visitTypeCast(TypeCast *Cast) {
    visit(Cast->getExpr());
    PM->setAnnotation<ExprTypeAnnotatorPass>(Cast, Cast->getTargetType());
    return Cast->getTargetType();
}

const Type *ExprTypeAnnotatorPass::visitBoolLiteral(BoolLiteral *Bool) {
    PM->setAnnotation<ExprTypeAnnotatorPass>(Bool, PM->TypeReg.getBooleanTy());
    return PM->TypeReg.getBooleanTy();
}
