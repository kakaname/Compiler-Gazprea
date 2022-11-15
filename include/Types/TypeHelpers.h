//
// Created by dhanrajbir on 14/11/22.
//

#ifndef GAZPREABASE_TYPEHELPERS_H
#define GAZPREABASE_TYPEHELPERS_H

#include "CompositeTypes.h"

using llvm::dyn_cast;

bool isValidTupleCast(const Type *BaseTy, const Type *TargetTy) {
    auto BaseTuple = cast<TupleTy>(BaseTy);
    auto TargetTuple = dyn_cast<TupleTy>(TargetTy);

    if (!TargetTy)
        return false;
    if (BaseTuple->getNumOfMembers() != TargetTuple->getNumOfMembers())
        return false;

    auto IsValid = true;
    for (int I = 0; I < BaseTuple->getNumOfMembers(); I++)
        IsValid = IsValid && BaseTuple->getMemberTypeAt(I)->canCastTo(
                TargetTuple->getMemberTypeAt(I));
    return IsValid;
}

#endif //GAZPREABASE_TYPEHELPERS_H
