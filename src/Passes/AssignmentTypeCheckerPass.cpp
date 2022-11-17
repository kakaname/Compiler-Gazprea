//
// Created by dhanrajbir on 15/11/22.
//

#include "Passes/AssignmentTypeCheckerPass.h"
#include "Passes/ExprTypeAnnotatorPass.h"

void AssignmentTypeCheckerPass::visitAssignment(Assignment *Assign) {
    auto IdentType = Assign->getIdentifier()->getIdentType();
    assert(IdentType && "Identifier must have their types assigned at this point.");
    auto AssignedType = PM->getAnnotation<ExprTypeAnnotatorPass>(Assign->getExpr());
    assert(!IdentType->isConst() && "Assigning to a const type?");

    // If they are already the same type, we don't care.
    if (AssignedType->isSameTypeAs(IdentType))
        return;

    // If not, the expression must be promotable to the ident type.
    assert(AssignedType->canPromoteTo(IdentType) && "Invalid assignment,"
                                                    "types don't match");
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
    assert(AssignedType->canPromoteTo(IdentType) && "Invalid assignment,"
                                                    "types don't match");
    auto Cast = wrapWithCastTo(Decl->getInitExpr(), IdentType);
    Decl->setInitExpr(Cast);
}

void AssignmentTypeCheckerPass::visitMemberAssignment(MemberAssignment *Assign) {
    auto AssigneeTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Assign->getMemberAccess());
    assert(!AssigneeTy->isConst() && "Assigning to a const types?");
    auto AssignedType = PM->getAnnotation<ExprTypeAnnotatorPass>(Assign->getExpr());

    // If they are already the same type, we don't care.
    if (AssignedType->isSameTypeAs(AssigneeTy))
        return;

    // If not, the expression must be promotable to the ident type.
    assert(AssignedType->canPromoteTo(AssigneeTy) && "Invalid assignment,"
                                                    "types don't match");
    auto Cast = wrapWithCastTo(Assign->getExpr(), AssigneeTy);
    Assign->setExpr(Cast);
}

TypeCast *AssignmentTypeCheckerPass::wrapWithCastTo(ASTNodeT *Expr, const Type *Target) const {
    auto Cast = PM->Builder.build<TypeCast>();
    Cast->setExpr(Expr);
    Cast->setTargetType(Target);
    PM->setAnnotation<ExprTypeAnnotatorPass>(Cast, Target);
    return Cast;
}
