//
// Created by dhanrajbir on 18/11/22.
//

#include "Passes/EnsureValidGlobalInitPass.h"


void EnsureValidGlobalInitPass::visitProgram(Program *Prog) {
    for (auto Child : *Prog) {
        auto GlobalDecl = dyn_cast<Declaration>(Child);
        if (!GlobalDecl)
            continue;
        visit(GlobalDecl->getInitExpr());
    }
}

void EnsureValidGlobalInitPass::visitProcedureCall(ProcedureCall *Call) {
    throw std::runtime_error("The initializer for global declarations"
                             " must not have procedure calls");
}

void EnsureValidGlobalInitPass::visitFunctionCall(FunctionCall *Call) {
    throw std::runtime_error("The initializer for global declarations"
                             " must not have function calls");
}
