//
// Created by dhanrajbir on 18/11/22.
//

#ifndef GAZPREABASE_ENSUREVALIDGLOBALINITPASS_H
#define GAZPREABASE_ENSUREVALIDGLOBALINITPASS_H


#include "Passes/VisitorPass.h"

struct EnsureValidGlobalInitPass : VisitorPass<EnsureValidGlobalInitPass, void> {
    static void visitFunctionCall(FunctionCall *Call);
    static void visitProcedureCall(ProcedureCall *Call);
    void visitProgram(Program *Prog);

    void runOnAST(ASTPassManager &, ASTNodeT *Root) {
        visit(Root);
    }
};

#endif //GAZPREABASE_ENSUREVALIDGLOBALINITPASS_H
