//
// Created by rajan on 11/07/22.
//

#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"

using std::make_pair;

void ExprTypeAnnotatorPass::runOnAST(ASTPassManager &Manager, ASTNodeT *Root) {
    PM = &Manager;
    TypeReg = &Manager.TypeReg;
    visit(Root);
}

const Type *ExprTypeAnnotatorPass::visitComparisonOp(ComparisonOp *Op) {
    auto LExpr = Op->getLeftExpr();
    auto RExpr = Op->getRightExpr();
    auto LType = visit(LExpr);
    auto RType = visit(RExpr);

    auto BoolTy = TypeReg->getBooleanTy();

    // If both of them are opaque types, we cast them to the opaque type target.
    if (LType->isOpaqueTy() && RType->isOpaqueTy()) {
        if (!OpaqueTyCastTarget)
            throw NullIdentityOpError(Op);

        if (!OpaqueTyCastTarget->isValidForComparisonOp())
            throw InvalidComparisonOpError(Op, OpaqueTyCastTarget->getTypeName());

        if (!isTypeSizeKnown(OpaqueTyCastTarget))
            throw runtime_error("Target type size is unknown cannot coerce "
                                "opaque type to target type");

        auto LCast = wrapWithCastTo(LExpr, OpaqueTyCastTarget);
        auto RCast = wrapWithCastTo(RExpr, OpaqueTyCastTarget);
        Op->setLeftExpr(LCast);
        Op->setRightExpr(RCast);

        // If the comparison is between composite types (vector or matrices)
        // then the result is a composite type of the same size but with inner
        // type boolean.
        if (OpaqueTyCastTarget->isCompositeTy()) {
            annotateWithConst(Op, BoolTy);
            return BoolTy;
        }

        // Otherwise, the result is a composite type with the same size as
        // the opaque type but with inner type as boolean.
        auto ResultTy = PM->TypeReg.getCompositeTyWithInner(
                OpaqueTyCastTarget, BoolTy);
        annotateWithConst(Op, ResultTy);
        return ResultTy;
    }

    auto GetTargetTyForOpaqueCasts = [&](const Type *OtherTy){
        if (OtherTy->isCompositeTy())
            return TypeRegistry::getInnerTyFromComposite(OtherTy);
        return OtherTy;
    };
    // Handle the cases where one of the types is opaque.
    if (LType->isOpaqueTy()) {
        auto Cast = wrapWithCastTo(LExpr, GetTargetTyForOpaqueCasts(RType));
        Op->setLeftExpr(Cast);
        LType = RType;
    }

    if (RType->isOpaqueTy()) {
        auto Cast = wrapWithCastTo(RExpr, GetTargetTyForOpaqueCasts(LType));
        Op->setRightExpr(Cast);
        RType = LType;
    }

    // Check both types are valid for comparison.
    if (!LType->isValidForComparisonOp())
        throw InvalidComparisonOpError(Op, LType->getTypeName());

    if (!RType->isValidForComparisonOp())
        throw InvalidComparisonOpError(Op, RType->getTypeName());


    // Easy case where both types are the same.
    if (LType->isSameTypeAs(RType)) {
        auto ResultTy = (LType->isCompositeTy()) ?
                TypeReg->getCompositeTyWithInner(LType, BoolTy) : BoolTy;
        annotateWithConst(Op, ResultTy);
        return ResultTy;
    }

    matchBoolPair(LType->isCompositeTy(), RType->isCompositeTy()) {
        // When both the operands are composite types, we cast based on the
        // inner type and the result is a composite type of the wider inner
        // type with its size being unknown if both operands have unknown
        // size or equal to the size of the known sized type.
        matchPattern(true, true): {
            if (!(isa<VectorTy>(LType) && isa<VectorTy>(RType)) ||
            !(isa<MatrixTy>(LType) && isa<MatrixTy>(RType)))
                throw runtime_error("Operation between incompatible types");

            auto LInner = TypeRegistry::getInnerTyFromComposite(LType);
            auto RInner = TypeRegistry::getInnerTyFromComposite(RType);
            auto WiderTy = getWiderType(LInner, RInner);
            if (!WiderTy)
                throw runtime_error("No suitable promotion possible");

            // The result type is a vector with the size equal to the size of the
            // operands but where the inner type is boolean.
            auto GetResType = [&](const Type *T1, const Type *T2) {
                // If the types are vectors.
                if (auto Vec1 = dyn_cast<VectorTy>(T1)) {
                    auto Vec2 = cast<VectorTy>(T2);
                    matchBoolPair(Vec1->isSizeKnown(), Vec2->isSizeKnown()) {
                        matchPattern(true, true):
                            if (Vec1->getSize() != Vec2->getSize())
                                throw runtime_error("Operation between vectors of unequal size");
                            return TypeReg->getCompositeTyWithInner(Vec1, BoolTy);
                        matchPattern(true, false):
                            return TypeReg->getCompositeTyWithInner(Vec1, BoolTy);
                        matchPattern(false, true):
                            return TypeReg->getCompositeTyWithInner(Vec2, BoolTy);
                        matchPattern(false, false):
                            return TypeReg->getVectorType(BoolTy);
                    }
                }
                auto Mat1 = dyn_cast<MatrixTy>(T1);
                auto Mat2 = cast<MatrixTy>(T2);
                matchBoolPair(Mat1->isSizeKnown(), Mat2->isSizeKnown()) {
                    matchPattern(true, true):
                        if (Mat1->isSameSizeAs(Mat2))
                            throw runtime_error("Operation between vectors of unequal size");
                        return TypeReg->getCompositeTyWithInner(Mat1, BoolTy);
                    matchPattern(true, false):
                        return TypeReg->getCompositeTyWithInner(Mat1, BoolTy);
                    matchPattern(false, true):
                        return TypeReg->getCompositeTyWithInner(Mat2, BoolTy);
                    matchPattern(false, false):
                        return TypeReg->getMatrixType(BoolTy);
                }
                throw runtime_error("Should be unreachable");
            };

            // Do the casts.
            if (!LInner->isSameTypeAs(WiderTy)) {
                auto TargetTy = TypeReg->getCompositeTyWithInner(
                        LType, WiderTy);
                LExpr = wrapWithCastTo(LExpr, TargetTy);
                Op->setLeftExpr(LExpr);
                auto ResType = GetResType(TargetTy, RType);
                annotateWithConst(Op, ResType);
                return ResType;
            }

            if (!RInner->isSameTypeAs(WiderTy)) {
                auto TargetTy = TypeReg->getCompositeTyWithInner(
                        RType, WiderTy);
                RExpr = wrapWithCastTo(RExpr, TargetTy);
                Op->setRightExpr(RExpr);
                auto ResType = GetResType(LType, TargetTy);
                annotateWithConst(Op, ResType);
                return ResType;
            }
        }

        // If the left type is composite, and the right type is not, we cast
        // the right type to the inner type of the left type if the inner type
        // is wider than the right type. If the reverse is true, i.e. the right
        // type is wider, then we cast the left type to a composite type with
        // the same size as before but the inner being the right type. The
        // result is always a composite type with equal size as the composite
        // type but the inner type being the wider type.
        matchPattern(true, false): {
            auto LInner = TypeRegistry::getInnerTyFromComposite(LType);
            auto WiderTy = getWiderType(LInner, RType);

            if (!WiderTy)
                throw runtime_error("No suitable promotion possible");

            if (!LInner->isSameTypeAs(WiderTy)) {
                auto TargetTy = TypeReg->getCompositeTyWithInner(
                        LType, WiderTy);
                LExpr = wrapWithCastTo(LExpr, TargetTy);
                Op->setLeftExpr(LExpr);
                auto ResTy = TypeReg->getCompositeTyWithInner(
                        LType, BoolTy);
                annotateWithConst(Op, ResTy);
                return ResTy;
            }

            if (!RType->isSameTypeAs(WiderTy))  {
                RExpr = wrapWithCastTo(RExpr, WiderTy);
                Op->setRightExpr(RExpr);
                auto ResTy = TypeReg->getCompositeTyWithInner(RType, BoolTy);
                annotateWithConst(Op, ResTy);
                return ResTy;
            }
            throw runtime_error("Should be unreachable");
        }

        // Same as the (true, false) case just mirrored.
        matchPattern(false, true): {
            auto RInner = TypeRegistry::getInnerTyFromComposite(RType);
            auto WiderTy = getWiderType(RInner, LType);

            if (!WiderTy)
                throw runtime_error("No suitable promotion possible");

            if (!RInner->isSameTypeAs(WiderTy)) {
                auto TargetTy = TypeReg->getCompositeTyWithInner(
                        RType, WiderTy);
                RExpr = wrapWithCastTo(RExpr, TargetTy);
                Op->setRightExpr(RExpr);
                auto ResTy = TypeReg->getCompositeTyWithInner(
                        RType, BoolTy);
                annotateWithConst(Op, ResTy);
                return ResTy;
            }

            if (!LType->isSameTypeAs(WiderTy))  {
                LExpr = wrapWithCastTo(LExpr, WiderTy);
                Op->setLeftExpr(LExpr);
                auto ResTy = TypeReg->getCompositeTyWithInner(RType, BoolTy);
                annotateWithConst(Op, ResTy);
                return ResTy;
            }
            throw runtime_error("Should be unreachable");
        }

        // In this case we simply cast the narrower type to the wider type and
        // we are done
        matchPattern(false, false): {
            auto WiderTy = getWiderType(LType, RType);
            if (!LType->isSameTypeAs(WiderTy)) {
                Op->setLeftExpr(wrapWithCastTo(LExpr, WiderTy));
                annotateWithConst(Op, WiderTy);
                return WiderTy;
            }

            Op->setRightExpr(wrapWithCastTo(RExpr, WiderTy));
            annotateWithConst(Op, WiderTy);
            return WiderTy;
        }
    }
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

        if (!OpaqueTyCastTarget->isValidForArithOps())
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
    annotate(Ident, Ident->getIdentType());
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

        if (LeftType->getKind() == Type::TypeKind::T_Vector && RightType->getKind() == Type::TypeKind::T_Vector) {
            auto LeftVecTy = cast<VectorTy>(LeftType);
            auto RightVecTy = cast<VectorTy>(RightType);
            if (!LeftVecTy->getInnerTy()->isSameTypeAs(RightVecTy->getInnerTy())) {
                throw runtime_error("Vector types must be of the same type");
            }

            if (!LeftVecTy->getInnerTy()->isSameTypeAs(PM->TypeReg.getBooleanTy())) {
                throw InvalidComparisonOpError(Op, LeftVecTy->getInnerTy()->getTypeName());
            }

            PM->setAnnotation<ExprTypeAnnotatorPass>(Op, LeftType);
            return LeftType;
        }

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
        if (!ChildType->isValidForUnaryNot()) {
            if (ChildType->getKind() == Type::TypeKind::T_Vector && dyn_cast<VectorTy>(ChildType)->getInnerTy()->isValidForUnaryNot()) {
                auto NewVecTy = PM->TypeReg.getVectorType(PM->TypeReg.getBooleanTy(), dyn_cast<VectorTy>(ChildType)->getSize());
                annotate(Op, NewVecTy);
                return NewVecTy;
            }
            throw InvalidUnaryNotError(Op, ChildType->getTypeName());
        }
        annotate(Op, PM->TypeReg.getBooleanTy());
        return PM->TypeReg.getBooleanTy();
    }

    if (!ChildType->isValidForUnaryAddOrSub() && ChildType->getKind() != Type::TypeKind::T_Vector) {
        throw InvalidUnaryAddOrSubError(Op, ChildType->getTypeName());
    }
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
        annotateWithConst(MAccess, ResultTy);
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

