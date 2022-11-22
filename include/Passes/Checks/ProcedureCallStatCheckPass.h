//
// Created by Chen on 2022-11-17.
//

#ifndef GAZPREABASE_PROCEDURECALLSTATCHECKPASS_H
#define GAZPREABASE_PROCEDURECALLSTATCHECKPASS_H

#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"
#include "ErrorHandling/exceptions.h"

struct ProcedureCallStatCheckPass: public VisitorPass<ProcedureCallStatCheckPass, void> {
    ASTPassManager *PM;

    void visitProcedureCall(ProcedureCall *ProcedureCall);

    void runOnAST(ASTPassManager &PManager, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "ExplicitCastCheckPass should run on the entire program");
        PM = &PManager;
        visit(Root);
    }
};


#endif //GAZPREABASE_PROCEDURECALLSTATCHECKPASS_H
