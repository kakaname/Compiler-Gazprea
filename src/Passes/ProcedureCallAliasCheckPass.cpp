//
// Created by dhanrajbir on 16/11/22.
//

#include "Passes/ProcedureCallAliasCheckPass.h"

void ProcedureCallAliasCheckPass::visitFunctionCall(FunctionCall *Call) {
    auto ProcTy = dyn_cast<ProcedureTy>(Call->getIdentifier()->getIdentType());
    // We don't care about function calls.
    if (!ProcTy)
        return;
    
    // TODO: Ask Deric about aliasing due to tuple vars and then complete this.
    auto Args = Call->getArgsList();
    for (auto I = 0; I < Args->numOfChildren(); I++) {
        // Don't need to check const args.
        if (ProcTy->getArgTypeAt(I)->isConst())
            continue;
    }
}
