//
// Created by Chen on 2022-11-17.
//

#include "Passes/ProcedureCallStatCheckPass.h"

void ProcedureCallStatCheckPass::visitProcedureCall(ProcedureCall *ProcedureCall) {
    if (ProcedureCall->getIdentifier()->getIdentType()->getKind() == Type::T_Function)
        throw SyntaxError("Invalid function call");
}
