//
// Created by dhanrajbir on 04/12/22.
//

#ifndef GAZPREABASE_SIMPLIFYCOMPOSITETYPECASTING_H
#define GAZPREABASE_SIMPLIFYCOMPOSITETYPECASTING_H

#include "Passes/VisitorPass.h"

struct SimplifyCompositeTypeCasting : VisitorPass<SimplifyCompositeTypeCasting, void> {
    void visitExplicitCast(ExplicitCast *Cast);
    void visitTypeCast(TypeCast *Cast);

    Generator *getGenWithUpperBound(ASTNodeT *Bound);

    Generator *getIteratingGenerator(ASTNodeT *Bound, Type* DomainVarTy);

    Identifier *getAnonymousIdent(Type*);

    IntLiteral *getIntLiteralWithVal(long Val) const;

    TypeCast *wrapWithCastTo(ASTNodeT *Expr, Type *Target) const;

    Index *buildIndexExpr(ASTNodeT *Base, ASTNodeT *IdxExpr);

    Interval *getIntervalWithUpperBound(ASTNodeT *Bound);

    ASTPassManager *PM;

    void runOnAST(ASTPassManager &Manager, ASTNodeT *Root) {
        PM = &Manager;
        visit(Root);
    }
};

#endif //GAZPREABASE_SIMPLIFYCOMPOSITETYPECASTING_H
