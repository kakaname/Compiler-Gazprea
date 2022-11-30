//
// Created by Chen on 2022-11-29.
//

#ifndef GAZPREABASE_VECTORSCALAROPTRANSFORMATIONPASS_H
#define GAZPREABASE_VECTORSCALAROPTRANSFORMATIONPASS_H

#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"
#include "Symbol/Symbol.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"


struct VectorScalarOpTransformationPass: public VisitorPass<VectorScalarOpTransformationPass, void> {
    ASTPassManager *PM;

    void visitComparisonOp(ComparisonOp *Op);

    void visitArithmeticOp(ArithmeticOp *Op);

    void visitLogicalOp(LogicalOp *Op);


    void runOnAST(ASTPassManager &PManager, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "ExplicitCastCheckPass should run on the entire program");
        PM = &PManager;
        visit(Root);
    }
};


#endif //GAZPREABASE_VECTORSCALAROPTRANSFORMATIONPASS_H
