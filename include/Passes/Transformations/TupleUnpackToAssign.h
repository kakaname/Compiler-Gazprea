#pragma once

#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"
#include "Symbol/Symbol.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"

struct TupleUnpackToAssignTransformationPass: public VisitorPass<TupleUnpackToAssignTransformationPass, void> {
    ASTPassManager *PM;

    void visitAssignment(Assignment *Assign);
    MemberAccess *buildMemberAccess(ASTNodeT *BaseExpr, int Idx) const;

    void runOnAST(ASTPassManager &PManager, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "ExplicitCastCheckPass should run on the entire program");
        PM = &PManager;
        visit(Root);
    }
};

