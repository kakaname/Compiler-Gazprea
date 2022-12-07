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

Type *ExprTypeAnnotatorPass::visitComparisonOp(ComparisonOp *Op) {
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

    auto GetTargetTyForOpaqueCasts = [&](Type *OtherTy){
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
            if (!((isa<VectorTy>(LType) && isa<VectorTy>(RType)) ||
            (isa<MatrixTy>(LType) && isa<MatrixTy>(RType))))
                throw runtime_error("Operation between incompatible "
                                    "types " + LType->getTypeName() +
                                    RType->getTypeName());

            auto LInner = TypeRegistry::getInnerTyFromComposite(LType);
            auto RInner = TypeRegistry::getInnerTyFromComposite(RType);
            auto WiderTy = getWiderType(LInner, RInner);
            if (!WiderTy)
                throw runtime_error("No suitable promotion possible");

            // The result type is a vector with the size equal to the size of the
            // operands but where the inner type is boolean.
            auto GetResType = [&](Type *T1, Type *T2) {
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
                            return TypeReg->getCompositeTyWithInner(Vec1, BoolTy);
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
            }
            auto ResTy = TypeReg->getCompositeTyWithInner(LType, BoolTy);
            annotateWithConst(Op, ResTy);
            return ResTy;
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
            }
            auto ResTy = TypeReg->getCompositeTyWithInner(RType, BoolTy);
            annotateWithConst(Op, ResTy);
            return ResTy;
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
    throw runtime_error("Should be unreachable: TypeAnnotatorComparisonOp");
}

