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

    Generator *getIteratingGenerator(ASTNodeT *Domain);

    Identifier *getAnonymousIdent(const Type*);

    IntLiteral *getIntLiteralWithVal(long Val);

    TypeCast *wrapWithCastTo(ASTNodeT *Expr, const Type *Target) const;

    ASTPassManager *PM;

    void runOnAST(ASTPassManager &Manager, ASTNodeT *Root) {
        PM = &Manager;
        visit(Root);
    }
};

#endif //GAZPREABASE_SIMPLIFYCOMPOSITETYPECASTING_H
