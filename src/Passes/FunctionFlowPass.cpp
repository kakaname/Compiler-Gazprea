//
// Created by rajan on 11/11/22.
//

#include "Passes/FunctionFlowPass.h"
#include "Passes/PassManager.h"


bool FunctionFlowPass::visitFunctionDef(FunctionDef *FuncDef) {
    assert(visit(FuncDef->getBlock()) && "Not all branches"
                                         " lead to a return.");
}

bool FunctionFlowPass::visitBlock(Block *Blk) {
    for (auto *Child : *Blk) {
        if (isa<Return>(Child))
            return true;

        if (isa<ConditionalElse>(Child))
            if (visit(Child))
                return true;
    }
    return false;
}

bool FunctionFlowPass::visitProcedureDef(ProcedureDef *ProcDef) {
    // If the procedure has no return type, we don't care.
    if (!ProcDef->getRetTy())
        return true;
    assert(visit(ProcDef->getBlock()) && "Not all branches "
                                         "lead to a return.");
}

bool FunctionFlowPass::visitConditionalElse(ConditionalElse *Cond) {
    return visit(Cond->getIfBlock()) && visit(Cond->getElseBlock());
}
