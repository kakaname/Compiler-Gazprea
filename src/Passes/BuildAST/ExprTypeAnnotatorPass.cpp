//
// Created by rajan on 11/07/22.
//

#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"


void ExprTypeAnnotatorPass::runOnAST(ASTPassManager &Manager, ASTNodeT *Root) {
    PM = &Manager;
    visit(Root);
}

const Type *ExprTypeAnnotatorPass::visitComparisonOp(ComparisonOp *Op) {
    auto LeftExpr = Op->getLeftExpr();
    auto RightExpr = Op->getRightExpr();
    auto LeftType = visit(LeftExpr);
    auto RightType = visit(RightExpr);

    // If both of them are opaque types, we cast them to the opaque type target.
    if (LeftType->isOpaqueTy() && RightType->isOpaqueTy()) {
        if (!OpaqueTyCastTarget)
            throw NullIdentityOpError(Op);

        if (!OpaqueTyCastTarget->isValidForComparisonOp())
            throw InvalidComparisonOpError(Op, OpaqueTyCastTarget->getTypeName());

        auto LeftCast = wrapWithCastTo(LeftExpr, OpaqueTyCastTarget);
        auto RightCast = wrapWithCastTo(RightExpr, OpaqueTyCastTarget);
        Op->setLeftExpr(LeftCast);
        Op->setRightExpr(RightCast);
        annotateWithConst(Op, OpaqueTyCastTarget);
        return OpaqueTyCastTarget;
    }

    // At least one of them is opaque.
    if (LeftType->isOpaqueTy() || RightType->isOpaqueTy()) {
        //
        if (LeftType->isOpaqueTy()) {
            auto Cast = wrapWithCastTo(LeftExpr, RightType);
            Op->setLeftExpr(Cast);
            LeftType = RightType;
        } else {
            auto Cast = wrapWithCastTo(RightExpr, LeftType);
            Op->setRightExpr(Cast);
            RightType = LeftType;
        }
    }

    if (!LeftType->isValidForComparisonOp())
        throw InvalidComparisonOpError(Op, LeftType->getTypeName());

    if (!RightType->isValidForComparisonOp())
        throw InvalidComparisonOpError(Op, RightType->getTypeName());

    if (LeftType->isSameTypeAs(RightType)) {
        annotate(Op, PM->TypeReg.getBooleanTy());
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

    // If both of them are opaque types, we cast them to the opaque type target.
    if (LeftType->isOpaqueTy() && RightType->isOpaqueTy()) {
        if (!OpaqueTyCastTarget)
            throw NullIdentityOpError(Op);

        if (!OpaqueTyCastTarget->isValidForComparisonOp())
            throw InvalidComparisonOpError(Op, OpaqueTyCastTarget->getTypeName());

        auto LeftCast = wrapWithCastTo(LeftExpr, OpaqueTyCastTarget);
        auto RightCast = wrapWithCastTo(RightExpr, OpaqueTyCastTarget);
        Op->setLeftExpr(LeftCast);
        Op->setRightExpr(RightCast);
        annotateWithConst(Op, OpaqueTyCastTarget);
        return OpaqueTyCastTarget;
    }

    // At least one of them is opaque.
    if (LeftType->isOpaqueTy() || RightType->isOpaqueTy()) {
        //
        if (LeftType->isOpaqueTy()) {
            auto Cast = wrapWithCastTo(LeftExpr, RightType);
            Op->setLeftExpr(Cast);
            LeftType = RightType;
        } else {
            auto Cast = wrapWithCastTo(RightExpr, LeftType);
            Op->setRightExpr(Cast);
            RightType = LeftType;
        }
    }

    if (!LeftType->isValidForArithOps())
        throw InvalidArithmeticOpError(Op, LeftType->getTypeName());

    if (!RightType->isValidForArithOps())
        throw InvalidArithmeticOpError(Op, RightType->getTypeName());

    if (LeftType->isSameTypeAs(RightType)) {
        annotateWithConst(Op, LeftType);
        return PM->TypeReg.getConstTypeOf(LeftType);
    }

    if (LeftType->canPromoteTo(RightType)) {
        auto Cast = wrapWithCastTo(LeftExpr, RightType);
        Op->setLeftExpr(Cast);
        annotateWithConst(Op, RightType);
        return PM->TypeReg.getConstTypeOf(RightType);
    }

    if (RightType->canPromoteTo(LeftType)) {
        auto Cast = wrapWithCastTo(RightExpr, LeftType);
        Op->setRightExpr(Cast);
        annotateWithConst(Op, LeftType);
        return PM->TypeReg.getConstTypeOf(LeftType);
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

    // If both of them are opaque types, we cast them to the opaque type target.
    if (LeftType->isOpaqueTy() && RightType->isOpaqueTy()) {
        if (!OpaqueTyCastTarget)
            throw NullIdentityOpError(Op);

        if (!OpaqueTyCastTarget->isValidForComparisonOp())
            throw InvalidComparisonOpError(Op, OpaqueTyCastTarget->getTypeName());

        auto LeftCast = wrapWithCastTo(LeftExpr, OpaqueTyCastTarget);
        auto RightCast = wrapWithCastTo(RightExpr, OpaqueTyCastTarget);
        Op->setLeftExpr(LeftCast);
        Op->setRightExpr(RightCast);
        annotateWithConst(Op, OpaqueTyCastTarget);
        return OpaqueTyCastTarget;
    }

    // At least one of them is opaque.
    if (LeftType->isOpaqueTy() || RightType->isOpaqueTy()) {
        //
        if (LeftType->isOpaqueTy()) {
            auto Cast = wrapWithCastTo(LeftExpr, RightType);
            Op->setLeftExpr(Cast);
            LeftType = RightType;
        } else {
            auto Cast = wrapWithCastTo(RightExpr, LeftType);
            Op->setRightExpr(Cast);
            RightType = LeftType;
        }
    }

    if (Op->getOpKind() == LogicalOp::EQ || Op->getOpKind() == LogicalOp::NEQ) {
        if (!LeftType->isValidForEq())
            throw InvalidEqualityOpError(Op, LeftType->getTypeName());

        if (!RightType->isValidForEq())
            throw InvalidEqualityOpError(Op, RightType->getTypeName());

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
//        if (isa<TupleTy>(LeftType) && isa<TupleTy>(RightType)) {
//            if(!(LeftType->isSameTypeAs(RightType) || LeftType->canPromoteTo(RightType)
//                    || RightType->canPromoteTo(LeftType)))
//                throw InvalidTupleComparisonError(Op, LeftType->getTypeName(), RightType->getTypeName());
//
//            if (LeftType->canPromoteTo(RightType)) {
//                auto Cast = PM->Builder.build<TypeCast>();
//                Cast->copyCtx(Op);
//                auto TupleType = cast<TupleTy>(RightType);
//
//                Cast->setExpr(LeftExpr);
//                Cast->setTargetType(TupleType);
//                Op->setLeftExpr(Cast);
//                LeftType = Cast->getTargetType();
//            }
//
//            if (RightType->canPromoteTo(LeftType)) {
//                auto Cast = PM->Builder.build<TypeCast>();
//                Cast->copyCtx(Op);
//                auto TupleType = cast<TupleTy>(LeftType);
//
//                Cast->setExpr(RightExpr);
//                Cast->setTargetType(TupleType);
//                Op->setRightExpr(Cast);
//                RightType = Cast->getTargetType();
//            }
//        }

        if (!LeftType->isSameTypeAs(RightType))
            throw InvalidComparisonError(Op, LeftType->getTypeName(), RightType->getTypeName());

        PM->setAnnotation<ExprTypeAnnotatorPass>(Op, PM->TypeReg.getBooleanTy());
        return PM->TypeReg.getBooleanTy();

    } else {
        // All other "logical ops" are only supported for booleans.
        if (!isa<BoolTy>(LeftType))
            throw InvalidLogicalOpError(Op, LeftType->getTypeName());

        if (!isa<BoolTy>(RightType))
            throw InvalidLogicalOpError(Op, RightType->getTypeName());

        PM->setAnnotation<ExprTypeAnnotatorPass>(Op, PM->TypeReg.getBooleanTy());
        return PM->TypeReg.getBooleanTy();
    }
}

TypeCast *ExprTypeAnnotatorPass::wrapWithCastTo(ASTNodeT *Expr, const Type *TargetType) const {
    auto Cast = PM->Builder.build<TypeCast>();
    Cast->copyCtx(Expr);
    Cast->setExpr(Expr);
    Cast->setTargetType(TargetType);
    PM->setAnnotation<ExprTypeAnnotatorPass>(Cast, TargetType);
    return Cast;
}

const Type *ExprTypeAnnotatorPass::visitUnaryOp(UnaryOp *Op) {
    auto ChildType = visit(Op->getExpr());
    if (Op->getOpKind() == UnaryOp::NOT) {
        if (!ChildType->isValidForUnaryNot())
            throw InvalidUnaryNotError(Op, ChildType->getTypeName());
        annotate(Op, PM->TypeReg.getBooleanTy());
        return PM->TypeReg.getBooleanTy();
    }

    if (!ChildType->isValidForUnaryAddOrSub())
        throw InvalidUnaryAddOrSubError(Op, ChildType->getTypeName());
    annotateWithConst(Op, ChildType);
    return PM->TypeReg.getConstTypeOf(ChildType);
}

const Type *ExprTypeAnnotatorPass::visitMemberAccess(MemberAccess *MAccess) {
    auto BaseTy = visit(MAccess->getExpr());
    assert(BaseTy && "Type not assigned to identifier.");
    auto Tuple = dyn_cast<TupleTy>(BaseTy);
    if (!Tuple)
        throw NonAccessibleError(MAccess, BaseTy->getTypeName());
    if (auto MemberIdx = dyn_cast<IntLiteral>(MAccess->getMemberExpr())) {
        if (Tuple->getNumOfMembers() < MemberIdx->getVal())
            throw OutOfRangeError(MAccess, MemberIdx->getVal(), Tuple->getNumOfMembers(), BaseTy->getTypeName());
        auto ResultTy = Tuple->getMemberTypeAt(MemberIdx->getVal() - 1);
        PM->setAnnotation<ExprTypeAnnotatorPass>(MAccess, ResultTy);
        return ResultTy;
    }

    if (auto MemberIdent = dyn_cast<Identifier>(MAccess->getMemberExpr())) {
        auto MemIdx = Tuple->getMemberIdx(MemberIdent->getName());
        if (!MemIdx)
            throw TupleAccessError(MAccess, MemberIdent->getName(), BaseTy->getTypeName());
        auto ResultTy = Tuple->getMemberTypeAt(MemIdx - 1);
        PM->setAnnotation<ExprTypeAnnotatorPass>(MAccess, ResultTy);
        return ResultTy;
    }

    assert(false && "Invalid access into tuple type.");
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
    auto IdentTy = visit(Call->getIdentifier());
    assert(IdentTy && "Ident type not set for function call");

    if (!IdentTy->isCallable())
        throw NonCallableError(Call, IdentTy->getTypeName());

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
    PM->setAnnotation<ExprTypeAnnotatorPass>(
            Cast, PM->TypeReg.getConstTypeOf(Cast->getTargetType()));
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
    PM->setAnnotation<ExprTypeAnnotatorPass>(
            Cast, PM->TypeReg.getConstTypeOf(Cast->getTargetType()));
    return Cast->getTargetType();
}

const Type *ExprTypeAnnotatorPass::visitBoolLiteral(BoolLiteral *Bool) {
    PM->setAnnotation<ExprTypeAnnotatorPass>(Bool, PM->TypeReg.getBooleanTy());
    return PM->TypeReg.getBooleanTy();
}

const Type *ExprTypeAnnotatorPass::visitCharLiteral(CharLiteral *Char) {
    PM->setAnnotation<ExprTypeAnnotatorPass>(Char, PM->TypeReg.getCharTy());
    return PM->TypeReg.getCharTy();
}

const Type *ExprTypeAnnotatorPass::visitMemberReference(MemberReference *Ref) {
    auto BaseTy = visit(Ref->getIdentifier());
    assert(BaseTy && "Type not assigned to identifier.");
    auto Tuple = dyn_cast<TupleTy>(BaseTy);
    assert(Tuple && "Only expressions that are of type tuple maybe have their members accessed");

    if (auto MemberIdx = dyn_cast<IntLiteral>(Ref->getMemberExpr())) {
        assert((int32_t) Tuple->getNumOfMembers() >= MemberIdx->getVal()
               && "Invalid index to access a member");
        auto ResultTy = Tuple->getMemberTypeAt(MemberIdx->getVal() - 1);
        PM->setAnnotation<ExprTypeAnnotatorPass>(Ref, ResultTy);
        return ResultTy;
    }

    if (auto MemberIdent = dyn_cast<Identifier>(Ref->getMemberExpr())) {
        auto MemIdx = Tuple->getMemberIdx(MemberIdent->getName());
        assert(MemIdx && "Member of that name not found in the type.");
        auto ResultTy = Tuple->getMemberTypeAt(MemIdx - 1);
        PM->setAnnotation<ExprTypeAnnotatorPass>(Ref, ResultTy);
        return ResultTy;
    }

    assert(false && "Invalid access into tuple type.");
}

const Type *ExprTypeAnnotatorPass::visitIdentReference(IdentReference *Ref) {
    visit(Ref->getIdentifier());
    auto IdentTy = Ref->getIdentifier()->getIdentType();
    assert(IdentTy && "Ident type not known in ident reference");
    PM->setAnnotation<ExprTypeAnnotatorPass>(Ref, IdentTy);
    return IdentTy;
}

const Type *ExprTypeAnnotatorPass::visitProcedureCall(ProcedureCall *Call) {
    visit(Call->getArgsList());
    auto IdentTy = visit(Call->getIdentifier());
    assert(IdentTy && "Ident type not set for function call");

    if (!IdentTy->isCallable())
        throw NonCallableError(Call, IdentTy->getTypeName());

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

const Type *ExprTypeAnnotatorPass::visitInterval(Interval *Int) {
    auto Lower = visit(Int->getLowerExpr());
    auto Upper = visit(Int->getUpperExpr());

    auto IntTy = PM->TypeReg.getIntegerTy();

    // Check that lower and upper both evaluate to the integer type, otherwise
    // cast it to the integer type if valid
    if (!Lower->isSameTypeAs(IntTy)) {
        if (!Lower->canPromoteTo(IntTy))
            throw std::runtime_error("Lower bound of interval must be of type integer");

        auto Cast = wrapWithCastTo(Int->getLowerExpr(), IntTy);
        Int->setLowerExpr(Cast);
    }

    if (!Upper->isSameTypeAs(IntTy)) {
        if (!Upper->canPromoteTo(IntTy))
            throw std::runtime_error("Upper bound of interval must be of type integer");

        auto Cast = wrapWithCastTo(Int->getUpperExpr(), IntTy);
        Int->setUpperExpr(Cast);
    }

    PM->setAnnotation<ExprTypeAnnotatorPass>(Int, PM->TypeReg.getIntervalTy());
    return PM->TypeReg.getIntervalTy();
}
