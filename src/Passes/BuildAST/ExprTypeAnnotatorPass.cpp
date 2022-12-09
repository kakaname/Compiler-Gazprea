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


    if (RType->isSameTypeAs(PM->TypeReg.getIntervalTy())) {
        auto NewRType = PM->TypeReg.getVectorType(
                PM->TypeReg.getIntegerTy());
        auto Cast = wrapWithCastTo(RExpr, NewRType);
        Op->setRightExpr(Cast);
        RExpr = Cast;
        RType = NewRType;
    }
    if (LType->isSameTypeAs(PM->TypeReg.getIntervalTy())) {
        auto NewLType = PM->TypeReg.getVectorType(
                PM->TypeReg.getIntegerTy());
        auto Cast = wrapWithCastTo(LExpr, NewLType);
        Op->setLeftExpr(Cast);
        LExpr = Cast;
        LType = NewLType;
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

            if (!LInner->isSameTypeAs(WiderTy)) {
                auto Cast = wrapWithCastTo(LExpr, PM->TypeReg.getCompositeTyWithInner(
                        LType, WiderTy));
                Op->setLeftExpr(Cast);
            }

            if (!RInner->isSameTypeAs(WiderTy)) {
                auto Cast = wrapWithCastTo(RExpr, PM->TypeReg.getCompositeTyWithInner(
                        RType, WiderTy));
                Op->setRightExpr(Cast);
            }

            auto ResTy = PM->TypeReg.getCompositeTyWithInner(
                    LType, BoolTy);
            annotate(Op, ResTy);
            return ResTy;
        }

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

    if (RType->isSameTypeAs(PM->TypeReg.getIntervalTy())) {
        auto NewRType = PM->TypeReg.getVectorType(
                PM->TypeReg.getIntegerTy());
        auto Cast = wrapWithCastTo(RExpr, NewRType);
        Op->setRightExpr(Cast);
        RExpr = Cast;
        RType = NewRType;
    }

    if (LType->isSameTypeAs(PM->TypeReg.getIntervalTy())) {
        auto NewLType = PM->TypeReg.getVectorType(
                PM->TypeReg.getIntegerTy());
        auto Cast = wrapWithCastTo(LExpr, NewLType);
        Op->setLeftExpr(Cast);
        LExpr = Cast;
        LType = NewLType;
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

            if (!LInner->isSameTypeAs(WiderTy)) {
                auto Cast = wrapWithCastTo(LExpr, PM->TypeReg.getCompositeTyWithInner(
                        LType, WiderTy));
                Op->setLeftExpr(Cast);
            }

            if (!RInner->isSameTypeAs(WiderTy)) {
                auto Cast = wrapWithCastTo(RExpr, PM->TypeReg.getCompositeTyWithInner(
                        RType, WiderTy));
                Op->setRightExpr(Cast);
            }

            auto ResTy = PM->TypeReg.getCompositeTyWithInner(
                    LType, WiderTy);
            annotate(Op, ResTy);
            return ResTy;
        }

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
    auto LExpr = Op->getLeftExpr();
    auto RExpr = Op->getRightExpr();
    auto LType = visit(LExpr);
    auto RType = visit(RExpr);
    auto BoolType = TypeReg->getBooleanTy();

    // If both of them are opaque types, we cast them to the opaque type target.
    if (LType->isOpaqueTy() && RType->isOpaqueTy()) {
        if (!OpaqueTyCastTarget)
            throw NullIdentityOpError(Op);

        if (!OpaqueTyCastTarget->isValidForComparisonOp())
            throw InvalidComparisonOpError(Op, OpaqueTyCastTarget->getTypeName());

        auto LeftCast = wrapWithCastTo(LExpr, OpaqueTyCastTarget);
        auto RightCast = wrapWithCastTo(RExpr, OpaqueTyCastTarget);
        Op->setLeftExpr(LeftCast);
        Op->setRightExpr(RightCast);
        annotateWithConst(Op, OpaqueTyCastTarget);
        return OpaqueTyCastTarget;
    }

    // At least one of them is opaque.
    if (LType->isOpaqueTy() || RType->isOpaqueTy()) {
        //
        if (LType->isOpaqueTy()) {
            auto Cast = wrapWithCastTo(LExpr, RType);
            Op->setLeftExpr(Cast);
            LType = RType;
        } else {
            auto Cast = wrapWithCastTo(RExpr, LType);
            Op->setRightExpr(Cast);
            RType = LType;
        }
    }

    if (RType->isSameTypeAs(PM->TypeReg.getIntervalTy())) {
        auto NewRType = PM->TypeReg.getVectorType(
                PM->TypeReg.getIntegerTy());
        auto Cast = wrapWithCastTo(RExpr, NewRType);
        Op->setRightExpr(Cast);
        RExpr = Cast;
        RType = NewRType;
    }

    if (LType->isSameTypeAs(PM->TypeReg.getIntervalTy())) {
        auto NewLType = PM->TypeReg.getVectorType(
                PM->TypeReg.getIntegerTy());
        auto Cast = wrapWithCastTo(LExpr, NewLType);
        Op->setLeftExpr(Cast);
        LExpr = Cast;
        LType = NewLType;
    }

    if (Op->getOpKind() == LogicalOp::EQ || Op->getOpKind() == LogicalOp::NEQ) {
        if (!LType->isValidForEq())
            throw InvalidEqualityOpError(Op, LType->getTypeName());

        if (!RType->isValidForEq())
            throw InvalidEqualityOpError(Op, RType->getTypeName());

        if (LType->isSameTypeAs(RType)) {
            PM->setAnnotation<ExprTypeAnnotatorPass>(Op, PM->TypeReg.getBooleanTy());
            return PM->TypeReg.getBooleanTy();
        }

        matchBoolPair(LType->isCompositeTy(), RType->isCompositeTy()) {
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

                if (!LInner->isSameTypeAs(WiderTy)) {
                    auto Cast = wrapWithCastTo(LExpr, PM->TypeReg.getCompositeTyWithInner(
                            LType, WiderTy));
                    Op->setLeftExpr(Cast);
                }

                if (!RInner->isSameTypeAs(WiderTy)) {
                    auto Cast = wrapWithCastTo(RExpr, PM->TypeReg.getCompositeTyWithInner(
                            RType, WiderTy));
                    Op->setRightExpr(Cast);
                }
                annotate(Op, BoolType);
                return BoolType;
            }

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
                    annotateWithConst(Op, BoolType);
                    return BoolType;
                }

                if (!RType->isSameTypeAs(WiderTy)) {
                    RExpr = wrapWithCastTo(RExpr, WiderTy);
                    Op->setRightExpr(RExpr);
                }

                annotateWithConst(Op, BoolType);
                return BoolType;
        }

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
                annotateWithConst(Op, BoolType);
                return BoolType;
            }

            if (!LType->isSameTypeAs(WiderTy))  {
                LExpr = wrapWithCastTo(LExpr, WiderTy);
                Op->setLeftExpr(LExpr);
            }
            annotateWithConst(Op, BoolType);
            return BoolType;
        }

            matchPattern(false, false): {
                auto WiderTy = getWiderType(LType, RType);
                if (!LType->isSameTypeAs(WiderTy)) {
                    auto Cast = wrapWithCastTo(LExpr, WiderTy);
                    Op->setLeftExpr(Cast);
                }

                if (!RType->isSameTypeAs(WiderTy)) {
                    auto Cast = wrapWithCastTo(RExpr, WiderTy);
                    Op->setRightExpr(Cast);
                }

                annotateWithConst(Op, BoolType);
                return BoolType;
            }
        }

    } else {
        // All other "logical ops" are only supported for booleans.
        matchBoolPair(LType->isCompositeTy(), RType->isCompositeTy()) {
            matchPattern(true, true): {
                if (!((isa<VectorTy>(LType) && isa<VectorTy>(RType)) ||
                      (isa<MatrixTy>(LType) && isa<MatrixTy>(RType))))
                    throw runtime_error("Operation between incompatible "
                                        "types " + LType->getTypeName() +
                                        RType->getTypeName());

                auto LInner = TypeRegistry::getInnerTyFromComposite(LType);
                auto RInner = TypeRegistry::getInnerTyFromComposite(RType);
                auto WiderTy = getWiderType(LInner, RInner);

                if (!isa<BoolTy>(LInner) || !isa<BoolTy>(RInner))
                    throw runtime_error("This operation is only supported for booleans");
                auto ResTy = TypeReg->getCompositeTyWithInner(LType, BoolType);
                annotateWithConst(Op, ResTy);
                return ResTy;
            }

            matchPattern(true, false): {
                auto LInner = TypeRegistry::getInnerTyFromComposite(LType);

                if (!isa<BoolTy>(LInner) || !isa<BoolTy>(RType))
                    throw runtime_error("This operation is only supported for booleans");

                auto ResTy = TypeReg->getCompositeTyWithInner(LType, BoolType);
                annotateWithConst(Op, ResTy);
                return ResTy;
            }

            matchPattern(false, true): {
                auto RInner = TypeRegistry::getInnerTyFromComposite(RType);

                if (!isa<BoolTy>(RInner) || !isa<BoolTy>(LType))
                    throw runtime_error("This operation is only supported for booleans");

                auto ResTy = TypeReg->getCompositeTyWithInner(RType, BoolType);
                annotateWithConst(Op, ResTy);
                return ResTy;
            }

            matchPattern(false, false): {
                if (!isa<BoolTy>(RType) || !isa<BoolTy>(LType))
                    throw runtime_error("This operation is only supported for booleans");
                annotateWithConst(Op, BoolType);
                return BoolType;
            }
        }
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

Type *ExprTypeAnnotatorPass::visitTupleDestruct(TupleDestruct *TupDestr) {
    vector<Type*> ChildTypes;
    for (auto *ChildExpr : *TupDestr) {
        auto ChildTy = visit(ChildExpr);
        ChildTy = PM->TypeReg.getVarTypeOf(ChildTy);
        ChildTypes.emplace_back(ChildTy);
    }
    map<string, int> Temp{};
    auto TupleTy = PM->TypeReg.getTupleType(ChildTypes, Temp);
    TupleTy = PM->TypeReg.getVarTypeOf(TupleTy);
    PM->setAnnotation<ExprTypeAnnotatorPass>(TupDestr, TupleTy);
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

Type *ExprTypeAnnotatorPass::visitBuiltInLen(LengthFunc *Len){
    visit(Len->getVector());
    PM->setAnnotation<ExprTypeAnnotatorPass>(Len, PM->TypeReg.getIntegerTy());

    return PM->TypeReg.getIntegerTy();

}

Type *ExprTypeAnnotatorPass::visitBuiltInRow(RowFunc *Row){
    visit(Row->getMatrix());

    PM->setAnnotation<ExprTypeAnnotatorPass>(Row, PM->TypeReg.getIntegerTy());

    return PM->TypeReg.getIntegerTy();

}

Type *ExprTypeAnnotatorPass::visitBuiltInCol(ColFunc *Col){
    visit(Col->getMatrix());
    PM->setAnnotation<ExprTypeAnnotatorPass>(Col, PM->TypeReg.getIntegerTy());

    return PM->TypeReg.getIntegerTy();

}

Type *ExprTypeAnnotatorPass::visitBuiltInReverse(ReverseFunc *Rev){
    auto retType = visit(Rev->getVector());
    PM->setAnnotation<ExprTypeAnnotatorPass>(Rev, retType);

    return retType;

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
    auto CastedTy = visit(Cast->getExpr());

    if (auto VecTy = dyn_cast<VectorTy>(Cast->getTargetType()))
        if (VecTy->getSizeExpr())
            visit(VecTy->getSizeExpr());

    if (auto MatTy = dyn_cast<MatrixTy>(Cast->getTargetType())) {
        if (MatTy->getRowSizeExpr())
            visit(MatTy->getRowSizeExpr());

        if (MatTy->getColSizeExpr())
            visit(MatTy->getColSizeExpr());
    }

    matchBoolPair(CastedTy->isCompositeTy(), Cast->getTargetType()->isCompositeTy()) {
        matchPattern(true, true):
        matchPattern(false, false): {
            annotateWithConst(Cast, Cast->getTargetType());
            return Cast->getTargetType();
        }

        matchPattern(true, false): {
            throw runtime_error("Cannot cast a composite type to a scalar");
        }

        matchPattern(false, true): {
            auto InnerTy = TypeRegistry::getInnerTyFromComposite(Cast->getTargetType());
            if (!CastedTy->isSameTypeAs(InnerTy) && !isa<IntervalTy>(CastedTy))
                Cast->setExpr(wrapWithCastTo(Cast->getExpr(), InnerTy));
            annotateWithConst(Cast, Cast->getTargetType());
            return Cast->getTargetType();
        }
    }
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
    auto CastedTy = visit(Cast->getExpr());

    if (auto VecTy = dyn_cast<VectorTy>(Cast->getTargetType()))
        if (VecTy->getSizeExpr())
            visit(VecTy->getSizeExpr());

    if (auto MatTy = dyn_cast<MatrixTy>(Cast->getTargetType())) {
        if (MatTy->getRowSizeExpr())
            visit(MatTy->getRowSizeExpr());

        if (MatTy->getColSizeExpr())
            visit(MatTy->getColSizeExpr());
    }

    matchBoolPair(CastedTy->isCompositeTy(), Cast->getTargetType()->isCompositeTy()) {
        matchPattern(true, true):
        matchPattern(false, false): {
            annotateWithConst(Cast, Cast->getTargetType());
            return Cast->getTargetType();
        }

        matchPattern(true, false): {
            throw runtime_error("Cannot cast a composite type to a scalar");
        }

        matchPattern(false, true): {
            auto InnerTy = TypeRegistry::getInnerTyFromComposite(Cast->getTargetType());
            if (!CastedTy->isSameTypeAs(InnerTy) && !isa<IntervalTy>(CastedTy))
                Cast->setExpr(wrapWithCastTo(Cast->getExpr(), InnerTy));
            annotateWithConst(Cast, Cast->getTargetType());
            return Cast->getTargetType();
        }
    }
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

    if (!VecLit->numOfChildren()) {
        auto ResTy = TypeReg->getVectorType(TypeReg->getIntegerTy(), 0, true);
        auto LiteralSize = PM->Builder.build<IntLiteral>();
        LiteralSize->setIntVal(0);
        cast<VectorTy>(ResTy)->setSizeExpr(LiteralSize);
        annotate(VecLit, ResTy);
        return ResTy;
    }

    auto IsMatrix = [&]() {
        for (auto ChildExpr: *VecLit) {
            auto ChildTy = visit(ChildExpr);
            if (ChildTy->isCompositeTy() || isa<IntervalTy>(ChildTy))
                return true;
        }
        return false;
    }();

    if (!IsMatrix) {
        for (auto ChildExpr: *VecLit) {
            auto ChildTy = visit(ChildExpr);
            ChildTy = PM->TypeReg.getConstTypeOf(ChildTy);
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
        auto VecTy = TypeReg->getVectorType(WidestType, (int) VecLit->numOfChildren());
        annotate(VecLit, VecTy);
        return VecTy;
    }

    IsFirst = true;
    for (auto I = 0; I < VecLit->numOfChildren(); I++) {
        auto ChildExpr = VecLit->getExprAtPos(I);
        auto ChildTy = visit(ChildExpr);

        if (isa<IntervalTy>(ChildTy)) {
            auto VecTy = TypeReg->getVectorType(TypeReg->getIntegerTy());
            VecLit->setExprAtPos(wrapWithCastTo(ChildExpr, VecTy), I);
            ChildTy = VecTy;
        }

        if (!isa<VectorTy>(ChildTy))
            throw runtime_error("All elements inside a matrix literal must be vectors.");

        auto InnerTy = TypeRegistry::getInnerTyFromComposite(ChildTy);
        if (IsFirst)
            WidestType = InnerTy, IsFirst = false;
        else
            WidestType = getWiderType(WidestType, InnerTy);
    }

    for (auto I = 0; I < VecLit->numOfChildren(); I++) {
        auto ChildExpr = VecLit->getExprAtPos(I);
        auto ChildTy = visit(ChildExpr);
        auto InnerTy = TypeRegistry::getInnerTyFromComposite(ChildTy);
        if (InnerTy->isSameTypeAs(WidestType))
            continue;
        auto TargetTy = TypeReg->getVectorType(WidestType);
        VecLit->setExprAtPos(wrapWithCastTo(VecLit->getExprAtPos(I), TargetTy), I);
    }

    auto ResultTy = PM->TypeReg.getMatrixType(WidestType, VecLit->numOfChildren());
    PM->setAnnotation<ExprTypeAnnotatorPass>(VecLit, ResultTy);
    return ResultTy;
}

Type *ExprTypeAnnotatorPass::visitStringLiteral(StringLiteral *StrLit) {


    if (!StrLit->numOfChildren())
        throw runtime_error("Unimplemented");

    Type *CharTy = PM->TypeReg.getCharTy();

    // Get the vector type
    auto StrTy = dyn_cast<VectorTy>(PM->TypeReg.getVectorType(CharTy, (int) StrLit->numOfChildren(), true, true));
    annotate(StrLit, StrTy);
    return StrTy;

}

Type *ExprTypeAnnotatorPass::visitIndex(Index *Idx) {
    auto BaseTy = visit(Idx->getBaseExpr());

    if (isa<VectorTy>(BaseTy)) {
        auto VecTy = dyn_cast<VectorTy>(BaseTy);
        auto IdxTy = visit(Idx->getIndexExpr());

        if (isa<IntervalTy>(IdxTy)) {
            auto CastedTy = TypeReg->getVectorType(TypeReg->getIntegerTy());
            Idx->setIndexExpr(wrapWithCastTo(Idx->getIndexExpr(), CastedTy));
            IdxTy = CastedTy;
        }

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

        if (isa<IntervalTy>(Idx1Ty)) {
            auto CastedTy = TypeReg->getVectorType(TypeReg->getIntegerTy());
            Idx->setIndexExpr(wrapWithCastTo(Idx->getIndexExpr(), CastedTy));
            Idx1Ty = CastedTy;
        }

        if (isa<IntervalTy>(Idx2Ty)) {
            auto CastedTy = TypeReg->getVectorType(TypeReg->getIntegerTy());
            Idx->setIndex2Expr(wrapWithCastTo(Idx->getIndex2Expr(), CastedTy));
            Idx2Ty = CastedTy;
        }

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

Type *ExprTypeAnnotatorPass::visitInterval(Interval *Int) {
    auto Lower = visit(Int->getLowerExpr());
    auto Upper = visit(Int->getUpperExpr());

    auto IntTy = TypeReg->getIntegerTy();

    if (Upper->isOpaqueTy()) {
        Int->setUpperExpr(wrapWithCastTo(Int->getUpperExpr(), IntTy));
        Upper = IntTy;
    }

    if (Lower->isOpaqueTy()) {
        Int->setLowerExpr(wrapWithCastTo(Int->getLowerExpr(), IntTy));
        Lower = IntTy;
    }

    if (!isa<IntegerTy>(Lower) && !isa<IntegerTy>(Upper))
        throw runtime_error("Interval bounds must be integers");

    annotateWithConst(Int, TypeReg->getIntervalTy());
    return TypeReg->getIntervalTy();
}


Type *ExprTypeAnnotatorPass::visitByOp(ByOp *By) {
    auto BaseExprTy = visit(By->getBaseExpr());
    auto ByExprTy = visit(By->getByExpr());

    if (isa<IntervalTy>(BaseExprTy)) {
        auto VecTy = TypeReg->getVectorType(TypeReg->getIntegerTy());
        By->setBaseExpr(wrapWithCastTo(By->getBaseExpr(), VecTy));
        BaseExprTy = VecTy;
    }

    if (ByExprTy->isOpaqueTy()) {
        By->setByExpr(wrapWithCastTo(By->getBaseExpr(), TypeReg->getIntegerTy()));
        ByExprTy = TypeReg->getIntegerTy();
    }

    if (!isa<IntegerTy>(ByExprTy))
        throw runtime_error("The expression used in a by operation must be"
                            " integer");

    if (!isa<VectorTy>(BaseExprTy))
        throw runtime_error("Only vectors maybe used in a by expr");

    auto InnerTy = TypeRegistry::getInnerTyFromComposite(BaseExprTy);
    auto ResTy = TypeReg->getVectorType(InnerTy);
    annotate(By, ResTy);
    return ResTy;
}

Type *ExprTypeAnnotatorPass::visitDotProduct(DotProduct *Dot) {

    auto LHS = visit(Dot->getLHS());
    auto RHS = visit(Dot->getRHS());

    if (isa<IntervalTy>(LHS)) {
        auto VecTy = TypeReg->getVectorType(TypeReg->getIntegerTy());
        Dot->setLHS(wrapWithCastTo(Dot->getLHS(), VecTy));
        LHS = VecTy;
    }

    if (isa<IntervalTy>(RHS)) {
        auto VecTy = TypeReg->getVectorType(TypeReg->getIntegerTy());
        Dot->setLHS(wrapWithCastTo(Dot->getLHS(), VecTy));
        RHS = VecTy;
    }

    if (isa<VectorTy>(LHS) && isa<VectorTy>(RHS)) {
        // Dot product
        auto LVecTy = dyn_cast<VectorTy>(LHS);
        auto RVecTy = dyn_cast<VectorTy>(RHS);

        auto LInner = TypeRegistry::getInnerTyFromComposite(LVecTy);
        auto RInner = TypeRegistry::getInnerTyFromComposite(RVecTy);

        auto WiderTy = getWiderType(LInner, RInner);
        if (!WiderTy)
            throw runtime_error("No suitable promotion found");

        if (!LInner->isSameTypeAs(WiderTy)) {
            auto VecTy = TypeReg->getVectorType(WiderTy);
            Dot->setLHS(wrapWithCastTo(Dot->getLHS(), VecTy));
        }

        if (!RInner->isSameTypeAs(WiderTy)) {
            auto VecTy = TypeReg->getVectorType(WiderTy);
            Dot->setRHS(wrapWithCastTo(Dot->getRHS(), VecTy));
        }

        annotateWithConst(Dot, WiderTy);
        return WiderTy;

    } else if (isa<MatrixTy>(LHS) && isa<MatrixTy>(RHS)) {

        // Matrix multiplication

        auto LHSMatTy = dyn_cast<MatrixTy>(LHS);
        auto RHSMatTy = dyn_cast<MatrixTy>(RHS);

        auto LInner = TypeRegistry::getInnerTyFromComposite(LHSMatTy);
        auto RInner = TypeRegistry::getInnerTyFromComposite(RHSMatTy);

        auto WiderTy = getWiderType(LInner, RInner);
        if (!WiderTy)
            throw runtime_error("No suitable promotion found");

        if (!LInner->isSameTypeAs(WiderTy)) {
            auto MatTy = TypeReg->getMatrixType(WiderTy);
            Dot->setLHS(wrapWithCastTo(Dot->getLHS(), MatTy));
        }

        if (!RInner->isSameTypeAs(WiderTy)) {
            auto MatTy = TypeReg->getMatrixType(WiderTy);
            Dot->setRHS(wrapWithCastTo(Dot->getRHS(), MatTy));
        }

        auto RetTy = TypeReg->getMatrixType(WiderTy);
        annotateWithConst(Dot, RetTy);
        return RetTy;
    }
    throw std::runtime_error("Dot product/matrix multiplication can only be applied to vector/matrix types");

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

    auto IsString = (LVecTy->isString() || RVecTy->isString());

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
        auto ResTy = TypeReg->getVectorType(LInner, -1, true, IsString);
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
    auto ResTy = TypeReg->getVectorType(WiderTy, ResLen, true, IsString);
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

    throw runtime_error("No suitable promotion found!");
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

    if (isa<IntervalTy>(ExprTy)) {
        Out->setOutStreamExpr(wrapWithCastTo(
                Out->getOutStreamExpr(),
                TypeReg->getVectorType(TypeReg->getIntegerTy())));
        return nullptr;
    }

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

    if (isa<IntervalTy>(DomainType)) {
        auto VecTy = TypeReg->getVectorType(TypeReg->getIntegerTy());
        Gen->setDomain(wrapWithCastTo(Gen->getDomain(), VecTy));
    }
    auto ResTy = TypeReg->getVectorType(InnerType);
    PM->setAnnotation<ExprTypeAnnotatorPass>(Gen, ResTy);
    return ResTy;
}


Type *ExprTypeAnnotatorPass::visitMatrixGenerator(MatrixGenerator *Gen) {
    visit(Gen->getRowDomainVar());
    visit(Gen->getColumnDomainVar());

    auto RowType = visit(Gen->getRowDomain());

    if (isa<IntervalTy>(RowType)) {
        auto VecTy = TypeReg->getVectorType(TypeReg->getIntegerTy());
        Gen->setRowDomain(wrapWithCastTo(Gen->getRowDomain(), VecTy));
        RowType = VecTy;
    }

    if (!isa<VectorTy>(RowType))
        throw runtime_error("The row must be a vector");

    auto ColumnType = visit(Gen->getColumnDomain());

    if (isa<IntervalTy>(ColumnType)) {
        auto VecTy = TypeReg->getVectorType(TypeReg->getIntegerTy());
        Gen->setColumnDomain(wrapWithCastTo(Gen->getColumnDomain(), VecTy));
        ColumnType = VecTy;
    }

    if (!isa<VectorTy>(ColumnType))
        throw runtime_error("The row must be a vector");

    auto ExprTy = visit(Gen->getExpr());
    auto ResTy = TypeReg->getMatrixType(ExprTy);
    PM->setAnnotation<ExprTypeAnnotatorPass>(Gen, ResTy);
    return ResTy;
}


Type *ExprTypeAnnotatorPass::visitFilter(Filter *Filter) {
    visit(Filter->getDomainVar());
    auto DomainType = visit(Filter->getDomain());


    auto DomainVarType = [&]() {
        if (isa<IntervalTy>(DomainType)) {
            auto VecTy = TypeReg->getVectorType(TypeReg->getIntegerTy());
            Filter->setDomain(wrapWithCastTo(Filter->getDomain(), VecTy));
            return TypeReg->getIntegerTy();
        }
        if (!DomainType->isCompositeTy())
            throw runtime_error("Cannot use a non composite type as a filter domain.");
        return TypeRegistry::getInnerTyFromComposite(DomainType);
    }();

    map<string, int> Temp{};

    vector<Type *> ChildTypes;
    for (auto ChildExpr: *Filter->getPredicatedList()) {
        visit(ChildExpr);
        auto ElementTy = PM->TypeReg.getVectorType(DomainVarType);
        ChildTypes.emplace_back(ElementTy);
    }
    // There's an additional vector
    ChildTypes.emplace_back(PM->TypeReg.getVectorType(DomainVarType));

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

Type *ExprTypeAnnotatorPass::visitDomainLoop(DomainLoop *Loop) {
    auto DomainTy = visit(Loop->getDomain());
    if (isa<IntervalTy>(DomainTy)) {
        auto VecTy = TypeReg->getVectorType(TypeReg->getIntegerTy());
        Loop->setDomain(wrapWithCastTo(
                Loop->getDomain(),VecTy));
        DomainTy = VecTy;
    }

    if (!isa<VectorTy>(DomainTy))
        throw runtime_error("The domain must be a vector or promotable to vector");

    auto InnerTy = TypeRegistry::getInnerTyFromComposite(DomainTy);

    Loop->getID()->setIdentType(InnerTy);
    visit(Loop->getID());
    visit(Loop->getBody());
}
