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

        if (LeftType->getKind() == Type::TypeKind::T_Vector) {
            auto LeftVec = dyn_cast<VectorTy>(LeftType);
            auto ResType = PM->TypeReg.getVectorType(LeftVec->getInnerTy(), LeftVec->getSize());
            annotate(Op, ResType);
            return ResType;
        }

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
            } else if (ChildType->getKind() == Type::TypeKind::T_Matrix && dyn_cast<MatrixTy>(ChildType)->getInnerTy()->isValidForUnaryNot()) {
                auto NewMatTy = PM->TypeReg.getMatrixType(PM->TypeReg.getBooleanTy(), dyn_cast<MatrixTy>(ChildType)->getNumOfRows(), dyn_cast<MatrixTy>(ChildType)->getNumOfColumns());
                annotate(Op, NewMatTy);
                return NewMatTy;
            }
            throw InvalidUnaryNotError(Op, ChildType->getTypeName());
        }
        annotate(Op, PM->TypeReg.getBooleanTy());
        return PM->TypeReg.getBooleanTy();
    }

    if (!ChildType->isValidForUnaryAddOrSub() && (ChildType->getKind() != Type::TypeKind::T_Vector && ChildType->getKind() != Type::TypeKind::T_Matrix)) {
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
    const Type *VecTy = nullptr;
    // Pass 1: Check if all elements are of the same or promotable type (get the highest type)
    for (auto *ChildExpr : *VecLit) {
        const Type *ChildTy = visit(ChildExpr);
        bool IsMatrix = ChildTy->getKind() == Type::TypeKind::T_Vector;

        // If a vector has an inner type of vector of scalars, then it is a matrix
        if (IsMatrix) {

            auto InVecTy = dyn_cast<VectorTy>(ChildTy);
            if (!InVecTy->getInnerTy()->isScalarTy()) {
                throw runtime_error("Vector literal has a vector of vectors as an element.");
            }
            if (!VecTy) {
                VecTy = ChildTy;
                continue;
            }

            // If the current type is a scalar and we encounter a matrix, then we can promote it like normal
            if (VecTy->isScalarTy()) {
                VecTy = ChildTy->getPromotedType(VecTy);
                continue;
            }

            // All of this is done here since the type class should not be generating types, only the type registry
            // should be doing that. This is to cover the special case where we have to use the type from one existing
            // vector and the inner type from another vector.
            auto Size = InVecTy->getPromotedVectorSizeForMatrix(dyn_cast<VectorTy>(VecTy));
            auto PromotedTy = InVecTy->getInnerTy()->getPromotedType(dyn_cast<VectorTy>(VecTy)->getInnerTy());
            VecTy = PM->TypeReg.getVectorType(PromotedTy, Size);

        } else if (!ChildTy->isScalarTy()) {
                throw std::runtime_error("Vector literal can only contain scalar types");
        }

        if (!VecTy) {
            VecTy = ChildTy;
            continue;
        }

        // By the nature of Gazprea only supporting int->real scalar promotion, this
        // simplifies this pass of the compiler.
        VecTy = ChildTy->getPromotedType(VecTy);
        if (!VecTy)
            throw std::runtime_error("Vector literal can only contain values of the same or promotable types");
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

    // Get the correct type
    if (VecTy->getKind() == Type::TypeKind::T_Vector) {
        auto InVecTy = dyn_cast<VectorTy>(VecTy);
        VecTy = PM->TypeReg.getMatrixType(InVecTy->getInnerTy(), VecLit->numOfChildren(), InVecTy->getSize());
    } else {
        VecTy = PM->TypeReg.getVectorType(VecTy, VecLit->numOfChildren());
    }
    PM->setAnnotation<ExprTypeAnnotatorPass>(VecLit, VecTy);
    return VecTy;

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

const Type *ExprTypeAnnotatorPass::visitByOp(ByOp *By) {
    auto LHS = visit(By->getLHS());

    if (!LHS->isSameTypeAs(PM->TypeReg.getIntervalTy()) && LHS->getKind() != Type::TypeKind::T_Vector)
        throw std::runtime_error("By operator can only be applied to interval or vector types");

    auto RHS = visit(By->getRHS());

    if (!RHS->isSameTypeAs(PM->TypeReg.getIntegerTy()))
        throw std::runtime_error("By operator can only be applied to integer types");

    // TODO set size of vector using future const folding pass
    // right now, it assumes the only expression is an integer literal

    long SizeOfBase = -1;
    long Stride = -1;

    auto RHSExpr = dyn_cast<IntLiteral>(By->getRHS());
    if (RHSExpr) {
        // if the right hand side is an integer literal or has been constant folded
        Stride = RHSExpr->getVal();
        if (LHS->isSameTypeAs(PM->TypeReg.getIntervalTy())) {
            // check if the left hand side contains only purely literal values
            // this is the only case for intervals that we will handle at compile
            // time
            if (isa<Interval>(By->getLHS())
                && isa<IntLiteral>(dyn_cast<Interval>(By->getLHS())->getLowerExpr())
                && isa<IntLiteral>(dyn_cast<Interval>(By->getLHS())->getUpperExpr())) {
                    SizeOfBase = dyn_cast<IntLiteral>(dyn_cast<Interval>(By->getLHS())->getUpperExpr())->getVal()
                        - dyn_cast<IntLiteral>(dyn_cast<Interval>(By->getLHS())->getLowerExpr())->getVal() + 1;
            }
        } else {
            // for vectors, we have more information as to the vector size in the type definition
            SizeOfBase = dyn_cast<VectorTy>(LHS)->getSize();
        }
    }

    const Type *VecType;
    if (SizeOfBase != -1 && Stride != -1) {
        // if we have both the size of the base and the stride, we can calculate the size of the vector
        // and create the vector type
        VecType = PM->TypeReg.getVectorType(PM->TypeReg.getIntegerTy(), (SizeOfBase + Stride - 1 ) / Stride);
    } else {
        // otherwise, we will just create a vector of size 1
        VecType = PM->TypeReg.getVectorType(PM->TypeReg.getIntegerTy(), 1);
    }

    PM->setAnnotation<ExprTypeAnnotatorPass>(By, VecType);
    return VecType;

}

const Type *ExprTypeAnnotatorPass::visitDotProduct(DotProduct *Dot) {
    auto LHS = visit(Dot->getLHS());
    auto RHS = visit(Dot->getRHS());

    if (LHS->getKind() != Type::TypeKind::T_Vector || RHS->getKind() != Type::TypeKind::T_Vector)
        throw std::runtime_error("Dot product can only be applied to vector types");

    // TODO ensure vector isSameTypeAs ignores size of -1
    // TODO casting logic
    if (!LHS->isSameTypeAs(RHS))
        throw std::runtime_error("Dot product can only be applied to vectors of the same type and size");

    auto ResTy = dyn_cast<VectorTy>(LHS)->getInnerTy();

    PM->setAnnotation<ExprTypeAnnotatorPass>(Dot, ResTy);
    return ResTy;
}

const Type *ExprTypeAnnotatorPass::visitConcat(Concat *Concat) {
    auto LHS = visit(Concat->getLHS());
    auto RHS = visit(Concat->getRHS());

    if (LHS->getKind() != Type::TypeKind::T_Vector || RHS->getKind() != Type::TypeKind::T_Vector)
        throw std::runtime_error("Concatenation can only be applied to vector types");

    auto LHSInner = dyn_cast<VectorTy>(LHS)->getInnerTy();
    auto RHSInner = dyn_cast<VectorTy>(RHS)->getInnerTy();

    if (!LHSInner->isSameTypeAs(RHSInner)) {
        // TODO check if the inner types can be promoted to the same type
        throw std::runtime_error("Concatenation can only be applied to vectors of the same type");
    }

    long SizeOfLHS = dyn_cast<VectorTy>(LHS)->getSize();
    long SizeOfRHS = dyn_cast<VectorTy>(RHS)->getSize();
    long SizeOfRes = -1;

    if (SizeOfLHS != -1 && SizeOfRHS != -1) {
        SizeOfRes = SizeOfLHS + SizeOfRHS;
    }


    auto ResTy = PM->TypeReg.getVectorType(dyn_cast<VectorTy>(LHS)->getInnerTy(),
        SizeOfRes);

    PM->setAnnotation<ExprTypeAnnotatorPass>(Concat, ResTy);
    return ResTy;
}

