//
// Created by Chen on 2022-12-01.
//

#ifndef GAZPREABASE_VECTORSCALARAPPENDTRANSFORMATIONPASS_H
#define GAZPREABASE_VECTORSCALARAPPENDTRANSFORMATIONPASS_H

#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"
#include "Symbol/Symbol.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"


struct VectorScalarAppendTransformationPass: public VisitorPass<VectorScalarAppendTransformationPass, void> {
    ASTPassManager *PM;

    void visitComparisonOp(ComparisonOp *Op);

    void visitArithmeticOp(ArithmeticOp *Op);

    void visitLogicalOp(LogicalOp *Op);

    bool isScalarTy(const Type *Ty);

    void runOnAST(ASTPassManager &PManager, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "ExplicitCastCheckPass should run on the entire program");
        PM = &PManager;
        visit(Root);
    }
};


#endif //GAZPREABASE_VECTORSCALARAPPENDTRANSFORMATIONPASS_H
