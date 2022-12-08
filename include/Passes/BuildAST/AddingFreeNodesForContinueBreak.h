//
// Created by Chen on 2022-12-08.
//

#ifndef GAZPREABASE_ADDINGFREENODESFORCONTINUEBREAK_H
#define GAZPREABASE_ADDINGFREENODESFORCONTINUEBREAK_H

#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"
#include "ErrorHandling/exceptions.h"
#include <vector>
using std::vector;


struct AddingFreeNodesForContinueBreak: public VisitorPass<AddingFreeNodesForContinueBreak, void> {
    ASTPassManager *PM;
    bool WithinTheSameLoop;
    vector<Identifier *> FreedIdentifiers;


    void visitInfiniteLoop(InfiniteLoop *Loop);

    void visitConditionalLoop(ConditionalLoop *Loop);

    void visitDomainLoop(DomainLoop *Loop);

    void runOnAST(ASTPassManager &PManager, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "ExplicitCastCheckPass should run on the entire program");
        PM = &PManager;
        visit(Root);
    }
};


#endif //GAZPREABASE_ADDINGFREENODESFORCONTINUEBREAK_H
