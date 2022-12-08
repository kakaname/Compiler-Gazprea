//
// Created by dhanrajbir on 15/11/22.
//

#ifndef GAZPREABASE_CALLABLEARGUMENTTYPECHECKINGPASS_H
#define GAZPREABASE_CALLABLEARGUMENTTYPECHECKINGPASS_H

#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"

struct CallableArgumentTypeCheckingPass : VisitorPass<CallableArgumentTypeCheckingPass, void> {
    void visitFunctionCall(FunctionCall *Call);
    void visitProcedureCall(ProcedureCall *Call);


    TypeCast *wrapWithCastTo(ASTNodeT *Expr, Type *Ty) const;

    void runOnAST(ASTPassManager &M, ASTNodeT *Root) {
        PM = &M;
        visit(Root);
    }

    ASTPassManager *PM;
};

#endif //GAZPREABASE_CALLABLEARGUMENTTYPECHECKINGPASS_H
