//
// Created by dhanrajbir on 15/11/22.
//

#ifndef GAZPREABASE_CALLABLEARGUMENTTYPECHECKINGPASS_H
#define GAZPREABASE_CALLABLEARGUMENTTYPECHECKINGPASS_H

#include "VisitorPass.h"
#include "PassManager.h"

struct CallableArgumentTypeCheckingPass : VisitorPass<CallableArgumentTypeCheckingPass, void> {
    void visitFunctionCall(FunctionCall *Call);
    void visitProcedureCall(ProcedureCall *Call);


    TypeCast *wrapWithCastTo(ASTNodeT *Expr, const Type *Ty) const;

    void runOnAST(ASTPassManager &M, ASTNodeT *Root) {
        PM = &M;
        visit(Root);
    }

    ASTPassManager *PM;
};

#endif //GAZPREABASE_CALLABLEARGUMENTTYPECHECKINGPASS_H
