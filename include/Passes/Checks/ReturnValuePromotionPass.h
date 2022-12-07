//
// Created by dhanrajbir on 16/11/22.
//

#ifndef GAZPREABASE_RETURNVALUEPROMOTIONPASS_H
#define GAZPREABASE_RETURNVALUEPROMOTIONPASS_H

#include "Passes/VisitorPass.h"

#include "Passes/PassManager.h"

struct ReturnValuePromotionPass : VisitorPass<ReturnValuePromotionPass, void> {
    void visitProcedureDef(ProcedureDef *Def);
    void visitFunctionDef(FunctionDef *Def);
    void visitReturn(Return *Ret);

    TypeCast *wrapWithCastTo(ASTNodeT *Expr, Type *Ty) const;

    void runOnAST(ASTPassManager &M, ASTNodeT *Root) {
        PM = &M;
        visit(Root);
    }

    ASTPassManager *PM;
    Type *ReturnTy;
};

#endif //GAZPREABASE_RETURNVALUEPROMOTIONPASS_H
