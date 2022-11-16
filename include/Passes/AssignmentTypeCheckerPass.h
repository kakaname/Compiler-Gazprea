//
// Created by dhanrajbir on 15/11/22.
//

#ifndef GAZPREABASE_ASSIGNMENTTYPECHECKERPASS_H
#define GAZPREABASE_ASSIGNMENTTYPECHECKERPASS_H

#include "VisitorPass.h"

#include "PassManager.h"

struct AssignmentTypeCheckerPass : public VisitorPass<AssignmentTypeCheckerPass, void> {
    void visitAssignment(Assignment *Assign);
    void visitDeclaration(Declaration *Decl);


    void runOnAST(ASTPassManager &M, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "This pass must be run on the root");
        PM = &M;
        visit(Root);
    }
    ASTPassManager *PM;
};

#endif //GAZPREABASE_ASSIGNMENTTYPECHECKERPASS_H
