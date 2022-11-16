//
// Created by 陈 on 2022-11-14.
//

#include "Passes/TupleCompToMemberCompPass.h"
#include <vector>

void TupleCompToMemberCompPass::visitLogicalOp(LogicalOp *Op) {
    auto LeftExpr = Op->getLeftExpr();
    auto RightExpr = Op->getRightExpr();

    visit(LeftExpr);
    visit(RightExpr);

    auto LeftTuple = dyn_cast<TupleTy>(PM->getAnnotation<ExprTypeAnnotatorPass>(LeftExpr));
    auto RightType = PM->getAnnotation<ExprTypeAnnotatorPass>(RightExpr);

    // We only care about EQ Operation on two tuples in this pass
    if (!LeftTuple || !isa<TupleTy>(RightType))
        return;

    size_t MemCount = LeftTuple->getNumOfMembers();

    std::vector<LogicalOp*> EqualOps;
    for (int I = 0; I < MemCount; I++) {
        // Set the left member access
        auto LeftMemberAccess = buildMemberAccess(LeftExpr, I+1);
        auto RightMemberAccess = buildMemberAccess(RightExpr, I+1);

        // Setup the comparison
        auto EqualOp = buildLogicalOp(
                LeftMemberAccess, RightMemberAccess, Op->getOpKind());
        EqualOps.push_back(EqualOp);
    }

    auto AndOp = PM->Builder.build<LogicalOp>();
    AndOp->setOp(LogicalOp::AND);
    AndOp->setLeftExpr(EqualOps[0]);

    LogicalOp *First = AndOp;
    for (size_t I = 0; I < EqualOps.size() - 1; ++I) {
        auto Second = PM->Builder.build<LogicalOp>();
        Second->setOp(LogicalOp::AND);
        Second->setLeftExpr(EqualOps[I]);
        First->setRightExpr(Second);
        First = Second;

        if (I < EqualOps.size() - 2)
            continue;

        Second->setRightExpr(EqualOps[I + 1]);
    }

    Op->getParent()->replaceChildWith(Op, AndOp);
}

MemberAccess *TupleCompToMemberCompPass::buildMemberAccess(
        ASTNodeT *BaseExpr, int Idx) const {
    auto IntLit = PM->Builder.build<IntLiteral>();
    IntLit->setIntVal(Idx);

    auto Access = PM->Builder.build<MemberAccess>();
    Access->setExpr(BaseExpr);
    Access->setMemberExpr(IntLit);

    return Access;
}

LogicalOp *
TupleCompToMemberCompPass::buildLogicalOp(
        ASTNodeT *LExpr, ASTNodeT *RExpr, LogicalOp::OpKind OpKind) const {
    auto Op = PM->Builder.build<LogicalOp>();
    Op->setLeftExpr(LExpr);
    Op->setRightExpr(RExpr);
    Op->setOp(OpKind);
    return Op;
}
