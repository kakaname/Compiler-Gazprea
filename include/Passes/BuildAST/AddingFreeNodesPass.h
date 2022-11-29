//
// Created by Chen on 2022-11-28.
//

#ifndef GAZPREABASE_ADDINGFREENODESPASS_H
#define GAZPREABASE_ADDINGFREENODESPASS_H

#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"
#include "ErrorHandling/exceptions.h"

struct AddingFreeNodesPass: public VisitorPass<AddingFreeNodesPass, void> {
    ASTPassManager *PM;


    

    void runOnAST(ASTPassManager &PManager, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "ExplicitCastCheckPass should run on the entire program");
        PM = &PManager;
        visit(Root);
    }
};


#endif //GAZPREABASE_ADDINGFREENODESPASS_H