Type *ExprTypeAnnotatorPass::visitArithmeticOp(ArithmeticOp *Op) {
    auto LExpr = Op->getLeftExpr();
    auto RExpr = Op->getRightExpr();
    auto LType = visit(LExpr);
    auto RType = visit(RExpr);

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
            annotateWithConst(Op, OpaqueTyCastTarget);
            return OpaqueTyCastTarget;
        }

        // Otherwise, the result is of the same type as the OpaqueTyCastTarget
        annotateWithConst(Op, OpaqueTyCastTarget);
        return OpaqueTyCastTarget;
    }

    auto GetTargetTyForOpaqueCasts = [&](Type *OtherTy){
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
    if (!LType->isValidForArithOps())
        throw InvalidArithmeticOpError(Op, LType->getTypeName());

    if (!RType->isValidForArithOps())
        throw InvalidArithmeticOpError(Op, RType->getTypeName());


    // Easy case where both types are the same.
    if (LType->isSameTypeAs(RType)) {
        annotateWithConst(Op, LType);
        return LType;
    }

    matchBoolPair(LType->isCompositeTy(), RType->isCompositeTy()) {
        // When both the operands are composite types, we cast based on the
        // inner type and the result is a composite type of the wider inner
        // type with its size being unknown if both operands have unknown
        // size or equal to the size of the known sized type.
        matchPattern(true, true): {
            if (!((isa<VectorTy>(LType) && isa<VectorTy>(RType)) ||
                  (isa<MatrixTy>(LType) && isa<MatrixTy>(RType))))
                throw runtime_error("Operation between incompatible "
                                    "types " + LType->getTypeName() +
                                    RType->getTypeName());

            auto LInner = TypeRegistry::getInnerTyFromComposite(LType);
            auto RInner = TypeRegistry::getInnerTyFromComposite(RType);
            auto WiderTy = getWiderType(LInner, RInner);
            if (!WiderTy)
                throw runtime_error("No suitable promotion possible");

            // The result type is a vector with the size equal to the size of the
            // operands but where the inner type is boolean.
            auto GetResType = [&](Type *T1, Type *T2) {
                // If the types are vectors.
                if (auto Vec1 = dyn_cast<VectorTy>(T1)) {
                    auto Vec2 = cast<VectorTy>(T2);
                    matchBoolPair(Vec1->isSizeKnown(), Vec2->isSizeKnown()) {
                        matchPattern(true, true):
                            if (Vec1->getSize() != Vec2->getSize())
                                throw runtime_error("Operation between vectors of unequal size");
                            return TypeReg->getCompositeTyWithInner(Vec1, WiderTy);
                        matchPattern(true, false):
                            return TypeReg->getCompositeTyWithInner(Vec1, WiderTy);
                        matchPattern(false, true):
                            return TypeReg->getCompositeTyWithInner(Vec2, WiderTy);
                        matchPattern(false, false):
                            return TypeReg->getVectorType(WiderTy);
                    }
                }
                auto Mat1 = dyn_cast<MatrixTy>(T1);
                auto Mat2 = cast<MatrixTy>(T2);
                matchBoolPair(Mat1->isSizeKnown(), Mat2->isSizeKnown()) {
                    matchPattern(true, true):
                        if (Mat1->isSameSizeAs(Mat2))
                            throw runtime_error("Operation between matrices of unequal size");
                        return TypeReg->getCompositeTyWithInner(Mat1, WiderTy);
                    matchPattern(true, false):
                        return TypeReg->getCompositeTyWithInner(Mat1, WiderTy);
                    matchPattern(false, true):
                        return TypeReg->getCompositeTyWithInner(Mat2, WiderTy);
                    matchPattern(false, false):
                        return TypeReg->getMatrixType(WiderTy);
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
                        LType, WiderTy);
                annotateWithConst(Op, ResTy);
                return ResTy;
            }

            if (!RType->isSameTypeAs(WiderTy))  {
                RExpr = wrapWithCastTo(RExpr, WiderTy);
                Op->setRightExpr(RExpr);
            }
            auto ResTy = TypeReg->getCompositeTyWithInner(LType, WiderTy);
            annotateWithConst(Op, ResTy);
            return ResTy;
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
                        RType, WiderTy);
                annotateWithConst(Op, ResTy);
                return ResTy;
            }

            if (!LType->isSameTypeAs(WiderTy))  {
                LExpr = wrapWithCastTo(LExpr, WiderTy);
                Op->setLeftExpr(LExpr);
            }
            auto ResTy = TypeReg->getCompositeTyWithInner(RType, WiderTy);
            annotateWithConst(Op, ResTy);
            return ResTy;
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

Type *ExprTypeAnnotatorPass::visitIdentifier(Identifier *Ident) const {
    assert(Ident->getIdentType() && "Identifier type unknown");
    annotate(Ident, Ident->getIdentType());
    return Ident->getIdentType();
}

Type *ExprTypeAnnotatorPass::visitLogicalOp(LogicalOp *Op) {
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

TypeCast *ExprTypeAnnotatorPass::wrapWithCastTo(ASTNodeT *Expr, Type *TargetType) const {
    auto Cast = PM->Builder.build<TypeCast>();
    Cast->copyCtx(Expr);
    Cast->setExpr(Expr);
    Cast->setTargetType(TargetType);
    PM->setAnnotation<ExprTypeAnnotatorPass>(Cast, TargetType);
    return Cast;
}

Type *ExprTypeAnnotatorPass::visitMemberAccess(MemberAccess *MAccess) {
    auto BaseTy = visit(MAccess->getExpr());
    assert(BaseTy && "Type not assigned to identifier.");
    auto Tuple = dyn_cast<TupleTy>(BaseTy);
    if (!Tuple)
        throw NonAccessibleError(MAccess, BaseTy->getTypeName());
    if (auto MemberIdx = dyn_cast<IntLiteral>(MAccess->getMemberExpr())) {
        if (Tuple->getNumOfMembers() < MemberIdx->getVal())
            throw OutOfRangeError(MAccess, (int) MemberIdx->getVal(),
                                  (int) Tuple->getNumOfMembers(),
                                  BaseTy->getTypeName());
        auto ResultTy = Tuple->getMemberTypeAt(MemberIdx->getVal() - 1);
        annotateWithConst(MAccess, ResultTy);
        return ResultTy;
    }

    if (auto MemberIdent = dyn_cast<Identifier>(MAccess->getMemberExpr())) {
        auto MemIdx = Tuple->getMemberIdx(MemberIdent->getName());
        if (!MemIdx)
            throw TupleAccessError(MAccess, MemberIdent->getName(),
                                   BaseTy->getTypeName());
        auto ResultTy = Tuple->getMemberTypeAt(MemIdx - 1);
        PM->setAnnotation<ExprTypeAnnotatorPass>(MAccess, ResultTy);
        return ResultTy;
    }

    assert(false && "Invalid access into tuple type.");
}

Type *ExprTypeAnnotatorPass::visitUnaryOp(UnaryOp *Op) {
    auto ChildType = visit(Op->getExpr());
    if (Op->getOpKind() == UnaryOp::NOT) {
        if (!ChildType->isValidForUnaryNot())
            throw runtime_error("Unary not not supported for this type");
    } else {
        if (!ChildType->isValidForUnaryAddOrSub())
            throw runtime_error("Unary Add or Sub not supported for this type");
    }
    if (ChildType->isCompositeTy()) {
        auto ResType = TypeReg->getCompositeTyWithInner(
                ChildType, TypeRegistry::getInnerTyFromComposite(ChildType));
        annotateWithConst(Op, ResType);
        return ResType;
    }
    annotateWithConst(Op, ChildType);
    return ChildType;
}

Type *ExprTypeAnnotatorPass::visitTupleLiteral(TupleLiteral *TupLit) {
    vector<Type*> ChildTypes;
    for (auto *ChildExpr : *TupLit) {
        auto ChildTy = visit(ChildExpr);
        ChildTypes.emplace_back(ChildTy);
    }
    map<string, int> Temp{};
    auto TupleTy = PM->TypeReg.getTupleType(ChildTypes, Temp);
    PM->setAnnotation<ExprTypeAnnotatorPass>(TupLit, TupleTy);
    return TupleTy;
}

Type *ExprTypeAnnotatorPass::visitFunctionCall(FunctionCall *Call) {
    visit(Call->getArgsList());
    auto IdentTy = visit(Call->getIdentifier());
    assert(IdentTy && "Ident type not set for function call");

    if (!IdentTy->isCallable())
        throw NonCallableError(Call, IdentTy->getTypeName());

    if (auto FuncTy = dyn_cast<FunctionTy>(IdentTy)) {
        auto RetTy = FuncTy->getRetType();
        PM->setAnnotation<ExprTypeAnnotatorPass>(Call, RetTy);
        return RetTy;
    }
    auto ProcTy = cast<ProcedureTy>(IdentTy);
    auto RetTy = ProcTy->getRetTy();
    PM->setAnnotation<ExprTypeAnnotatorPass>(Call, RetTy);
    return RetTy;
}

Type *ExprTypeAnnotatorPass::visitIntLiteral(IntLiteral *Int) const {
    PM->setAnnotation<ExprTypeAnnotatorPass>(Int, PM->TypeReg.getIntegerTy());
    return PM->TypeReg.getIntegerTy();
}

Type *ExprTypeAnnotatorPass::visitRealLiteral(RealLiteral *Real) const {
    PM->setAnnotation<ExprTypeAnnotatorPass>(Real, PM->TypeReg.getRealTy());
    return PM->TypeReg.getRealTy();
}

Type *ExprTypeAnnotatorPass::visitExplicitCast(ExplicitCast *Cast) {
    auto ExprType = visit(Cast->getExpr());
    if (!isInferredSizedType(Cast->getTargetType())) {
        annotate(Cast, PM->TypeReg.getConstTypeOf(Cast->getTargetType()));
        return Cast->getTargetType();
    }

    copyOverCompositeSizeTypes(ExprType, Cast->getTargetType());
    annotate(Cast, Cast->getTargetType());
    return Cast->getTargetType();
}

Type *ExprTypeAnnotatorPass::visitNullLiteral(NullLiteral *Null) {
    PM->setAnnotation<ExprTypeAnnotatorPass>(Null, PM->TypeReg.getNullTy());
    return PM->TypeReg.getNullTy();
}

Type *ExprTypeAnnotatorPass::visitIdentityLiteral(IdentityLiteral *ID) {
    PM->setAnnotation<ExprTypeAnnotatorPass>(ID, PM->TypeReg.getIdentityTy());
    return PM->TypeReg.getIdentityTy();
}

Type *ExprTypeAnnotatorPass::visitTypeCast(TypeCast *Cast) {
    auto ExprType = visit(Cast->getExpr());
    visitTypeSizeExpressions(Cast->getTargetType());
    if (!isInferredSizedType(Cast->getTargetType())) {
        annotate(Cast, PM->TypeReg.getConstTypeOf(Cast->getTargetType()));
        return Cast->getTargetType();
    }

    copyOverCompositeSizeTypes(ExprType, Cast->getTargetType());
    annotate(Cast, Cast->getTargetType());
    return Cast->getTargetType();
}

Type *ExprTypeAnnotatorPass::visitBoolLiteral(BoolLiteral *Bool) {
    PM->setAnnotation<ExprTypeAnnotatorPass>(Bool, PM->TypeReg.getBooleanTy());
    return PM->TypeReg.getBooleanTy();
}

Type *ExprTypeAnnotatorPass::visitCharLiteral(CharLiteral *Char) {
    PM->setAnnotation<ExprTypeAnnotatorPass>(Char, PM->TypeReg.getCharTy());
    return PM->TypeReg.getCharTy();
}

Type *ExprTypeAnnotatorPass::visitIndexReference(IndexReference *Ref) {
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

Type *ExprTypeAnnotatorPass::visitMemberReference(MemberReference *Ref) {
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

Type *ExprTypeAnnotatorPass::visitIdentReference(IdentReference *Ref) {
    visit(Ref->getIdentifier());
    auto IdentTy = Ref->getIdentifier()->getIdentType();
    assert(IdentTy && "Ident type not known in ident reference");
    PM->setAnnotation<ExprTypeAnnotatorPass>(Ref, IdentTy);
    return IdentTy;
}

Type *ExprTypeAnnotatorPass::visitProcedureCall(ProcedureCall *Call) {
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

Type *ExprTypeAnnotatorPass::visitVectorLiteral(VectorLiteral *VecLit) {
    Type *WidestType = nullptr;
    bool IsFirst = true;

    if (!VecLit->numOfChildren())
        throw runtime_error("Unimplemented");

    for (auto *ChildExpr : *VecLit) {
        auto ChildTy = visit(ChildExpr);

        if (!ChildTy->isScalarTy())
            throw std::runtime_error("Vector literal can only contain scalar types");

        if (IsFirst)
            WidestType = ChildTy, IsFirst = false;
        else
            WidestType = getWiderType(WidestType, ChildTy);

        if (!WidestType)
            throw runtime_error("Vector literal has members that are of incompatible type");
    }

    // Pass 2: Promote all elements to the highest type
    for (size_t I = 0; I < VecLit->numOfChildren(); ++I) {
        auto ChildExpr = VecLit->getChildAt(I);
        auto ChildTy = visit(ChildExpr);
        if (ChildTy->isSameTypeAs(WidestType))
            continue;
        auto Casted = wrapWithCastTo(ChildExpr, WidestType);
        VecLit->setExprAtPos(Casted, I);
    }

    // Get the vector type
    auto VecTy = TypeReg->getVectorType(WidestType, (int) VecLit->numOfChildren());
    cast<VectorTy>(VecTy)->setSizeExpr(getIntLiteralWithVal((long) VecLit->numOfChildren()));
    annotate(VecLit, VecTy);
    return VecTy;
}

Type *ExprTypeAnnotatorPass::visitIndex(Index *Idx) {
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

Type *ExprTypeAnnotatorPass::visitInterval(Interval *Int) {
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


Type *ExprTypeAnnotatorPass::visitByOp(ByOp *By) {
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

Type *ExprTypeAnnotatorPass::visitDotProduct(DotProduct *Dot) {
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

Type *ExprTypeAnnotatorPass::visitConcat(Concat *Concat) {
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
        cast<VectorTy>(LType)->setSizeExpr(getIntLiteralWithVal(1));
        LExpr = wrapWithCastTo(LExpr, LType);
    }

    // Similarly for RHS
    if (!isa<VectorTy>(RType)) {
        RType = TypeReg->getVectorType(RType, 1);
        cast<VectorTy>(RType)->setSizeExpr(getIntLiteralWithVal(1));
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
        cast<VectorTy>(ResTy)->setSizeExpr(
                getAddOpBetween(LVecTy->getSizeExpr(), RVecTy->getSizeExpr()));
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


Type *ExprTypeAnnotatorPass::getWiderType(Type *Ty1, Type *Ty2) {
    if (Ty1->isCompositeTy() != Ty2->isCompositeTy())
        throw std::runtime_error("Called wider type with incompatible args");

    if (Ty1->canPromoteTo(Ty2))
        return Ty2;

    if (Ty2->canPromoteTo(Ty1))
        return Ty1;

    return nullptr;
}

bool ExprTypeAnnotatorPass::isTypeSizeKnown(Type *Ty) {
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

Type *ExprTypeAnnotatorPass::visitOutStream(OutStream *Out) {
    auto ExprTy = visit(Out->getOutStreamExpr());
    if (!ExprTy->isOpaqueTy())
        return nullptr;

    auto Cast = wrapWithCastTo(Out->getOutStreamExpr(), TypeReg->getCharTy());
    Out->setOutStreamExpr(Cast);
    return nullptr;
}

Type *ExprTypeAnnotatorPass::visitConditionalLoop(ConditionalLoop *Loop) {
    visit(Loop->getBlock());
    auto ExprTy = visit(Loop->getConditional());
    if (!ExprTy->isOpaqueTy())
        return nullptr;

    auto Cast = wrapWithCastTo(Loop->getConditional(), TypeReg->getBooleanTy());
    Loop->setConditional(Cast);
    return nullptr;
}

Type *ExprTypeAnnotatorPass::visitConditional(Conditional *Cond) {
    visit(Cond->getBlock());
    auto ExprTy = visit(Cond->getConditional());
    if (!ExprTy->isOpaqueTy())
        return nullptr;

    auto Cast = wrapWithCastTo(Cond->getConditional(), TypeReg->getBooleanTy());
    Cond->setConditional(Cast);
    return nullptr;
}

Type *ExprTypeAnnotatorPass::visitConditionalElse(ConditionalElse *Cond) {
    visit(Cond->getIfBlock());
    visit(Cond->getElseBlock());
    auto ExprTy = visit(Cond->getConditional());
    if (!ExprTy->isOpaqueTy())
        return nullptr;

    auto Cast = wrapWithCastTo(Cond->getConditional(), TypeReg->getBooleanTy());
    Cond->setConditional(Cast);
    return nullptr;
}


Type *ExprTypeAnnotatorPass::visitGenerator(Generator *Gen) {
    visit(Gen->getDomainVar());
    auto DomainType = visit(Gen->getDomain());
    auto InnerType = visit(Gen->getExpr());

    Type* ResTy = nullptr;
    if (dyn_cast<VectorTy>(DomainType)) {
        ResTy = TypeReg->getVectorType(InnerType, dyn_cast<VectorTy>(DomainType)->getSize());
    }
    else if (dyn_cast<IntervalTy>(DomainType)) {
        // Cast the interval to a vector<int>
        auto CastNode = PM->Builder.build<ExplicitCast>();
        CastNode->setExpr(Gen->getDomain());
        CastNode->setTargetType(PM->TypeReg.getVectorType(PM->TypeReg.getIntegerTy()));
        ResTy = CastNode->getTargetType();
    }

    PM->setAnnotation<ExprTypeAnnotatorPass>(Gen, ResTy);
    return ResTy;
}


Type *ExprTypeAnnotatorPass::visitMatrixGenerator(MatrixGenerator *Gen) {
    visit(Gen->getRowDomainVar());
    visit(Gen->getColumnDomainVar());

    auto RowType = visit(Gen->getRowDomain());
    Type *RowResTy = nullptr;
    if (dyn_cast<VectorTy>(RowType)) {
        auto RowVectorType = dyn_cast<VectorTy>(RowType);
        RowResTy = TypeReg->getVectorType(RowVectorType->getInnerTy(), RowVectorType->getSize());
    }
    else if (dyn_cast<IntervalTy>(RowType)) {
        // Cast the interval to a vector<int>
        auto CastNode = PM->Builder.build<ExplicitCast>();
        CastNode->setExpr(Gen->getRowDomain());
        CastNode->setTargetType(PM->TypeReg.getVectorType(PM->TypeReg.getIntegerTy()));
        RowResTy = CastNode->getTargetType();
    }

    auto ColumnType = visit(Gen->getRowDomain());
    Type *ColumnResTy = nullptr;
    if (dyn_cast<VectorTy>(ColumnType)) {
        auto ColumnVectorType = dyn_cast<VectorTy>(ColumnType);
        ColumnResTy = TypeReg->getVectorType(ColumnVectorType->getInnerTy(), ColumnVectorType->getSize());
    }
    else if (dyn_cast<IntervalTy>(ColumnType)) {
        // Cast the interval to a vector<int>
        auto CastNode = PM->Builder.build<ExplicitCast>();
        CastNode->setExpr(Gen->getColumnDomain());
        CastNode->setTargetType(PM->TypeReg.getVectorType(PM->TypeReg.getIntegerTy()));
        ColumnResTy = CastNode->getTargetType();
    }

    auto ExprTy = visit(Gen->getExpr());
    auto ResTy = TypeReg->getMatrixType(ExprTy,
                                        dyn_cast<VectorTy>(RowResTy)->getSize(),
                                        dyn_cast<VectorTy>(ColumnResTy)->getSize());

    PM->setAnnotation<ExprTypeAnnotatorPass>(Gen, ResTy);
    return ResTy;
}


Type *ExprTypeAnnotatorPass::visitFilter(Filter *Filter) {
    visit(Filter->getDomainVar());
    auto DomainType = visit(Filter->getDomain());

    Type* DomainResTy = nullptr;
    if (dyn_cast<VectorTy>(DomainType)) {
        auto VectorType = dyn_cast<VectorTy>(DomainType);
        DomainResTy = TypeReg->getVectorType(VectorType->getInnerTy(), -1);
    }
    else if (dyn_cast<IntervalTy>(DomainType)) {
        // Cast the interval to a vector<int>
        auto CastNode = PM->Builder.build<ExplicitCast>();
        CastNode->setExpr(Filter->getDomain());
        CastNode->setTargetType(PM->TypeReg.getVectorType(PM->TypeReg.getIntegerTy()));
        DomainResTy = CastNode->getTargetType();
    }

    map<string, int> Temp{};

    vector<Type *> ChildTypes;
    for (auto ChildExpr: *Filter->getPredicatedList()) {
        visit(ChildExpr);
        auto ElementTy = PM->TypeReg.getVectorType(dyn_cast<VectorTy>(DomainResTy)->getInnerTy());
        ChildTypes.emplace_back(ElementTy);
    }
    // There's an additional vector
    ChildTypes.emplace_back(PM->TypeReg.getVectorType(dyn_cast<VectorTy>(DomainResTy)->getInnerTy()));

    auto TupleTy = PM->TypeReg.getTupleType(ChildTypes, Temp);
    PM->setAnnotation<ExprTypeAnnotatorPass>(Filter, TupleTy);
    return TupleTy;
}

bool ExprTypeAnnotatorPass::isInferredSizedType(Type *Ty) {
    if (auto VecTy = dyn_cast<VectorTy>(Ty))
        return !VecTy->getSizeExpr();

    if (auto MatTy = dyn_cast<MatrixTy>(Ty))
        return !MatTy->getColSizeExpr() || !MatTy->getRowSizeExpr();

    return false;
}

void ExprTypeAnnotatorPass::copyOverCompositeSizeTypes(Type *Src, Type *Dest) {
    if (auto VecTy = dyn_cast<VectorTy>(Src)) {
        auto DestVec = dyn_cast<VectorTy>(Dest);
        if (!DestVec)
            throw runtime_error("Casting a non vector to an "
                                "inferred size vector");
        if (!DestVec->getSizeExpr())
            DestVec->setSizeExpr(VecTy->getSizeExpr());
        return;
    }

    auto SrcMat = dyn_cast<MatrixTy>(Src);
    auto DestMat = dyn_cast<MatrixTy>(Dest);
    if (!SrcMat || !DestMat)
        throw runtime_error("Malformed cast");

    if (!DestMat->getRowSizeExpr())
        DestMat->setRowSizeExpr(SrcMat->getRowSizeExpr());

    if (!DestMat->getColSizeExpr())
        DestMat->setColSizeExpr(SrcMat->getColSizeExpr());
}

IntLiteral *ExprTypeAnnotatorPass::getIntLiteralWithVal(long Val) {
    auto Lit = PM->Builder.build<IntLiteral>();
    Lit->setIntVal(Val);
    return Lit;
}

ArithmeticOp *ExprTypeAnnotatorPass::getAddOpBetween(ASTNodeT *N1, ASTNodeT *N2) {
    auto Add = PM->Builder.build<ArithmeticOp>();
    Add->setOp(ArithmeticOp::ADD);
    Add->setLeftExpr(N1);
    Add->setRightExpr(N2);
    return Add;
}

void ExprTypeAnnotatorPass::visitTypeSizeExpressions(Type *T) {
    if (auto VecTy = dyn_cast<VectorTy>(T)) {
        if (VecTy->getSizeExpr())
            visit(VecTy->getSizeExpr());
        return;
    }

    if (auto MatTy = dyn_cast<MatrixTy>(T)) {
        if (MatTy->getColSizeExpr())
            visit(MatTy->getColSizeExpr());

        if (MatTy->getRowSizeExpr())
            visit(MatTy->getRowSizeExpr());
    }
}
