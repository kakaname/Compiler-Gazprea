//
// Created by 陈 on 2022-11-14.
//

#ifndef GAZPREABASE_TUPLECOMPARISONTOMEMBERACCESSPASS_H
#define GAZPREABASE_TUPLECOMPARISONTOMEMBERACCESSPASS_H


#include "VisitorPass.h"
#include "PassManager.h"
#include "Symbol/Symbol.h"
#include "Passes/ExprTypeAnnotatorPass.h"

struct TupleComparisonToMemberAccessPass: public VisitorPass<TupleComparisonToMemberAccessPass, void> {
    ASTPassManager *PM;

    void visitLogicalOp(LogicalOp *Op);

    void runOnAST(ASTPassManager &PManager, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "ExplicitCastCheckPass should run on the entire program");
        PM = &PManager;
        visit(Root);
    }
};


#endif //GAZPREABASE_TUPLECOMPARISONTOMEMBERACCESSPASS_H
