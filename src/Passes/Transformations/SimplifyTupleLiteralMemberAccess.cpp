//
// Created by dhanrajbir on 09/12/22.
//
#include "Passes/Transformations/SimplifyTupleLiteralMemberAccess.h"

void SimplifyTupleLiteralMemAccess::visitMemberAccess(MemberAccess *Access) {
    visit(Access->getMemberExpr());
    visit(Access->getExpr());
    auto Lit = llvm::dyn_cast<TupleLiteral>(Access->getExpr());
    if (!Lit)
        return;
    auto MemberIdx = llvm::dyn_cast<IntLiteral>(Access->getMemberExpr());
    if (!MemberIdx)
        return;

    auto TargetExpr = Lit->getExprAtPos(MemberIdx->getVal() - 1);
    auto TargetTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Access);
    PM->setAnnotation<ExprTypeAnnotatorPass>(TargetExpr, TargetTy);
    Access->getParent()->replaceChildWith(Access, TargetExpr);
}
