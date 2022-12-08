//
// Created by dhanrajbir on 15/11/22.
//

#include "Passes/Checks/AssignmentTypeCheckerPass.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"

void AssignmentTypeCheckerPass::visitAssignment(Assignment *Assign) {
    auto IdentTy = PM->getAnnotation<ExprTypeAnnotatorPass>(
            Assign->getAssignedTo());
    auto AssignedTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Assign->getExpr());
    if (IdentTy->isConst())
        throw ConstantAssignmentError(Assign, IdentTy->getTypeName());

    // If they are already the same type, we don't care.
    if (AssignedTy->isSameTypeAs(IdentTy))
        return;

    matchBoolPair(IdentTy->isCompositeTy(), AssignedTy->isCompositeTy()) {
        // In the scalar case we just check if the scalar can be promoted to
        // the assigned to type.
        matchPattern(false, false): {
            if (!AssignedTy->canPromoteTo(IdentTy))
                throw ScalarPromotionError(Assign, AssignedTy->getTypeName(),
                                           IdentTy->getTypeName());
            auto Cast = wrapWithCastTo(Assign->getExpr(), IdentTy);
            Assign->setExpr(Cast);
            return;
        }

            // A composite type cannot be assigned to a scalar type, so we throw
            // an error.
        matchPattern(false, true): {
            throw ScalarPromotionError(Assign, AssignedTy->getTypeName(),
                                       IdentTy->getTypeName());
        }

            // If both of them are composite types, we first ensure they are the
            // same composite type and then ensure that the inner type of the
            // assigned expression can be promoted to the inner of the assigned to.
        matchPattern(true, true): {
            if (!((isa<VectorTy>(AssignedTy) && isa<VectorTy>(IdentTy)) ||
                  (isa<MatrixTy>(AssignedTy) && isa<MatrixTy>(IdentTy))))
                throw runtime_error("Operation between incompatible "
                                    "types " + AssignedTy->getTypeName() +
                                    IdentTy->getTypeName());
            auto AssignedInner = TypeRegistry::getInnerTyFromComposite(
                    AssignedTy);
            auto IdentInner = TypeRegistry::getInnerTyFromComposite(
                    IdentTy);
            if (AssignedInner->isSameTypeAs(IdentInner))
                return;
            if (!AssignedInner->canPromoteTo(IdentInner))
                throw ScalarPromotionError(Assign, AssignedTy->getTypeName(),
                                           IdentTy->getTypeName());
            auto TargetTy = PM->TypeReg.getCompositeTyWithInner(AssignedTy, IdentInner);
            Assign->setExpr(wrapWithCastTo(Assign->getExpr(), TargetTy));
            return;
        }

        matchPattern(true, false): {
            auto IdentInner = TypeRegistry::getInnerTyFromComposite(
                    IdentTy);
            if (AssignedTy->isSameTypeAs(IdentInner))
                return;

            if (!AssignedTy->canPromoteTo(IdentInner))
                throw ScalarPromotionError(Assign, AssignedTy->getTypeName(),
                                           IdentTy->getTypeName());
            Assign->setExpr(wrapWithCastTo(Assign->getExpr(), IdentTy));
            return;
        }
    }
}

void AssignmentTypeCheckerPass::visitDeclaration(Declaration *Decl) {
    auto IdentTy = Decl->getIdentifier()->getIdentType();
    assert(IdentTy && "Identifier must have their types assigned at this point.");
    auto AssignedTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Decl->getInitExpr());

    // If they are already the same type, we set the right type so that any
    // size expressions gets copied over.
    if (AssignedTy->isSameTypeAs(IdentTy)) {
        return;
    }

    matchBoolPair(IdentTy->isCompositeTy(), AssignedTy->isCompositeTy()) {
        // In the scalar case we just check if the scalar can be promoted to
        // the assigned to type.
        matchPattern(false, false): {
            if (!AssignedTy->canPromoteTo(IdentTy))
                throw ScalarPromotionError(Decl, AssignedTy->getTypeName(),
                                           IdentTy->getTypeName());
            auto Cast = wrapWithCastTo(Decl->getInitExpr(), IdentTy);
            Decl->setInitExpr(Cast);
            return;
        }

        // A composite type cannot be assigned to a scalar type, so we throw
        // an error.
        matchPattern(false, true): {
            throw ScalarPromotionError(Decl, AssignedTy->getTypeName(),
                                       IdentTy->getTypeName());
        }

        // If both of them are composite types, we first ensure they are the
        // same composite type and then ensure that the inner type of the
        // assigned expression can be promoted to the inner of the assigned to.
        matchPattern(true, true): {
            if (!((isa<VectorTy>(AssignedTy) && isa<VectorTy>(IdentTy)) ||
                  (isa<MatrixTy>(AssignedTy) && isa<MatrixTy>(IdentTy))))
                throw runtime_error("Operation between incompatible "
                                    "types " + AssignedTy->getTypeName() +
                                    IdentTy->getTypeName());
            auto AssignedInner = TypeRegistry::getInnerTyFromComposite(
                    AssignedTy);
            auto IdentInner = TypeRegistry::getInnerTyFromComposite(
                    IdentTy);
            if (AssignedInner->isSameTypeAs(IdentInner))
                return;
            if (!AssignedInner->canPromoteTo(IdentInner))
                throw ScalarPromotionError(Decl, AssignedTy->getTypeName(),
                                           IdentTy->getTypeName());
            auto TargetTy = PM->TypeReg.getCompositeTyWithInner(AssignedTy, IdentInner);
            Decl->setInitExpr(wrapWithCastTo(Decl->getInitExpr(), TargetTy));
            return;
        }

        matchPattern(true, false): {
            auto IdentInner = TypeRegistry::getInnerTyFromComposite(
                    IdentTy);
            if (AssignedTy->isSameTypeAs(IdentInner))
                return;

            if (!AssignedTy->canPromoteTo(IdentInner))
                throw ScalarPromotionError(Decl, AssignedTy->getTypeName(),
                                           IdentTy->getTypeName());
            Decl->setInitExpr(wrapWithCastTo(Decl->getInitExpr(), IdentTy));
            return;
        }
    }
}


TypeCast *AssignmentTypeCheckerPass::wrapWithCastTo(ASTNodeT *Expr, Type *Target) const {
    auto Cast = PM->Builder.build<TypeCast>();
    Cast->copyCtx(Expr);
    Cast->setExpr(Expr);
    Cast->setTargetType(Target);
    PM->setAnnotation<ExprTypeAnnotatorPass>(Cast, Target);
    return Cast;
}
