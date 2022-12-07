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

    auto GetTargetTyForOpaqueCasts = [&](const Type *OtherTy) {
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

            if (!RType->isSameTypeAs(WiderTy)) {
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

            if (!LType->isSameTypeAs(WiderTy)) {
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

const Type *ExprTypeAnnotatorPass::visitArithmeticOp(ArithmeticOp *Op) {
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
            auto GetResType = [&](const Type *T1, const Type *T2) {
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

        if (!LeftType->isSameTypeAs(RightType))
            throw InvalidComparisonError(Op, LeftType->getTypeName(), RightType->getTypeName());

        PM->setAnnotation<ExprTypeAnnotatorPass>(Op, PM->TypeReg.getBooleanTy());
        return PM->TypeReg.getBooleanTy();

    } else {
        // All other "logical ops" are only supported for booleans.

        if (isa<VectorTy>(LeftType) && isa<VectorTy>(RightType)){
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

        } else if (isa<MatrixTy>(LeftType) && isa<MatrixTy>(RightType)) {
            auto LeftMatTy = cast<MatrixTy>(LeftType);
            auto RightMatTy = cast<MatrixTy>(RightType);

            // TODO switch to comparing left type only?
            if (!LeftMatTy->getInnerTy()->isSameTypeAs(RightMatTy->getInnerTy())) {
                throw runtime_error("Matrix types must be of the same type");
            }

            if (!LeftMatTy->getInnerTy()->isSameTypeAs(PM->TypeReg.getBooleanTy())) {
                throw InvalidComparisonOpError(Op, LeftMatTy->getInnerTy()->getTypeName());
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

const Type *ExprTypeAnnotatorPass::visitMemberAccess(MemberAccess *MAccess) {
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

const Type *ExprTypeAnnotatorPass::visitUnaryOp(UnaryOp *Op) {
    auto ChildType = visit(Op->getExpr());
    if (Op->getOpKind() == UnaryOp::NOT) {
        if (!ChildType->isValidForUnaryNot()) {

            if (isa<VectorTy>(ChildType) && dyn_cast<VectorTy>(ChildType)->getInnerTy()->isValidForUnaryNot()) {
                auto NewVecTy = PM->TypeReg.getVectorType(PM->TypeReg.getBooleanTy(), dyn_cast<VectorTy>(ChildType)->getSize());
                annotate(Op, NewVecTy);
                return NewVecTy;
            } else if (isa<MatrixTy>(ChildType) && dyn_cast<MatrixTy>(ChildType)->getInnerTy()->isValidForUnaryNot()) {
                auto NewMatTy = PM->TypeReg.getMatrixType(PM->TypeReg.getBooleanTy(), dyn_cast<MatrixTy>(ChildType)->getNumOfRows(), dyn_cast<MatrixTy>(ChildType)->getNumOfColumns());
                annotate(Op, NewMatTy);
                return NewMatTy;
            }

            throw runtime_error("Unary not not supported for this type");
        }
    } else {
        if (!ChildType->isValidForUnaryAddOrSub()) {
            if (isa<VectorTy>(ChildType) && dyn_cast<VectorTy>(ChildType)->getInnerTy()->isValidForUnaryAddOrSub()) {
                auto NewVecTy = PM->TypeReg.getVectorType(PM->TypeReg.getBooleanTy(), dyn_cast<VectorTy>(ChildType)->getSize());
                annotate(Op, NewVecTy);
                return NewVecTy;
            } else if (isa<MatrixTy>(ChildType) && dyn_cast<MatrixTy>(ChildType)->getInnerTy()->isValidForUnaryAddOrSub()) {
                auto NewMatTy = PM->TypeReg.getMatrixType(PM->TypeReg.getBooleanTy(), dyn_cast<MatrixTy>(ChildType)->getNumOfRows(), dyn_cast<MatrixTy>(ChildType)->getNumOfColumns());
                annotate(Op, NewMatTy);
                return NewMatTy;
            }

            throw runtime_error("Unary Add or Sub not supported for this type");
        }
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
    }
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
    // TODO check if baseexpr is simply an ID?
    // TODO run pass to convert vector indexing to simple loop
    auto BaseTy = visit(Ref->getBaseExpr());
    if (isa<VectorTy>(BaseTy)) {
        auto VecTy = dyn_cast<VectorTy>(BaseTy);
        auto IdxTy = visit(Ref->getIndexExpr());
        if (isa<IntegerTy>(IdxTy)) {
            auto ResultTy = VecTy->getInnerTy();
            ResultTy = PM->TypeReg.getVarTypeOf(ResultTy);
            PM->setAnnotation<ExprTypeAnnotatorPass>(Ref, ResultTy);
            return ResultTy;
        } else if (isa<VectorTy>(IdxTy)) {
            auto IdxVecTy = dyn_cast<VectorTy>(IdxTy);
            if (!isa<IntegerTy>(IdxVecTy->getInnerTy()))
                throw runtime_error("Index vector must be of integer type.");
            // TODO add new pass to check bounds of index expressions
            auto ResultTy = PM->TypeReg.getVectorType(VecTy->getInnerTy(), IdxVecTy->getSize(), false);
            PM->setAnnotation<ExprTypeAnnotatorPass>(Ref, ResultTy);
            return ResultTy;
        }
        throw runtime_error("Index type not set for index reference.");
    } else if (isa<MatrixTy>(BaseTy)) {
        auto MatTy = dyn_cast<MatrixTy>(BaseTy);
        auto Idx1Ty = visit(Ref->getIndexExpr());
        auto Idx2Ty = visit(Ref->getIndex2Expr());
        if (isa<IntegerTy>(Idx1Ty) && isa<IntegerTy>(Idx2Ty)) {
            auto ResultTy = MatTy->getInnerTy();
            ResultTy = PM->TypeReg.getVarTypeOf(ResultTy);
            PM->setAnnotation<ExprTypeAnnotatorPass>(Ref, ResultTy);
            return ResultTy;

        } else if (isa<VectorTy>(Idx1Ty) && isa<VectorTy>(Idx2Ty)) {
            auto Idx1VecTy = dyn_cast<VectorTy>(Idx1Ty);
            auto Idx2VecTy = dyn_cast<VectorTy>(Idx2Ty);
            if (!isa<IntegerTy>(Idx1VecTy->getInnerTy()) || !isa<IntegerTy>(Idx2VecTy->getInnerTy()))
                throw runtime_error("Index vector must be of integer type.");
            // TODO add new pass to check bounds of index expressions
            auto ResultTy = PM->TypeReg.getMatrixType(MatTy->getInnerTy(), Idx1VecTy->getSize(), Idx2VecTy->getSize(), false);
            PM->setAnnotation<ExprTypeAnnotatorPass>(Ref, ResultTy);
            return ResultTy;

        } else if (isa<VectorTy>(Idx1Ty) && isa<IntegerTy>(Idx2Ty)) {
            auto Idx1VecTy = dyn_cast<VectorTy>(Idx1Ty);
            if (!isa<IntegerTy>(Idx1VecTy->getInnerTy()))
                throw runtime_error("Index vector must be of integer type.");
            // TODO add new pass to check bounds of index expressions
            auto ResultTy = PM->TypeReg.getVectorType(MatTy->getInnerTy(), Idx1VecTy->getSize(), false);
            PM->setAnnotation<ExprTypeAnnotatorPass>(Ref, ResultTy);
            return ResultTy;

        } else if (isa<IntegerTy>(Idx1Ty) && isa<VectorTy>(Idx2Ty)) {
            auto Idx2VecTy = dyn_cast<VectorTy>(Idx2Ty);
            if (!isa<IntegerTy>(Idx2VecTy->getInnerTy()))
                throw runtime_error("Index vector must be of integer type.");
            // TODO add new pass to check bounds of index expressions
            auto ResultTy = PM->TypeReg.getVectorType(MatTy->getInnerTy(), Idx2VecTy->getSize(), false);
            PM->setAnnotation<ExprTypeAnnotatorPass>(Ref, ResultTy);
            return ResultTy;
        }
        throw runtime_error("Index type not set for index reference.");
    }
    throw runtime_error("Base type is not a vector or matrix.");
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
    const Type *WidestType = nullptr;
    bool IsFirst = true;

    if (!VecLit->numOfChildren())
        throw runtime_error("Unimplemented");

    for (auto *ChildExpr : *VecLit) {
        auto ChildTy = visit(ChildExpr);
        bool IsMatrix = ChildTy->getKind() == Type::TypeKind::T_Vector;

        // If a vector has an inner type of vector of scalars, then it is a matrix
        if (IsMatrix) {

            auto InVecTy = dyn_cast<VectorTy>(ChildTy);
            if (!InVecTy->getInnerTy()->isScalarTy()) {
                throw runtime_error("Vector literal has a vector of vectors as an element.");
            }
            if (!WidestType) {
                WidestType = ChildTy;
                continue;
            }

            // If the current type is a scalar and we encounter a matrix, then we can promote it like normal
            if (WidestType->isScalarTy()) {
                WidestType = ChildTy->getPromotedType(WidestType);
                continue;
            }

            // All of this is done here since the type class should not be generating types, only the type registry
            // should be doing that. This is to cover the special case where we have to use the type from one existing
            // vector and the inner type from another vector.
            auto Size = InVecTy->getPromotedVectorSizeForMatrix(dyn_cast<VectorTy>(WidestType));
            auto PromotedTy = InVecTy->getInnerTy()->getPromotedType(dyn_cast<VectorTy>(WidestType)->getInnerTy());
            WidestType = PM->TypeReg.getVectorType(PromotedTy, Size);

        } else if (!ChildTy->isScalarTy()) {
                throw std::runtime_error("Vector literal can only contain scalar types");
        }

        if (IsFirst)
            WidestType = ChildTy, IsFirst = false;
        else
            WidestType = getWiderType(WidestType, ChildTy);

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
    if (isa<VectorTy>(WidestType)) {
        auto VecTy = dyn_cast<VectorTy>(WidestType);
        auto ResultTy = PM->TypeReg.getMatrixType(VecTy->getInnerTy(), VecLit->numOfChildren(), VecTy->getSize());
        PM->setAnnotation<ExprTypeAnnotatorPass>(VecLit, ResultTy);
        return ResultTy;
    } else {
        auto VecTy = TypeReg->getVectorType(WidestType, (int) VecLit->numOfChildren());
        annotate(VecLit, VecTy);
        return VecTy;
    }
}

const Type *ExprTypeAnnotatorPass::visitStringLiteral(StringLiteral *StrLit) {


    if (!StrLit->numOfChildren())
        throw runtime_error("Unimplemented");

    const Type *CharTy = PM->TypeReg.getCharTy();

    // Get the vector type
    auto StrTy = PM->TypeReg.getStringType(CharTy, (int) StrLit->numOfChildren());
    annotate(StrLit, StrTy);
    return StrTy;

}

const Type *ExprTypeAnnotatorPass::visitIndex(Index *Idx) {
    auto BaseTy = visit(Idx->getBaseExpr());

    if (isa<VectorTy>(BaseTy)) {
        auto VecTy = dyn_cast<VectorTy>(BaseTy);
        auto IdxTy = visit(Idx->getIndexExpr());

        if (isa<IntegerTy>(IdxTy)) {
            auto ResultTy = VecTy->getInnerTy();
            PM->setAnnotation<ExprTypeAnnotatorPass>(Idx, ResultTy);
            return ResultTy;

        } else if (isa<VectorTy>(IdxTy)) {
            auto IdxVecTy = dyn_cast<VectorTy>(IdxTy);
            if (!isa<IntegerTy>(IdxVecTy->getInnerTy()))
                throw runtime_error("Index vector must be of integer type.");
            // TODO add new pass to check bounds of index expressions
            auto ResultTy = PM->TypeReg.getVectorType(VecTy->getInnerTy(), IdxVecTy->getSize());
            PM->setAnnotation<ExprTypeAnnotatorPass>(Idx, ResultTy);
            return ResultTy;
        }
        throw runtime_error("Index type not set for index reference.");

    } else if (isa<MatrixTy>(BaseTy)) {
        auto MatTy = dyn_cast<MatrixTy>(BaseTy);
        auto Idx1Ty = visit(Idx->getIndexExpr());
        auto Idx2Ty = visit(Idx->getIndex2Expr());
        if (isa<IntegerTy>(Idx1Ty) && isa<IntegerTy>(Idx2Ty)) {
            auto ResultTy = MatTy->getInnerTy();
            PM->setAnnotation<ExprTypeAnnotatorPass>(Idx, ResultTy);
            return ResultTy;

        } else if (isa<VectorTy>(Idx1Ty) && isa<VectorTy>(Idx2Ty)) {
            auto Idx1VecTy = dyn_cast<VectorTy>(Idx1Ty);
            auto Idx2VecTy = dyn_cast<VectorTy>(Idx2Ty);
            if (!isa<IntegerTy>(Idx1VecTy->getInnerTy()) || !isa<IntegerTy>(Idx2VecTy->getInnerTy()))
                throw runtime_error("Index vector must be of integer type.");
            // TODO add new pass to check bounds of index expressions
            auto ResultTy = PM->TypeReg.getMatrixType(MatTy->getInnerTy(), Idx1VecTy->getSize(), Idx2VecTy->getSize());
            PM->setAnnotation<ExprTypeAnnotatorPass>(Idx, ResultTy);
            return ResultTy;

        } else if (isa<VectorTy>(Idx1Ty) && isa<IntegerTy>(Idx2Ty)) {
            auto Idx1VecTy = dyn_cast<VectorTy>(Idx1Ty);
            if (!isa<IntegerTy>(Idx1VecTy->getInnerTy()))
                throw runtime_error("Index vector must be of integer type.");
            // TODO add new pass to check bounds of index expressions
            auto ResultTy = PM->TypeReg.getVectorType(MatTy->getInnerTy(), Idx1VecTy->getSize());
            PM->setAnnotation<ExprTypeAnnotatorPass>(Idx, ResultTy);
            return ResultTy;

        } else if (isa<IntegerTy>(Idx1Ty) && isa<VectorTy>(Idx2Ty)) {
            auto Idx2VecTy = dyn_cast<VectorTy>(Idx2Ty);
            if (!isa<IntegerTy>(Idx2VecTy->getInnerTy()))
                throw runtime_error("Index vector must be of integer type.");
            // TODO add new pass to check bounds of index expressions
            auto ResultTy = PM->TypeReg.getVectorType(MatTy->getInnerTy(), Idx2VecTy->getSize());
            PM->setAnnotation<ExprTypeAnnotatorPass>(Idx, ResultTy);
            return ResultTy;
        }
        throw runtime_error("Index type not set for index reference.");
    }
    throw runtime_error("Base type is not a vector or matrix.");

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

    auto LHS = visit(Dot->getLHS());
    auto RHS = visit(Dot->getRHS());

    if (isa<VectorTy>(LHS) && isa<VectorTy>(RHS)) {

        // Dot product

        auto LVecTy = dyn_cast<VectorTy>(LHS);
        auto RVecTy = dyn_cast<VectorTy>(RHS);

        // If the inner types are not the same then the dot product is invalid.
        if (!LVecTy->getInnerTy()->isSameTypeAs(RVecTy->getInnerTy()))
            throw std::runtime_error("Dot product can only be applied to vector types"
                                    " with the same inner type");

        if (!LVecTy->isSameTypeAs(RVecTy))
            throw std::runtime_error("Dot product can only be applied to vectors of the same type and size");

        auto ResTy = LVecTy->getInnerTy();
        annotateWithConst(Dot, ResTy);
        return ResTy;

    } else if (isa<MatrixTy>(LHS) && isa<MatrixTy>(RHS)) {

        // Matrix multiplication

        auto LHSMatTy = dyn_cast<MatrixTy>(LHS);
        auto RHSMatTy = dyn_cast<MatrixTy>(RHS);

        if (LHSMatTy->getNumOfColumns() != RHSMatTy->getNumOfRows()
            && LHSMatTy->getNumOfRows() != -1 && RHSMatTy->getNumOfColumns() != -1)
            throw std::runtime_error("Matrix multiplication can only be applied to matrices of compatible sizes");

        auto LHSInnerTy = LHSMatTy->getInnerTy();
        auto RHSInnerTy = RHSMatTy->getInnerTy();
        if (!LHSInnerTy->isSameTypeAs(RHSInnerTy))
            throw std::runtime_error("Matrix multiplication can only be applied to matrices of the same inner type");

        if (!LHSInnerTy->isValidForArithOps())
            throw std::runtime_error("Matrix multiplication can only be applied to matrices of arithmetic types");

        auto ResTy = PM->TypeReg.getMatrixType(LHSInnerTy, LHSMatTy->getNumOfRows(), RHSMatTy->getNumOfColumns());
        annotateWithConst(Dot, ResTy);
        return ResTy;

    }
    throw std::runtime_error("Dot product/matrix multiplication can only be applied to vector/matrix types");

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

const Type *ExprTypeAnnotatorPass::visitOutStream(OutStream *Out) {
    auto ExprTy = visit(Out->getOutStreamExpr());
    if (!ExprTy->isOpaqueTy())
        return nullptr;

    auto Cast = wrapWithCastTo(Out->getOutStreamExpr(), TypeReg->getCharTy());
    Out->setOutStreamExpr(Cast);
    return nullptr;
}

const Type *ExprTypeAnnotatorPass::visitConditionalLoop(ConditionalLoop *Loop) {
    visit(Loop->getBlock());
    auto ExprTy = visit(Loop->getConditional());
    if (!ExprTy->isOpaqueTy())
        return nullptr;

    auto Cast = wrapWithCastTo(Loop->getConditional(), TypeReg->getBooleanTy());
    Loop->setConditional(Cast);
    return nullptr;
}

const Type *ExprTypeAnnotatorPass::visitConditional(Conditional *Cond) {
    visit(Cond->getBlock());
    auto ExprTy = visit(Cond->getConditional());
    if (!ExprTy->isOpaqueTy())
        return nullptr;

    auto Cast = wrapWithCastTo(Cond->getConditional(), TypeReg->getBooleanTy());
    Cond->setConditional(Cast);
    return nullptr;
}

const Type *ExprTypeAnnotatorPass::visitConditionalElse(ConditionalElse *Cond) {
    visit(Cond->getIfBlock());
    visit(Cond->getElseBlock());
    auto ExprTy = visit(Cond->getConditional());
    if (!ExprTy->isOpaqueTy())
        return nullptr;

    auto Cast = wrapWithCastTo(Cond->getConditional(), TypeReg->getBooleanTy());
    Cond->setConditional(Cast);
    return nullptr;
}
