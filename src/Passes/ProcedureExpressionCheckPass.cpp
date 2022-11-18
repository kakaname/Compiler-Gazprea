//
// Created by Chen on 2022-11-17.
//

#include "Passes/ProcedureExpressionCheckPass.h"

void ProcedureExpressionCheckPass::visitFunctionCall(FunctionCall *FuncCall) {

    // We only care about the procedure
    if (FuncCall->getIdentifier()->getIdentType()->getKind() == Type::T_Procedure) {
        auto Parent = FuncCall->getParent();
        while (Parent) {
            if (dyn_cast<Assignment>(Parent))
                break;
            if (dyn_cast<UnaryOp>(Parent))
                Parent = Parent->getParent();
            else
                throw SyntaxError("Invalid procedure call");
        }
    }

}
