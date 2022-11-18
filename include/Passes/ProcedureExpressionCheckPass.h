//
// Created by Chen on 2022-11-17.
//

#ifndef GAZPREABASE_PROCEDUREEXPRESSIONCHECKPASS_H
#define GAZPREABASE_PROCEDUREEXPRESSIONCHECKPASS_H

#include "VisitorPass.h"
#include "PassManager.h"
#include "Passes/ExprTypeAnnotatorPass.h"
#include "ErrorHandling/exceptions.h"

/*
 * This pass is to check the procedure expression is only called in the assignment statement and is only
 * called in a unary expression
 */

struct ProcedureExpressionCheckPass: public VisitorPass<ProcedureExpressionCheckPass, void> {
    ASTPassManager *PM;

    void visitFunctionCall(FunctionCall *FuncCall);


    void runOnAST(ASTPassManager &PManager, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "ExplicitCastCheckPass should run on the entire program");
        PM = &PManager;
        visit(Root);
    }
};


#endif //GAZPREABASE_PROCEDUREEXPRESSIONCHECKPASS_H
