//
// Created by dhanrajbir on 14/11/22.
//

#ifndef GAZPREABASE_TYPEHELPERS_H
#define GAZPREABASE_TYPEHELPERS_H

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

#endif //GAZPREABASE_TYPEHELPERS_H
