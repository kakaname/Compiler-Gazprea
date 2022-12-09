//
// Created by dhanrajbir on 09/12/22.
//

#ifndef GAZPREABASE_SIMPLIFYTUPLELITERALMEMBERACCESS_H
#define GAZPREABASE_SIMPLIFYTUPLELITERALMEMBERACCESS_H


#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"
#include "Symbol/Symbol.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"

struct SimplifyTupleLiteralMemAccess: public VisitorPass<SimplifyTupleLiteralMemAccess, void>  {
    ASTPassManager *PM;

    void visitMemberAccess(MemberAccess *Access);

    void runOnAST(ASTPassManager &PManager, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "ExplicitCastCheckPass should run on the entire program");
        PM = &PManager;
        visit(Root);
    }
};




#endif //GAZPREABASE_SIMPLIFYTUPLELITERALMEMBERACCESS_H
