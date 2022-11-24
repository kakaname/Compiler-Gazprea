//
// Created by 陈 on 2022-11-14.
//

#ifndef GAZPREABASE_EXPLICITCASTCHECKPASS_H
#define GAZPREABASE_EXPLICITCASTCHECKPASS_H

#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"
#include "Symbol/Symbol.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"

struct ExplicitCastCheckPass: public VisitorPass<ExplicitCastCheckPass, void>  {
    ASTPassManager *PM;

    void visitExplicitCast(ExplicitCast *ExplicitCast);

    void runOnAST(ASTPassManager &PManager, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "ExplicitCastCheckPass should run on the entire program");
        PM = &PManager;
        visit(Root);
    }
};


#endif //GAZPREABASE_EXPLICITCASTCHECKPASS_H
