//
// Created by youngwo2 on 05/11/22.
//

#include "Passes/BadStreamPass.h"
#include "Common/TreeNode.h"

void BadStreamPass::visitOutStream(OutStream *OutStream){
    auto OutNode = OutStream;
    auto Node = OutStream->getParent();
    while(Node){
        if(isa<FunctionDef>(Node))
            throw std::runtime_error("Stream in function");

        if (isa<ProcedureDef>(Node))
            return;
        Node = Node->getParent();
    }
}

void BadStreamPass::visitInStream(InStream *InStream){
    auto InNode = InStream;
    auto Node = InStream->getParent();
    while(Node){
        if(isa<FunctionDef>(Node))
            throw std::runtime_error("Stream in function");

        if (isa<ProcedureDef>(Node))
            return;
        Node = Node->getParent();
    }
}