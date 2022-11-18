//
// Created by Chen on 2022-11-18.
//

#ifndef GAZPREABASE_LOOPIFCONDITIONALTYPECHECKPASS_H
#define GAZPREABASE_LOOPIFCONDITIONALTYPECHECKPASS_H


#include "VisitorPass.h"
#include "PassManager.h"
#include "Symbol/Symbol.h"
#include "Passes/ExprTypeAnnotatorPass.h"

struct LoopIfConditionalTypeCheckPass: public VisitorPass<LoopIfConditionalTypeCheckPass, void> {
    ASTPassManager *PM;

    void visitConditional(Conditional *Cond);

    void visitConditionalLoop(ConditionalLoop *Loop);

    void runOnAST(ASTPassManager &PManager, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "ExplicitCastCheckPass should run on the entire program");
        PM = &PManager;
        visit(Root);
    }
};


#endif //GAZPREABASE_LOOPIFCONDITIONALTYPECHECKPASS_H
