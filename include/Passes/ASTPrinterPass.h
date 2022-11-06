//
// Created by Chen on 2022-11-05.
//

#ifndef GAZPREABASE_ASTPRINTERPASS_H
#define GAZPREABASE_ASTPRINTERPASS_H

#include "PassManager.h"
#include "VisitorPass.h"
#include "SymbolTable.h"
#include "SymbolTableDummyPass.h"
#include "ExprTypeAnnotatorPass.h"

class ASTPrinterPass {

    void runOnAST(ASTPassManager &PManager, ASTNodeT &Root) {
        PM = &PManager;
        SymTab = &PManager.getResult<SymbolTableDummyPass>();
        this->Root = &Root;
        printTree(this->Root);
    }
};


#endif //GAZPREABASE_ASTPRINTERPASS_H
