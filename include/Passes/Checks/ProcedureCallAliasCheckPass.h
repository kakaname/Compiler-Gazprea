//
// Created by dhanrajbir on 16/11/22.
//

#ifndef GAZPREABASE_PROCEDURECALLALIASCHECKPASS_H
#define GAZPREABASE_PROCEDURECALLALIASCHECKPASS_H


#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"

struct ProcedureCallAliasCheckPass : VisitorPass<ProcedureCallAliasCheckPass, void> {
    void runOnAST(ASTPassManager &M, ASTNodeT *Root) {
        PM = &M;
        visit(Root);
    }

    void visitProcedureCall(ProcedureCall *Call);

    ASTPassManager *PM;
};

#endif //GAZPREABASE_PROCEDURECALLALIASCHECKPASS_H
