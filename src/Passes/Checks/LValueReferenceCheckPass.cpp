//
// Created by dhanrajbir on 18/11/22.
//

#include "Passes/Checks/LValueReferenceCheckPass.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"

void LValueReferenceCheckPass::visitMemberReference(MemberReference *Ref) {
    auto Ty = PM->getAnnotation<ExprTypeAnnotatorPass>(Ref);
    if (Ty->isConst())
        throw std::runtime_error("Can't take reference of an r-value");
}

void LValueReferenceCheckPass::visitIdentReference(IdentReference *Ref) {
    auto Ty = Ref->getIdentifier()->getIdentType();
    assert(Ty && "Should have types assigned");
    if (Ty->isConst())
        throw std::runtime_error("Can't take reference of an r-value");
}
