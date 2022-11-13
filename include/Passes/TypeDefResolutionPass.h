//
// Created by dhanrajbir on 12/11/22.
//

#ifndef GAZPREABASE_TYPEDEFRESOLUTIONPASS_H
#define GAZPREABASE_TYPEDEFRESOLUTIONPASS_H


#include "VisitorPass.h"
#include "PassManager.h"
#include "ScopeResolutionPass.h"


struct TypeDefResolutionPass {
    void runOnAST(ASTPassManager &PM, ASTNodeT &Root) {
        PM.setResource<ScopeTreeNode>(ScopeTreeNode());
        auto GlobalScope =  &PM.getResource<ScopeTreeNode>();
        assert(isa<Program>(&Root) && "Must be run on the Program");
        TypeDef *TypeDefNode;
        for (auto *GlobalStmt : Root) {
            if (!(TypeDefNode = dyn_cast<TypeDef>(GlobalStmt)))
                continue;

        }
    }
};


#endif //GAZPREABASE_TYPEDEFRESOLUTIONPASS_H
