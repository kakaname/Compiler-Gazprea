//
// Created by dhanrajbir on 15/11/22.
//

#include "Passes/ConvertIdentMemberAccessToIdxPass.h"
#include "Types/CompositeTypes.h"

using llvm::isa;

void ConvertIdentMemberAccessToIdxPass::visitMemberAccess(MemberAccess *Access) {
    auto Ident = Access->getIdentifier();
    auto Expr = Access->getMemberExpr();
    auto Tuple = dyn_cast<TupleTy>(Ident->getIdentType());
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
    Access->setMemberExpr(NewExpr);
}
