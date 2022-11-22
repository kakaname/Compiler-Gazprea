//
// Created by dhanrajbir on 20/11/22.
//

#ifndef GAZPREABASE_BUBBLEGLOBALDECLARATIONPASS_H
#define GAZPREABASE_BUBBLEGLOBALDECLARATIONPASS_H


#include "Passes/PassManager.h"

struct BubbleGlobalDeclarationPass {
    void runOnAST(ASTPassManager &P, ASTNodeT *Root) {
        auto Prog = dyn_cast<Program>(Root);
        assert(Prog && "This pass must be run on the entire program");

        auto &Children = Prog->getChildren();

        // The global declaration must be
        Children.sort([](const ASTNodeT *N1, const ASTNodeT *N2){
            return isa<Declaration>(N1) && !isa<Declaration>(N2);
        });
    }
};

#endif //GAZPREABASE_BUBBLEGLOBALDECLARATIONPASS_H
