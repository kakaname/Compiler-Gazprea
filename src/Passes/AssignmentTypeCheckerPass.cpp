//
// Created by dhanrajbir on 15/11/22.
//

#include "Passes/AssignmentTypeCheckerPass.h"
#include "Passes/ExprTypeAnnotatorPass.h"

void AssignmentTypeCheckerPass::visitAssignment(Assignment *Assign) {
    auto IdentType = Assign->getIdentifier()->getIdentType();
    assert(IdentType && "Identifier must have their types assigned at this point.");
    auto AssignedType = PM->getAnnotation<ExprTypeAnnotatorPass>(Assign->getExpr());
    assert((AssignedType->isSameTypeAs(IdentType)
    || AssignedType->canPromoteTo(IdentType)) && "Incompatible type assigned");

    if (AssignedType->isSameTypeAs(IdentType))
        return;

    auto Cast =  PM->Builder.build<ExplicitCast>();
    Cast->setTargetType(IdentType);
    Cast->setExpr(Assign->getExpr());
    Assign->setExpr(Cast);
    PM->invalidateResult<ExprTypeAnnotatorPass>();
}

void AssignmentTypeCheckerPass::visitDeclaration(Declaration *Decl) {
    auto IdentType = Decl->getIdentifier()->getIdentType();
    assert(IdentType && "Identifier must have their types assigned at this point.");
    auto AssignedType = PM->getAnnotation<ExprTypeAnnotatorPass>(Decl->getInitExpr());
    assert((AssignedType->isSameTypeAs(IdentType)
           || AssignedType->canPromoteTo(IdentType)) && "Incompatible type assigned");

    if (AssignedType->isSameTypeAs(IdentType))
        return;

    auto Cast =  PM->Builder.build<ExplicitCast>();
    Cast->setTargetType(IdentType);
    Cast->setExpr(Decl->getInitExpr());
    Decl->setInitExpr(Cast);
    PM->invalidateResult<ExprTypeAnnotatorPass>();
}
