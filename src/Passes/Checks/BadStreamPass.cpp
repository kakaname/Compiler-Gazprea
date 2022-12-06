//
// Created by youngwo2 on 05/11/22.
//

#include "Passes/Checks/BadStreamPass.h"
#include "Common/TreeNode.h"

void BadStreamPass::visitOutStream(OutStream *OutStream){
    auto Node = OutStream->getParent();
    while(Node){
        if(isa<FunctionDef>(Node))
            throw StreamInFuncError(OutStream);

        if (isa<ProcedureDef>(Node))
            return;
        Node = Node->getParent();
    }
}

void BadStreamPass::visitInStream(InStream *InStream){
    auto Node = InStream->getParent();
    while(Node){
        if(isa<FunctionDef>(Node))
            throw StreamInFuncError(InStream);

        if (isa<ProcedureDef>(Node))
            return;
        Node = Node->getParent();
    }
}