//
// Created by dhanrajbir on 16/11/22.
//

#ifndef GAZPREABASE_SIMPLIFYTUPLECASTING_H
#define GAZPREABASE_SIMPLIFYTUPLECASTING_H

#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"

struct SimplifyTupleCasting : VisitorPass<SimplifyTupleCasting, void> {
    void visitTypeCast(TypeCast *Cast);

    void visitExplicitCast(ExplicitCast *Cast);

    MemberAccess *buildMemberAccess(ASTNodeT *BaseExpr, int Idx) const;
    TypeCast *wrapWithCastTo(ASTNodeT *Expr, Type *Target) const;

    void runOnAST(ASTPassManager &P, ASTNodeT *Root) {
        PM = &P;
        visit(Root);
    }

    ASTPassManager *PM;
};

#endif //GAZPREABASE_SIMPLIFYTUPLECASTING_H
