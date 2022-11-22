//
// Created by rajan on 11/11/22.
//

#include "Passes/Checks/EnsureReturnPass.h"
#include "Passes/PassManager.h"


bool EnsureReturnPass::visitFunctionDef(FunctionDef *FuncDef) {

    if (!visit(FuncDef->getBlock()))
        throw FunctionReturnError(FuncDef, FuncDef->getIdentifier()->getName());
}

bool EnsureReturnPass::visitBlock(Block *Blk) {
    for (auto *Child : *Blk) {
        if (isa<Return>(Child))
            return true;

        if (isa<ConditionalElse>(Child))
            if (visit(Child))
                return true;
    }
    return false;
}

bool EnsureReturnPass::visitProcedureDef(ProcedureDef *ProcDef) {
    // If the procedure has no return type we add a return
    // statement to the end of function body to make sure there is a return.
    if (!ProcDef->getRetTy()) {
        auto RetStmt = PM->Builder.build<Return>();
        RetStmt->setReturnExpr(PM->Builder.build<NoOp>());
        ProcDef->getBlock()->addChild(RetStmt);
        return true;
    }
    if (!visit(ProcDef->getBlock()))
        throw ProcedureReturnError(ProcDef, ProcDef->getIdentifier()->getName());

}

bool EnsureReturnPass::visitConditionalElse(ConditionalElse *Cond) {
    return visit(Cond->getIfBlock()) && visit(Cond->getElseBlock());
}
