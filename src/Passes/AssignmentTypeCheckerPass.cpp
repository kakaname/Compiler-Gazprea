//
// Created by dhanrajbir on 15/11/22.
//

#include "Passes/AssignmentTypeCheckerPass.h"
#include "Passes/ExprTypeAnnotatorPass.h"

void AssignmentTypeCheckerPass::visitAssignment(Assignment *Assign) {
    auto IdentType = Assign->getIdentifier()->getIdentType();
    assert(IdentType && "Identifier must have their types assigned at this point.");
    auto AssignedType = PM->getAnnotation<ExprTypeAnnotatorPass>(Assign->getExpr());

    // If they are already the same type, we don't care.
    if (AssignedType->isSameTypeAs(IdentType))
        return;

    // If not, the expression must be promotable to the ident type.
    assert(AssignedType->canPromoteTo(IdentType) && "Invalid assignment,"
                                                    "types don't match");
    auto Cast =  PM->Builder.build<TypeCast>();
    Cast->setTargetType(IdentType);
    Cast->setExpr(Assign->getExpr());
    Assign->setExpr(Cast);
    PM->invalidateResult<ExprTypeAnnotatorPass>();
}

void AssignmentTypeCheckerPass::visitDeclaration(Declaration *Decl) {
    auto IdentType = Decl->getIdentifier()->getIdentType();
    assert(IdentType && "Identifier must have their types assigned at this point.");
    auto AssignedType = PM->getAnnotation<ExprTypeAnnotatorPass>(Decl->getInitExpr());

    // If they are already the same type, we don't care.
    if (AssignedType->isSameTypeAs(IdentType))
        return;

    // If not, the expression must be promotable to the ident type.
    assert(AssignedType->canPromoteTo(IdentType) && "Invalid assignment,"
                                                    "types don't match");
    auto Cast =  PM->Builder.build<TypeCast>();
    Cast->setTargetType(IdentType);
    Cast->setExpr(Decl->getInitExpr());
    Decl->setInitExpr(Cast);
    PM->invalidateResult<ExprTypeAnnotatorPass>();
}
