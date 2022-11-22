//
// Created by 陈 on 2022-11-14.
//

#ifndef GAZPREABASE_TUPLENOTEQUALTRANSFORMATIONPASS_H
#define GAZPREABASE_TUPLENOTEQUALTRANSFORMATIONPASS_H

#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"
#include "Symbol/Symbol.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"

struct TupleNotEqualTransformationPass: public VisitorPass<TupleNotEqualTransformationPass, void> {
    ASTPassManager *PM;

    void visitLogicalOp(LogicalOp *Op);

    void runOnAST(ASTPassManager &PManager, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "ExplicitCastCheckPass should run on the entire program");
        PM = &PManager;
        visit(Root);
    }
};


#endif //GAZPREABASE_TUPLENOTEQUALTRANSFORMATIONPASS_H
