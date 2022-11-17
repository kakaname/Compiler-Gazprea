//
// Created by youngwo2 on 05/11/22.
//

#include "Passes/BadStreamPass.h"
#include "Common/TreeNode.h"
#include "ErrorHandling/exceptions.h"

void BadStreamPass::visitOutStream(OutStream *OutStream){
    bool run = true;
    TreeNode *node = OutStream->getParent();
    while(run){

        if(node->getKind() == TreeNode::N_AST_FunctionDef){
            throw StreamInFuncError(999);
            // raise error
            run = false;
        }
        if(node->getKind() == TreeNode::N_AST_ProcedureDef){
            run = false;
        }
        node = node->getParent();

    }

}
void BadStreamPass::visitInStream(InStream *InStream){
    bool run = true;
    TreeNode *node = InStream->getParent();
    while(run){
        
        if(node->getKind() == TreeNode::N_AST_FunctionDef){
            throw StreamInFuncError(999);
            // raise error
            run = false;
        }
        if(node->getKind() == TreeNode::N_AST_ProcedureDef){
            run = false;
        }
        node = node->getParent();
    }
    
}