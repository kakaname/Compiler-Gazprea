//
// Created by dhanrajbir on 04/12/22.
//

#include "Passes/Transformations/SimplifyCompositeTypeCasting.h"
#include "Common/MatchBoolPair.h"
#include "Passes/PassManager.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"
#include "Passes/BuildAST/ASTBuilderPass.h"

using llvm::dyn_cast;

void SimplifyCompositeTypeCasting::visitExplicitCast(ExplicitCast *Cast) {
    visit(Cast->getExpr());
    auto ExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Cast->getExpr());
    auto TargetTy = Cast->getTargetType();
    matchBoolPair(TargetTy->isCompositeTy(), ExprTy->isCompositeTy()) {
        // Don't care when both the types are scalar.
        matchPattern(false, false):
            return;
            // Invalid, can't cast composite type to scalar.
        matchPattern(false, true):
            throw runtime_error("Can't cast composite type to scalar");
            // Casting a scalar to a composite type.
        matchPattern(true, false): {
            if (auto VecTy = dyn_cast<VectorTy>(TargetTy)) {
                auto ExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Cast);

                if (!VecTy->getSizeExpr())
                    throw runtime_error("Invalid Vector Type has no size expr");

                auto Gen = getGenWithUpperBound(VecTy->getSizeExpr());
                if (!VecTy->getInnerTy()->isSameTypeAs(ExprTy))
                    Cast->setExpr(wrapWithCastTo(Cast->getExpr(), VecTy->getInnerTy()));
                Gen->setExpr(Cast->getExpr());
                Cast->getParent()->replaceChildWith(Cast, Gen);
                return;
            }
            // TODO: Add matrices here.
        }
            // Both the target and the inner expr are vectors.
        matchPattern(true, true): {
            if (auto VecTy = dyn_cast<VectorTy>(TargetTy)) {
                // If no expression was given, then the size of the resultant
                // vector must be the same as the size of the inner expression.
                auto TargetInnerCast = TypeRegistry::getInnerTyFromComposite(
                        TargetTy);
                auto Gen = getGenWithUpperBound(VecTy->getSizeExpr());
                auto Idx = buildIndexExpr(Cast->getExpr(), Gen->getExpr());
                Gen->setExpr(wrapWithCastTo(Idx, TargetInnerCast));
                Cast->getParent()->replaceChildWith(Cast, Gen);
                return;
            }
            // TODO: Add matrices here.
        }
    }
}

TypeCast *SimplifyCompositeTypeCasting::wrapWithCastTo(ASTNodeT *Expr, Type *TargetType) const {
    auto Cast = PM->Builder.build<TypeCast>();
    Cast->copyCtx(Expr);
    Cast->setExpr(Expr);
    Cast->setTargetType(TargetType);
    PM->setAnnotation<ExprTypeAnnotatorPass>(Cast, TargetType);
    return Cast;
}

IntLiteral *SimplifyCompositeTypeCasting::getIntLiteralWithVal(long Val) const {
    auto Lit = PM->Builder.build<IntLiteral>();
    Lit->setIntVal(Val);
    return Lit;
}

Generator *SimplifyCompositeTypeCasting::getGenWithUpperBound(ASTNodeT *Bound) {
    auto Gen = PM->Builder.build<Generator>();
    auto DomainVar = getAnonymousIdent(PM->TypeReg.getIntegerTy());
    auto Range = PM->Builder.build<Interval>();
    Range->setLowerExpr(getIntLiteralWithVal(0));
    Range->setUpperExpr(Bound);
    Gen->setDomain(Range);
    Gen->setDomainVariable(DomainVar);
    auto Expr = getAnonymousIdent(PM->TypeReg.getIntegerTy());
    Expr->setReferred(DomainVar->getReferred());
    Gen->setExpr(Expr);
    return Gen;
}

Identifier *SimplifyCompositeTypeCasting::getAnonymousIdent(Type*Ty) {
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setIdentType(Ty);
    Ident->setReferred(PM->SymTable.defineObject("", Ty));
    return Ident;
}

Generator *SimplifyCompositeTypeCasting::getIteratingGenerator(ASTNodeT *Bound, Type* DomainVarTy) {
    auto Gen = PM->Builder.build<Generator>();
    auto DomainVar = getAnonymousIdent(DomainVarTy);
    Gen->setDomainVariable(DomainVar);
    Gen->setDomain(Bound);
    auto Expr = PM->Builder.build<Identifier>();
    Expr->setReferred(DomainVar->getReferred());
    Expr->setIdentType(DomainVar->getIdentType());
    Gen->setExpr(Expr);
    return Gen;
}

void SimplifyCompositeTypeCasting::visitTypeCast(TypeCast *Cast) {
    visit(Cast->getExpr());
    auto ExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Cast->getExpr());
    auto TargetTy = Cast->getTargetType();
    matchBoolPair(TargetTy->isCompositeTy(), ExprTy->isCompositeTy()) {
        // Don't care when both the types are scalar.
        matchPattern(false, false):
            return;
            // Invalid, can't cast composite type to scalar.
        matchPattern(false, true):
            throw runtime_error("Can't cast composite type to scalar");
            // Casting a scalar to a composite type.
        matchPattern(true, false): {
            if (auto VecTy = dyn_cast<VectorTy>(TargetTy)) {
                auto ExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Cast);

                if (!VecTy->getSizeExpr())
                    throw runtime_error("Invalid Vector Type has no size expr");

                auto Gen = getGenWithUpperBound(VecTy->getSizeExpr());
                if (!VecTy->getInnerTy()->isSameTypeAs(ExprTy))
                    Cast->setExpr(wrapWithCastTo(Cast->getExpr(), VecTy->getInnerTy()));
                Gen->setExpr(Cast->getExpr());
                Cast->getParent()->replaceChildWith(Cast, Gen);
                return;
            }
            // TODO: Add matrices here.
        }
            // Both the target and the inner expr are vectors.
        matchPattern(true, true): {
            if (auto VecTy = dyn_cast<VectorTy>(TargetTy)) {
                // If no expression was given, then the size of the resultant
                // vector must be the same as the size of the inner expression.
                auto TargetInnerCast = TypeRegistry::getInnerTyFromComposite(
                        TargetTy);
                auto Gen = getGenWithUpperBound(VecTy->getSizeExpr());
                auto Idx = buildIndexExpr(Cast->getExpr(), Gen->getExpr());
                Gen->setExpr(wrapWithCastTo(Idx, TargetInnerCast));
                Cast->getParent()->replaceChildWith(Cast, Gen);
                return;
            }
            // TODO: Add matrices here.
        }
    }
}

Index *SimplifyCompositeTypeCasting::buildIndexExpr(ASTNodeT *Base, ASTNodeT *IdxExpr) {
    auto Idx = PM->Builder.build<Index>();
    Idx->setBaseExpr(Base);
    Idx->setIndexExpr(IdxExpr);
    return Idx;
}

Interval *SimplifyCompositeTypeCasting::getIntervalWithUpperBound(ASTNodeT *Bound) {
    auto Range = PM->Builder.build<Interval>();
    Range->setLowerExpr(getIntLiteralWithVal(0));
    Range->setUpperExpr(Bound);
    return Range;
}
