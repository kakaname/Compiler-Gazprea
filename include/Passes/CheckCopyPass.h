//
// Created by youngwo2 on 05/11/22.
//

#ifndef GAZPREABASE_BADSTREAMPASS_H
#define GAZPREABASE_BADSTREAMPASS_H

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_os_ostream.h"

#include "Passes/PassManager.h"
#include "Passes/VisitorPass.h"
#include "Passes/ExprTypeAnnotatorPass.h"
#include "Symbol/Symbol.h"


struct CheckCopyPass: public VisitorPass<CheckCopyPass, void>  {

    void visitProcedureDecl(ProcedureDecl *ProcedureDecl);
    
    void visitFunctionDecl(FunctionDecl *FuncDecl);

    void runOnAST(ASTPassManager &P, ASTNodeT *Root) {
        PM = &P;
        visit(Root);
    };

private:
    bool checkExist(std::string str){
        for(auto var : map){
            if(var.compare(str) == 0){
                return true;
            }
        }
        return false;
    }
    void addIdentifier(std::string str){
        map.push_back(str);

    }
    std::vector<std::string> map;

    ASTPassManager *PM;
};
#endif //GAZPREABASE_BADSTREAMPASS_H 