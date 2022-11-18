//
// Created by dhanrajbir on 18/11/22.
//

#include "Passes/ConvertFuncCallNodesToProcCallPass.h"

void ConvertFuncCallNodesToProcCallPass::visitFunctionCall(FunctionCall *Call) {
    if (!isa<ProcedureTy>(Call->getIdentifier()->getIdentType()))
        return;
    auto ProcCall = PM->Builder.build<ProcedureCall>();
    ProcCall->setIdent(Call->getIdentifier());
    ProcCall->setArgsList(Call->getArgsList());
    ProcCall->copyCtx(Call);
    Call->getParent()->replaceChildWith(Call, ProcCall);
}