const Type *ExprTypeAnnotatorPass::visitIndexReference(IndexReference *Ref) {
    auto BaseTy = visit(Ref->getBaseExpr());
    if (!BaseTy)
        throw runtime_error("Base type not set for index reference.");
    if (BaseTy->getKind() != Type::TypeKind::T_Vector)
        throw runtime_error("Base type is not a vector.");
    // TODO check if baseexpr is simply an ID?
    auto VecTy = dyn_cast<VectorTy>(BaseTy);

    auto IdxTy = visit(Ref->getIndexExpr());
    if (!IdxTy)
        throw runtime_error("Index type not set for index reference.");
    // TODO run pass to convert vector indexing to simple loop
    if (!IdxTy->isSameTypeAs(PM->TypeReg.getIntegerTy()))
        throw runtime_error("Index type is not an integer.");

    auto ResultTy = VecTy->getInnerTy();
    ResultTy = PM->TypeReg.getVarTypeOf(ResultTy);
    PM->setAnnotation<ExprTypeAnnotatorPass>(Ref, ResultTy);
    return ResultTy;

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

const Type *ExprTypeAnnotatorPass::visitVectorLiteral(VectorLiteral *VecLit) {
    const Type *VecTy = nullptr;
    // Pass 1: Check if all elements are of the same or promotable type (get the highest type)
    for (auto *ChildExpr : *VecLit) {
        const Type *ChildTy = visit(ChildExpr);

        if (!ChildTy->isScalarTy())
            throw std::runtime_error("Vector literal can only contain scalar types");

        if (!VecTy) {
            VecTy = ChildTy;
            continue;
        }

        VecTy = ChildTy->getPromotedType(VecTy);
        if (!VecTy)
            throw std::runtime_error("Vector literal can only contain scalar types of the same or promotable types");
    }

    // Pass 2: Promote all elements to the highest type
    for (int i = 0; i < VecLit->numOfChildren(); i++) {
        auto ChildExpr = VecLit->getChildAt(i);
        auto ChildTy = visit(ChildExpr);
        if (!ChildTy->isSameTypeAs(VecTy) && ChildTy->canPromoteTo(VecTy)) {
            auto NewChildExpr = wrapWithCastTo(ChildExpr, VecTy);
            VecLit->setExprAtPos(NewChildExpr, i);
        }
    }

    // Get the vector type
    VecTy = PM->TypeReg.getVectorType(VecTy, (int) VecLit->numOfChildren());
    PM->setAnnotation<ExprTypeAnnotatorPass>(VecLit, VecTy);
    return VecTy;

}

const Type *ExprTypeAnnotatorPass::visitIndex(Index *Idx) {
    auto BaseTy = visit(Idx->getBaseExpr());
    auto VecTy = dyn_cast<VectorTy>(BaseTy);

    if (!VecTy)
        throw std::runtime_error("Indexing can only be done on vector types");

    auto IdxTy = visit(Idx->getIndexExpr());
    auto IntTy = dyn_cast<IntegerTy>(IdxTy);
    if (!IntTy)
        throw std::runtime_error("Indexing can only be done with integer types");

    auto ResultTy = VecTy->getInnerTy();
    PM->setAnnotation<ExprTypeAnnotatorPass>(Idx, ResultTy);
    return ResultTy;
}

const Type *ExprTypeAnnotatorPass::visitInterval(Interval *Int) {
    auto Lower = visit(Int->getLowerExpr());
    auto Upper = visit(Int->getUpperExpr());

    auto IntTy = TypeReg->getIntegerTy();

    if (Upper->isOpaqueTy()) {
        Int->setUpperExpr(wrapWithCastTo(Int->getLowerExpr(), IntTy));
        Upper = IntTy;
    }

    if (Lower->isOpaqueTy()) {
        Int->setLowerExpr(wrapWithCastTo(Int->getLowerExpr(), IntTy));
        Lower = IntTy;
    }

    if (!isa<IntegerTy>(Lower) && !isa<IntegerTy>(Upper))
        throw runtime_error("Interval bounds must be integers");

    // TODO: Check constant folding.

    annotateWithConst(Int, TypeReg->getIntervalTy());
    return TypeReg->getIntervalTy();
}


const Type *ExprTypeAnnotatorPass::visitByOp(ByOp *By) {
    auto BaseExprTy = visit(By->getBaseExpr());
    auto ByExprTy = visit(By->getByExpr());

    if (!isa<IntegerTy>(ByExprTy))
        throw runtime_error("The expression used in a by operation must be"
                            " integer");

    if (!BaseExprTy->isValidForBy())
        throw runtime_error("Only vectors or intervals may be used in a ByOp");

    // TODO: Constant fold here.
    auto ResLen = [&]() {
      return -1;
    }();

    auto InnerTy = isa<VectorTy>(BaseExprTy) ?
            TypeRegistry::getInnerTyFromComposite(BaseExprTy) :
            TypeReg->getIntegerTy();

    auto ResTy = TypeReg->getVectorType(InnerTy, ResLen);
    annotate(By, ResTy);
    return ResTy;
}

const Type *ExprTypeAnnotatorPass::visitDotProduct(DotProduct *Dot) {
    auto LVecTy = dyn_cast<VectorTy>(visit(Dot->getLHS()));
    auto RVecTy = dyn_cast<VectorTy>(visit(Dot->getRHS()));

    if (!LVecTy || !RVecTy)
        throw std::runtime_error("Dot product can only be applied to vector types");

    // If the inner types are not the same then the dot product is invalid.
    if (!LVecTy->getInnerTy()->isSameTypeAs(RVecTy->getInnerTy()))
        throw std::runtime_error("Dot product can only be applied to vector types"
                                 " with the same inner type");

    if (!LVecTy->isSameTypeAs(RVecTy))
        throw std::runtime_error("Dot product can only be applied to vectors of the same type and size");

    auto ResTy = LVecTy->getInnerTy();
    annotateWithConst(Dot, ResTy);
    return ResTy;
}

const Type *ExprTypeAnnotatorPass::visitConcat(Concat *Concat) {
    auto LExpr = Concat->getLHS();
    auto RExpr = Concat->getRHS();
    auto LType = visit(LExpr);
    auto RType = visit(RExpr);

    if (!isa<VectorTy>(LType) && !isa<VectorTy>(RType))
        throw runtime_error("At least one of the operands of a concat must be"
                            " a vector");


    // If LHS is not of type of vector, we make it
    if (!isa<VectorTy>(LType)) {
        LType = TypeReg->getVectorType(RType, 1);
        LExpr = wrapWithCastTo(LExpr, LType);
    }

    // Similarly for RHS
    if (!isa<VectorTy>(RType)) {
        RType = TypeReg->getVectorType(RType, 1);
        RExpr = wrapWithCastTo(RExpr, RType);
    }

    auto LVecTy = cast<VectorTy>(LType);
    auto RVecTy = cast<VectorTy>(RType);

    auto LInner = LVecTy->getInnerTy();
    auto RInner = RVecTy->getInnerTy();

    auto ResLen = [&]() {
        // If either of the sizes is unknown, the result size is unknown.
        if (!LVecTy->isSizeKnown() || !RVecTy->isSizeKnown())
            return -1;
        // Otherwise both sizes are known and the result has size as the sum
        // of the two.
        return LVecTy->getSize() + RVecTy->getSize();
    }();

    // If the inner types are the same.
    if (LInner->isSameTypeAs(RInner)) {
        Concat->setLHS(LExpr);
        Concat->setRHS(RExpr);
        auto ResTy = TypeReg->getVectorType(LInner, ResLen);
        annotateWithConst(Concat, ResTy);
        return ResTy;
    }

    auto WiderTy = getWiderType(LInner, RInner);

    if (LInner != WiderTy)
        LExpr = wrapWithCastTo(LExpr, TypeReg->getCompositeTyWithInner(LVecTy, WiderTy));

    if (RInner != WiderTy)
        RExpr = wrapWithCastTo(RExpr, TypeReg->getCompositeTyWithInner(RVecTy, WiderTy));

    Concat->setLHS(LExpr);
    Concat->setRHS(RExpr);
    auto ResTy = TypeReg->getVectorType(WiderTy, ResLen);
    annotateWithConst(Concat, ResTy);
    return ResTy;
}


const Type *ExprTypeAnnotatorPass::getWiderType(const Type *Ty1, const Type *Ty2) {
    if (Ty1->isCompositeTy() != Ty2->isCompositeTy())
        throw std::runtime_error("Called wider type with incompatible args");

    if (Ty1->canPromoteTo(Ty2))
        return Ty2;

    if (Ty2->canPromoteTo(Ty1))
        return Ty1;

    return nullptr;
}

bool ExprTypeAnnotatorPass::isTypeSizeKnown(const Type *Ty) {
    if (auto VecTy = dyn_cast<VectorTy>(Ty))
        return VecTy->isSizeKnown();

    if (auto MatTy = dyn_cast<MatrixTy>(Ty))
        return MatTy->isSizeKnown();

    if (auto TupTy = dyn_cast<TupleTy>(Ty)) {
        auto Members = TupTy->getMemberTypes();
        return std::all_of(Members.begin(), Members.end(), isTypeSizeKnown);
    }

    return true;
}
