//
// Created by youngwo2 on 05/11/22.
//

#include "Passes/CheckCopyPass.h"
#include "Common/TreeNode.h"

void CheckCopyPass::visitProcedureDecl(ProcedureDecl *ProcedureDecl){
   std::string str = ProcedureDecl->getIdentifier()->getName();
    if(CheckCopyPass::checkExist(str)){
        throw std::runtime_error("function or procedure has the same name as previous function or procedure");
    }else{
        CheckCopyPass::addIdentifier(str);
    }


}
    
void CheckCopyPass::visitFunctionDecl(FunctionDecl *FuncDecl){
   std::string str = FuncDecl->getIdentifier()->getName();
    if(CheckCopyPass::checkExist(str)){
        throw std::runtime_error("function or procedure has the same name as previous function or procedure");
    }else{
        CheckCopyPass::addIdentifier(str);
    }

}