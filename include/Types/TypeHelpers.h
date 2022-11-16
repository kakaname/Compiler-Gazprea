//
// Created by dhanrajbir on 14/11/22.
//

#ifndef GAZPREABASE_TYPEHELPERS_H
#define GAZPREABASE_TYPEHELPERS_H

#include <algorithm>

#include "CompositeTypes.h"

using llvm::dyn_cast;

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

    if (!TargetTy)
        return false;

    if (BaseTy->getNumOfMembers() != TargetTuple->getNumOfMembers())
        return false;

    for (int I = 0; I < BaseTy->getNumOfMembers(); I++)
        if (!BaseTy->getMemberTypeAt(I)->isSameTypeAs(
                TargetTuple->getMemberTypeAt(I)))
            return false;
    return true;
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

bool doesTupleSupportEq(const Type *Tup) {
    auto Members = cast<TupleTy>(Tup)->getMemberTypes();
    auto Pred = [&](const Type* T) { return T->isValidForEq();};
    return std::all_of(Members.begin(), Members.end(), Pred);
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

string getFunctionTypeName(const Type *Ty) {
    auto FuncTy = cast<FunctionTy>(Ty);
    std::string TypeName = "function(";
    size_t NumOfMembers = FuncTy->getNumOfArgs();
    for (size_t I = 0; I < NumOfMembers; I++) {
        if (I > 0)
            TypeName += ", ";
        TypeName += FuncTy->getArgTypeAt(I)->getTypeName();
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
        TypeName += ProcTy->getArgTypeAt(I)->getTypeName();
    }
    TypeName += ") -> ";
    if (ProcTy->getRetTy())
        TypeName += ProcTy->getRetTy()->getTypeName();
    else
        TypeName += "noreturn";
    return TypeName;
};

#endif //GAZPREABASE_TYPEHELPERS_H
