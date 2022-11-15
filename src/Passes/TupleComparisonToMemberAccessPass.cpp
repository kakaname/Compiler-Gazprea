//
// Created by 陈 on 2022-11-14.
//

#include "Passes/TupleComparisonToMemberAccessPass.h"
#include <vector>

void TupleComparisonToMemberAccessPass::visitLogicalOp(LogicalOp *Op) {
    auto LeftExpr = Op->getLeftExpr();
    auto RightExpr = Op->getRightExpr();

    // We only care about EQ Operation on two tuples in this pass
    if (!isa<TupleTy>(PM->getAnnotation<ExprTypeAnnotatorPass>(LeftExpr)) ||
        !isa<TupleTy>(PM->getAnnotation<ExprTypeAnnotatorPass>(RightExpr)) ||
        Op->getOpKind() != LogicalOp::EQ) {
        return;
    }

    // Change (tuple1 == tuple2) to tuple1.1 == tuple2.1 and tuple1.2 == tuple2.2 and tuple1.3 == tuple2.3 and ...
    auto ExprTy = dyn_cast<TupleTy>(PM->getAnnotation<ExprTypeAnnotatorPass>(LeftExpr));
    size_t NumOfMembers = ExprTy->getNumOfMembers();

    std::vector<LogicalOp*> EqualOps;
    for (size_t I = 0; I < NumOfMembers; I++) {
        // Set the left member access
        auto LeftMemberAccess = PM->Builder.build<MemberAccess>();
        LeftMemberAccess->setExpr(LeftExpr);
        auto LeftIntLit = PM->Builder.build<IntLiteral>();
        LeftIntLit->setVal(std::to_string(I + 1));
        LeftMemberAccess->setMemberExpr(LeftIntLit);

        // Set the right member access
        auto RightMemberAccess = PM->Builder.build<MemberAccess>();
        RightMemberAccess->setExpr(RightExpr);
        auto RightIntLit = PM->Builder.build<IntLiteral>();
        RightIntLit->setVal(std::to_string(I + 1));
        RightMemberAccess->setMemberExpr(RightIntLit);

        // Set the equal op
        auto EqualOp = PM->Builder.build<LogicalOp>();
        EqualOp->setOp(LogicalOp::EQ);
        EqualOp->setLeftExpr(LeftMemberAccess);
        EqualOp->setRightExpr(RightMemberAccess);

        EqualOps.push_back(EqualOp);
    }

    auto AndOp = PM->Builder.build<LogicalOp>();
    AndOp->setOp(LogicalOp::AND);
    LogicalOp *UpperAndOp = AndOp;

    AndOp->setLeftExpr(EqualOps[0]);
    for (size_t I = 0; I < EqualOps.size() - 1; ++I) {
        auto SubAndOp = PM->Builder.build<LogicalOp>();
        SubAndOp->setOp(LogicalOp::AND);
        SubAndOp->setLeftExpr(EqualOps[I]);
        UpperAndOp->setRightExpr(SubAndOp);
        UpperAndOp = SubAndOp;

        if (I < EqualOps.size() - 2)
            continue;

        SubAndOp->setRightExpr(EqualOps[I + 1]);
    }

    Op->getParent()->replaceChildWith(Op, AndOp);
}
