//
// Created by dhanrajbir on 15/11/22.
//

#include "Passes/AssignmentTypeCheckerPass.h"
#include "Passes/ExprTypeAnnotatorPass.h"

void AssignmentTypeCheckerPass::visitAssignment(Assignment *Assign) {
    auto IdentType = PM->getAnnotation<ExprTypeAnnotatorPass>(
            Assign->getAssignedTo());
    auto AssignedType = PM->getAnnotation<ExprTypeAnnotatorPass>(Assign->getExpr());
    if (IdentType->isConst())
        throw ConstantAssignmentError(Assign, IdentType->getTypeName());

    // If they are already the same type, we don't care.
    if (AssignedType->isSameTypeAs(IdentType))
        return;

    // If not, the expression must be promotable to the ident type.
    if (!AssignedType->canPromoteTo(IdentType))
        throw ScalarPromotionError(Assign, IdentType->getTypeName(), AssignedType->getTypeName());

    auto Cast = wrapWithCastTo(Assign->getExpr(), IdentType);
    Assign->setExpr(Cast);
}

void AssignmentTypeCheckerPass::visitDeclaration(Declaration *Decl) {
    auto IdentType = Decl->getIdentifier()->getIdentType();
    assert(IdentType && "Identifier must have their types assigned at this point.");
    auto AssignedType = PM->getAnnotation<ExprTypeAnnotatorPass>(Decl->getInitExpr());

    // If they are already the same type, we don't care.
    if (AssignedType->isSameTypeAs(IdentType))
        return;

    // If not, the expression must be promotable to the ident type.
    if (!AssignedType->canPromoteTo(IdentType))
        throw ScalarPromotionError(Decl, AssignedType->getTypeName(), IdentType->getTypeName());

    auto Cast = wrapWithCastTo(Decl->getInitExpr(), IdentType);
    Decl->setInitExpr(Cast);
}


TypeCast *AssignmentTypeCheckerPass::wrapWithCastTo(ASTNodeT *Expr, const Type *Target) const {
    auto Cast = PM->Builder.build<TypeCast>();
    Cast->copyCtx(Expr);
    Cast->setExpr(Expr);
    Cast->setTargetType(Target);
    PM->setAnnotation<ExprTypeAnnotatorPass>(Cast, Target);
    return Cast;
}
