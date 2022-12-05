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
                // If the size is known, we simply repeat the scalar
                // that many times.
                if (VecTy->isSizeKnown()) {
                    auto Gen = getGenWithUpperBound(
                            getIntLiteralWithVal(VecTy->getSize()));
                    Gen->setExpr(wrapWithCastTo(Cast->getExpr(), VecTy->getInnerTy()));
                    Cast->getParent()->replaceChildWith(Cast, Gen);
                    return;
                }

                // If the size is not known, it must be given as an expression
                auto Dimensions = PM->getResult<ASTBuilderPass>().find(
                        {Cast, 0})->second.first;
                if (!Dimensions)
                    throw runtime_error("Tried to cast to a vector of undefined"
                                        " size");
                auto Gen = getGenWithUpperBound(Dimensions);
                Gen->setExpr(Cast->getExpr());
                Cast->getParent()->replaceChildWith(Cast, Gen);
                return;
            }
            // TODO: Add matrices here.
        }
            // Both the target and the inner expr are vectors.
        matchPattern(true, true): {
            if (auto VecTy = dyn_cast<VectorTy>(TargetTy)) {
                // If the size is known, we simply repeat the scalar
                // that many times.
                if (VecTy->isSizeKnown()) {
                    auto Gen = getGenWithUpperBound(
                            getIntLiteralWithVal(VecTy->getSize()));
                    Gen->setExpr(Cast->getExpr());
                    Cast->getParent()->replaceChildWith(Cast, Gen);
                    return;
                }

                // If the size is not known, check if it is given as an expression.
                auto Dimensions = PM->getResult<ASTBuilderPass>().find(
                        {Cast, 0})->second.first;
                // If the dimensions were given cast to them.
                if (Dimensions) {
                    auto Gen = getGenWithUpperBound(Dimensions);
                    Gen->setExpr(Cast->getExpr());
                    Cast->getParent()->replaceChildWith(Cast, Gen);
                    return;
                }

                // If no expression was given, then the size of the resultant
                // vector must be the same as the size of the inner expression.
                auto TargetInnerCast = TypeRegistry::getInnerTyFromComposite(
                        TargetTy);
                auto Gen = getIteratingGenerator(Cast->getExpr());
                Gen->setExpr(wrapWithCastTo(Gen->getExpr(), TargetInnerCast));
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
    auto Range = PM->Builder.build<Interval>();
    Range->setLowerExpr(getIntLiteralWithVal(0));
    Range->setUpperExpr(Bound);
    Gen->setDomain(Range);
    Gen->setDomainVariable(getAnonymousIdent(PM->TypeReg.getIntegerTy()));
    return Gen;
}

Identifier *SimplifyCompositeTypeCasting::getAnonymousIdent(Type*Ty) {
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setIdentType(Ty);
    Ident->setReferred(PM->SymTable.defineObject("", Ty));
    return Ident;
}

Generator *SimplifyCompositeTypeCasting::getIteratingGenerator(ASTNodeT *Domain) {
    auto DomainTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Domain);
    auto DomainInner = TypeRegistry::getInnerTyFromComposite(DomainTy);

    auto Gen = PM->Builder.build<Generator>();
    auto DomainVar = getAnonymousIdent(DomainInner);
    Gen->setDomainVariable(DomainVar);
    Gen->setDomain(Domain);
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
                // If the size is known, we simply repeat the scalar
                // that many times.
                if (VecTy->isSizeKnown()) {
                    auto Gen = getGenWithUpperBound(
                            getIntLiteralWithVal(VecTy->getSize()));
                    Gen->setExpr(wrapWithCastTo(Cast->getExpr(), VecTy->getInnerTy()));
                    Cast->getParent()->replaceChildWith(Cast, Gen);
                    return;
                }

                // If the size is not known, it must be given as an expression
                auto Res = PM->getResult<ASTBuilderPass>().find({Cast, 0});
                if (Res == PM->getResult<ASTBuilderPass>().end())
                    throw runtime_error("Tried to cast to a vector of undefined"
                                        " size");

                auto Dimensions = Res->second.first;
                if (!Dimensions)
                    throw runtime_error("Tried to cast to a vector of undefined"
                                        " size");
                auto Gen = getGenWithUpperBound(Dimensions);
                Gen->setExpr(Cast->getExpr());
                Cast->getParent()->replaceChildWith(Cast, Gen);
                return;
            }
            // TODO: Add matrices here.
        }
            // Both the target and the inner expr are vectors.
        matchPattern(true, true): {
            if (auto VecTy = dyn_cast<VectorTy>(TargetTy)) {
                // If the size is known, we simply repeat the scalar
                // that many times.
                if (VecTy->isSizeKnown()) {
                    auto Gen = getGenWithUpperBound(
                            getIntLiteralWithVal(VecTy->getSize()));
                    Gen->setExpr(Cast->getExpr());
                    Cast->getParent()->replaceChildWith(Cast, Gen);
                    return;
                }

                // If the size is not known, check if it is given as an expression.
                auto Dimensions = PM->getResult<ASTBuilderPass>().find(
                        {Cast, 0})->second.first;
                // If the dimensions were given cast to them.
                if (Dimensions) {
                    auto Gen = getGenWithUpperBound(Dimensions);
                    Gen->setExpr(Cast->getExpr());
                    Cast->getParent()->replaceChildWith(Cast, Gen);
                    return;
                }

                // If no expression was given, then the size of the resultant
                // vector must be the same as the size of the inner expression.
                auto TargetInnerCast = TypeRegistry::getInnerTyFromComposite(
                        TargetTy);
                auto Gen = getIteratingGenerator(Cast->getExpr());
                Gen->setExpr(wrapWithCastTo(Gen->getExpr(), TargetInnerCast));
                Cast->getParent()->replaceChildWith(Cast, Gen);
                return;
            }
            // TODO: Add matrices here.
        }
    }
}
