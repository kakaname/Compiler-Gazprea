//
// Created by 陈 on 2022-11-14.
//

#ifndef GAZPREABASE_TUPLECOMPTOMEMBERCOMPPASS_H
#define GAZPREABASE_TUPLECOMPTOMEMBERCOMPPASS_H


#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"
#include "Symbol/Symbol.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"
// Change (tuple1 == tuple2) to tuple1.1 == tuple2.1 and tuple1.2 == tuple2.2 and so on
struct TupleCompToMemberCompPass: public VisitorPass<TupleCompToMemberCompPass, void> {
    ASTPassManager *PM;

    void visitLogicalOp(LogicalOp *Op);

    MemberAccess *buildMemberAccess(ASTNodeT *BaseExpr, int Idx) const;

    LogicalOp *buildLogicalOp(ASTNodeT *LExpr, ASTNodeT *RExpr, LogicalOp::OpKind OpKind) const;

    void runOnAST(ASTPassManager &PManager, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "ExplicitCastCheckPass should run on the entire program");
        PM = &PManager;
        visit(Root);
    }
};


#endif //GAZPREABASE_TUPLECOMPTOMEMBERCOMPPASS_H
