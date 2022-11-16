//
// Created by dhanrajbir on 15/11/22.
//

#ifndef GAZPREABASE_CALLABLEARGUMENTTYPECHECKINGPASS_H
#define GAZPREABASE_CALLABLEARGUMENTTYPECHECKINGPASS_H

#include "VisitorPass.h"
#include "PassManager.h"

struct CallableArgumentTypeCheckingPass : VisitorPass<CallableArgumentTypeCheckingPass, void> {
    void visitFunctionCall(FunctionCall *Call);

    void checkProcCall(FunctionCall *Call, const ProcedureTy *Ty);

    void checkFuncCall(FunctionCall *Call, const FunctionTy *Ty);


    TypeCast *wrapWithCastTo(ASTNodeT *Expr, const Type *Ty) const;

    void runOnAST(ASTPassManager &M, ASTNodeT *Root) {
        PM = &M;
        visit(Root);
    }

    ASTPassManager *PM;
};

#endif //GAZPREABASE_CALLABLEARGUMENTTYPECHECKINGPASS_H
