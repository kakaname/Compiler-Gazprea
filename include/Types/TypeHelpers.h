//
// Created by dhanrajbir on 14/11/22.
//

#ifndef GAZPREABASE_TYPEHELPERS_H
#define GAZPREABASE_TYPEHELPERS_H

#include <algorithm>

#include "CompositeTypes.h"
#include "ScalarTypes.h"
#include "TypeRegistry.h"

using llvm::dyn_cast;
using llvm::isa;


bool isValidTupleCast(const Type *BaseType, const Type *TargetTy) {
    auto BaseTy = cast<TupleTy>(BaseType);
    auto TargetTuple = dyn_cast<TupleTy>(TargetTy);

    if (!TargetTy)
        return false;
    if (BaseTy->getNumOfMembers() != TargetTuple->getNumOfMembers())
        return false;

    for (int I = 0; I < BaseTy->getNumOfMembers(); I++)
        if (!BaseTy->getMemberTypeAt(I)->canCastTo(
                TargetTuple->getMemberTypeAt(I)))
            return false;
    return true;
}

bool isSameTupleTypeAs(const Type* BaseType, const Type *TargetTy) {
    auto BaseTy = cast<TupleTy>(BaseType);
    auto TargetTuple = dyn_cast<TupleTy>(TargetTy);

    if (!TargetTuple)
        return false;

    if (BaseTy->getNumOfMembers() != TargetTuple->getNumOfMembers())
        return false;

    for (int I = 0; I < BaseTy->getNumOfMembers(); I++)
        if (!BaseTy->getMemberTypeAt(I)->isSameTypeAs(
                TargetTuple->getMemberTypeAt(I)))
            return false;
    return true;
}

bool isSameVectorAs(const Type* BaseType, const Type *TargetTy) {
    auto BaseTy = cast<VectorTy>(BaseType);
    auto TargetVecTy = dyn_cast<VectorTy>(TargetTy);

    if (!TargetVecTy)
        return false;

    // The inner type must be the same.
    if (!BaseTy->getInnerTy()->isSameTypeAs(TargetVecTy->getInnerTy()))
        return false;

    // If any of the sizes are not known, we assume they are the same type.
    // In this case the program will fail at runtime is this is not the case.
    if (!BaseTy->isSizeKnown() || !TargetVecTy->isSizeKnown())
        return true;

    // Sizes of both vectors are known. If they are different, the types are
    // different.
    return BaseTy->getSize() == TargetVecTy->getSize();
}

bool canPromoteTupleTo(const Type *BaseTy, const Type *TargetTy) {
    auto BaseTuple = cast<TupleTy>(BaseTy);
    auto TargetTuple = dyn_cast<TupleTy>(TargetTy);

    if (!TargetTuple)
        return false;

    if (BaseTuple->getNumOfMembers() != TargetTuple->getNumOfMembers())
        return false;

    for (int I = 0; I < BaseTuple->getNumOfMembers(); I++)
        if (!BaseTuple->getMemberTypeAt(I)->canPromoteTo(
                TargetTuple->getMemberTypeAt(I)))
            return false;
    return true;
}

bool canPromoteVectorTo(const Type *BaseTy, const Type *TargetTy) {
    auto BaseVec = cast<VectorTy>(BaseTy);
    auto TargetVec = dyn_cast<VectorTy>(TargetTy);

    if (!TargetVec)
        return false;

    // All scalars can be promoted to vectors whose type is promotable from
    // the base as well
    if (BaseTy->isScalarTy())
        return BaseTy->canPromoteTo(TargetVec->getInnerTy());

    // sizes must be the same for vector promotion, but sizes can differ for
    // vector casting
    if (BaseVec->getSize() != TargetVec->getSize() &&
        BaseVec->getSize() != -1 && TargetVec->getSize() != -1)
        return false;

    return BaseVec->getInnerTy()->canPromoteTo(TargetVec->getInnerTy());
}

const Type *getPromotedScalarType(const Type *BaseTy, const Type *TargetTy) {
    if (BaseTy->isSameTypeAs(TargetTy))
        return BaseTy;
    if (BaseTy->canPromoteTo(TargetTy))
        return TargetTy;
    if (TargetTy->canPromoteTo(BaseTy))
        return BaseTy;
    return nullptr;
}



