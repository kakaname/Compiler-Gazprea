//
// Created by dhanrajbir on 15/11/22.
//

#include "Passes/ConvertIdentMemberAccessToIdxPass.h"
#include "Types/CompositeTypes.h"
#include "Passes/ExprTypeAnnotatorPass.h"

using llvm::isa;

void ConvertIdentMemberAccessToIdxPass::visitMemberAccess(MemberAccess *Access) {
    auto Expr = Access->getMemberExpr();
    auto ExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Access->getExpr());
    auto Tuple = dyn_cast<TupleTy>(ExprTy);
    if (!Tuple)
        throw NonAccessibleError(Access, ExprTy->getTypeName());

    if (auto IntLit = dyn_cast<IntLiteral>(Expr)) {
        if(!(IntLit->getVal() <= Tuple->getNumOfMembers() && IntLit->getVal() > 0))
            throw OutOfRangeError(Access, IntLit->getVal(), Tuple->getNumOfMembers(), Tuple->getTypeName());
        return;
    }

    auto MemberName = dyn_cast<Identifier>(Expr);
    if (!MemberName)
        throw TupleExpressionError(Access);

    auto ResolvedIdx = Tuple->getMemberIdx(MemberName->getName());
    if (!ResolvedIdx)
        throw TupleAccessError(Access, Tuple->getTypeName(), MemberName->getName());

    auto NewExpr = PM->Builder.build<IntLiteral>();
    NewExpr->copyCtx(Access);
    NewExpr->setIntVal(ResolvedIdx);
    Access->setMemberExpr(NewExpr);
}

void ConvertIdentMemberAccessToIdxPass::visitMemberReference(MemberReference *Ref) {
    auto Expr = Ref->getMemberExpr();
    auto IdentTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Ref->getIdentifier());
    auto Tuple = dyn_cast<TupleTy>(IdentTy);
    assert(Tuple && "Only tuples may have their members accessed");

    if (auto IntLit = dyn_cast<IntLiteral>(Expr)) {
        assert(IntLit->getVal() <= Tuple->getNumOfMembers() && IntLit->getVal() > 0);
        return;
    }

    auto MemberName = dyn_cast<Identifier>(Expr);
    assert(MemberName && "Invalid expression used to access member");
    auto ResolvedIdx = Tuple->getMemberIdx(MemberName->getName());
    assert(ResolvedIdx && "Member not found in tuple!");
    auto NewExpr = PM->Builder.build<IntLiteral>();
    NewExpr->setIntVal(ResolvedIdx);
    Ref->setMemberExpr(NewExpr);
}
