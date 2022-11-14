//
// Created by dhanrajbir on 12/11/22.
//

#ifndef GAZPREABASE_TYPEDEFRESOLUTIONPASS_H
#define GAZPREABASE_TYPEDEFRESOLUTIONPASS_H


#include "VisitorPass.h"
#include "PassManager.h"
#include "ScopeResolutionPass.h"
#include "TypeBuilderPass.h"


struct TypeDefResolutionPass {
    void runOnAST(ASTPassManager &PM, ASTNodeT &Root) {

    }
};


#endif //GAZPREABASE_TYPEDEFRESOLUTIONPASS_H