//const Type *getPromotedVectorInMatrixType(const Type *BaseTy,
//                                          const Type *TargetTy) {
//    auto BaseVec = cast<VectorTy>(BaseTy);
//    auto TargetVec = cast<VectorTy>(TargetTy);
//
//    auto InnerTy = getPromotedScalarType(BaseVec->getInnerTy(),
//                                         TargetVec->getInnerTy());
//    if (!InnerTy)
//        return nullptr;
//
//    uint32_t Size;
//    if (BaseVec->getSize() == -1 || TargetVec->getSize() == -1)
//        Size = -1;
//    else
//        Size = std::max(BaseVec->getSize(), TargetVec->getSize());
//
//    return new VectorTy(InnerTy, Size);
//
//
//}


bool doesTupleSupportEq(const Type *Tup) {
    auto Members = cast<TupleTy>(Tup)->getMemberTypes();
    auto Pred = [&](const Type* T) { return T->isValidForEq();};
    return std::all_of(Members.begin(), Members.end(), Pred);
}

bool doesMatrixSupportEq(const Type *Mat) {
    return cast<MatrixTy>(Mat)->getInnerTy()->isValidForEq();
}

bool doesMatrixSupportArithOps(const Type *Mat) {
    return cast<MatrixTy>(Mat)->getInnerTy()->isValidForArithOps();
}

bool doesMatrixSupportComparisonOps(const Type *Mat) {
    return cast<MatrixTy>(Mat)->getInnerTy()->isValidForComparisonOp();
}

bool doesVectorSupportEq(const Type *Vec) {
    return cast<VectorTy>(Vec)->getInnerTy()->isValidForEq();
}

bool doesVectorSupportArithOps(const Type *Vec) {
    return cast<VectorTy>(Vec)->getInnerTy()->isValidForArithOps();
}

bool isVectorValidForComparisonOps(const Type *Vec) {
    return cast<VectorTy>(Vec)->getInnerTy()->isValidForComparisonOp();
}

std::string getTupleTypeName(const Type *Ty) {
    auto TupleType = cast<TupleTy>(Ty);
    std::string TypeName = "tuple(";
    size_t NumOfMembers = TupleType->getNumOfMembers();
    for (size_t I = 0; I < NumOfMembers; I++) {
        if (I > 0)
            TypeName += ", ";
        TypeName += TupleType->getMemberTypeAt(I)->getTypeName();
    }
    TypeName += ")";
    return TypeName;
}

std::string getVectorTypeName(const Type *Ty) {
    auto VectorType = cast<VectorTy>(Ty);
    std::string TypeName = "vector(";
    TypeName += VectorType->getInnerTy()->getTypeName();
    TypeName += "[";
    int NumOfElements = VectorType->getSize();
    if (NumOfElements < 0)
        TypeName += "*";
    else
        TypeName += std::to_string(NumOfElements);
    TypeName += "])";
    return TypeName;
}

std::string getMatrixTypeName(const Type *Ty) {
    auto MatrixType = cast<MatrixTy>(Ty);
    std::string TypeName = "matrix(";
    TypeName += MatrixType->getInnerTy()->getTypeName();
    TypeName += "[";
    int NumOfRows = MatrixType->getNumOfRows();
    int NumOfCols = MatrixType->getNumOfColumns();
    if (NumOfRows < 0)
        TypeName += "*";
    else
        TypeName += std::to_string(NumOfRows);
    TypeName += ", ";
    if (NumOfCols < 0)
        TypeName += "*";
    else
        TypeName += std::to_string(NumOfCols);
    TypeName += "])";
    return TypeName;
}

string getFunctionTypeName(const Type *Ty) {
    auto FuncTy = cast<FunctionTy>(Ty);
    std::string TypeName = "function(";
    size_t NumOfMembers = FuncTy->getNumOfArgs();
    for (size_t I = 0; I < NumOfMembers; I++) {
        if (I > 0)
            TypeName += ", ";
        TypeName += FuncTy->getParamTypeAt(I)->getTypeName();
    }
    TypeName += ") -> ";
    TypeName += FuncTy->getRetType()->getTypeName();
    return TypeName;
};

string getProcedureTypeName(const Type *Ty) {
    auto ProcTy = cast<ProcedureTy>(Ty);
    std::string TypeName = "procedure(";
    size_t NumOfMembers = ProcTy->getNumOfArgs();
    for (size_t I = 0; I < NumOfMembers; I++) {
        if (I > 0)
            TypeName += ", ";
        TypeName += ProcTy->getParamTypeAt(I)->getTypeName();
    }
    TypeName += ") -> ";
    if (ProcTy->getRetTy())
        TypeName += ProcTy->getRetTy()->getTypeName();
    else
        TypeName += "noreturn";
    return TypeName;
};


