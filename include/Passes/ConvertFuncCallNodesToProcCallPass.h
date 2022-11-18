//
// Created by dhanrajbir on 18/11/22.
//

#ifndef GAZPREABASE_CONVERTFUNCCALLNODESTOPROCCALLPASS_H
#define GAZPREABASE_CONVERTFUNCCALLNODESTOPROCCALLPASS_H

#include "VisitorPass.h"
#include "PassManager.h"

struct ConvertFuncCallNodesToProcCallPass : VisitorPass<ConvertFuncCallNodesToProcCallPass, void> {
    void visitFunctionCall(FunctionCall *Call);

    void runOnAST(ASTPassManager &P, ASTNodeT *Root) {
        PM = &P;
        visit(Root);
    }

    ASTPassManager *PM;
};

#endif //GAZPREABASE_CONVERTFUNCCALLNODESTOPROCCALLPASS_H