bool isSameFuncAs(const Type* Base, const Type* Other) {
    auto FuncTy = cast<FunctionTy>(Base);
    auto OtherFunc = dyn_cast<FunctionTy>(Other);

    if (!OtherFunc)
        return false;
    if (FuncTy->getNumOfArgs() != OtherFunc->getNumOfArgs())
        return false;

    for (auto I = 0; I < FuncTy->getNumOfArgs(); I++)
        if (!FuncTy->getParamTypeAt(I)->isSameTypeAs(
                OtherFunc->getParamTypeAt(I)))
            return false;

    return OtherFunc->getRetType()->isSameTypeAs(FuncTy->getRetType());
};
bool isSameProcAs(const Type *Base, const Type *Other) {
    auto ProcTy = cast<ProcedureTy>(Base);
    auto OtherProc = dyn_cast<ProcedureTy>(Other);

    if (!OtherProc)
        return false;

    if (ProcTy->getNumOfArgs() != OtherProc->getNumOfArgs())
        return false;

    for (auto I = 0; I < ProcTy->getNumOfArgs(); I++)
        if (!ProcTy->getParamTypeAt(I)->isSameTypeAs(
                OtherProc->getParamTypeAt(I)))
            return false;

    if (!ProcTy->getRetTy())
        return !OtherProc->getRetTy();

    if (!OtherProc->getRetTy())
        return false;

    return ProcTy->getRetTy()->isSameTypeAs(OtherProc->getRetTy());
}

bool canPromoteIntegerTo(const Type *TargetTy) {
    if (TargetTy->isCompositeTy())
        return canPromoteIntegerTo(TypeRegistry::getInnerTyFromComposite(
                TargetTy));
    return isa<RealTy>(TargetTy) || isa<IntegerTy>(TargetTy);
}

bool canPromoteRealTo(const Type* TargetTy) {
    if (TargetTy->isCompositeTy())
        return canPromoteRealTo(TypeRegistry::getInnerTyFromComposite(
                TargetTy));
    return isa<RealTy>(TargetTy);
}

bool canPromoteVectorTo(const Type* BaseTy, const Type* TargetTy) {
    auto BaseVec = cast<VectorTy>(BaseTy);
    auto TargetVec = dyn_cast<VectorTy>(TargetTy);

    if (!TargetVec)
        return false;

    auto CanInnerPromote = BaseVec->getInnerTy()->canPromoteTo(TargetVec->getInnerTy());

    if (TargetVec->isSizeKnown() && BaseVec->isSizeKnown())
        return (TargetVec->getSize() == BaseVec->getSize()) && CanInnerPromote;

    return CanInnerPromote;
}


bool canCastBoolCharIntTo(const Type* TargetTy) {
    switch (TargetTy->getKind()) {
        case Type::T_Bool:
        case Type::T_Char:
        case Type::T_Int:
        case Type::T_Real:
            return true;
        default:
            if (TargetTy->isCompositeTy())
                return canCastBoolCharIntTo(TypeRegistry::getInnerTyFromComposite(TargetTy));
            return false;
    }
}

bool canCastRealTo(const Type* TargetTy) {
    if (isa<RealTy>(TargetTy) || isa<IntegerTy>(TargetTy))
        return true;

    if (TargetTy->isCompositeTy())
        return canCastRealTo(TypeRegistry::getInnerTyFromComposite(TargetTy));

    return false;
}

bool isVectorValidForUnaryNot(const Type* T) {
    return cast<VectorTy>(T)->getInnerTy()->isValidForUnaryNot();
};
bool isMatrixValidForUnaryNot(const Type* T) {
    return cast<MatrixTy>(T)->getInnerTy()->isValidForUnaryNot();
}

bool isVectorValidForUnaryAddSub(const Type *T) {
    return cast<VectorTy>(T)->getInnerTy()->isValidForUnaryAddOrSub();
}
bool isMatrixValidForUnaryAddSub(const Type *T) {
    return cast<MatrixTy>(T)->getInnerTy()->isValidForUnaryAddOrSub();
}

bool canCastVectorTo(const Type *Base, const Type *TargetTy) {
    auto BaseVec = cast<VectorTy>(Base);
    auto VecTy = dyn_cast<VectorTy>(TargetTy);
    if (!VecTy)
        return false;

    return (BaseVec->getInnerTy()->canCastTo(VecTy->getInnerTy()));
}

#endif //GAZPREABASE_TYPEHELPERS_H
