//
// Created by dhanrajbir on 17/11/22.
//

#include "Passes/ChangeMemAccessToMemRef.h"

void ChangeMemAccessToMemRef::visitProcedureCall(ProcedureCall *Call) {
    auto ArgsList = Call->getArgsList();
    visit(ArgsList);
    auto ProcTy = dyn_cast<ProcedureTy>(Call->getIdentifier()->getIdentType());
    assert(ProcTy && "Malformed procedure");
    for (auto I = 0; I < ProcTy->getNumOfArgs(); I++) {
        if (ProcTy->getParamTypeAt(I)->isConst())
            continue;

        auto Expr = Call->getArgsList()->getExprAtPos(I);

        if (auto Ident = dyn_cast<Identifier>(Expr)) {
            ArgsList->setExprAtPos(getRefFromIdent(Ident), I);
            continue;
        }

        ArgsList->setExprAtPos(getRefFromMemAccess(cast<MemberAccess>(Expr)), I);
    }
}

IdentReference *ChangeMemAccessToMemRef::getRefFromIdent(Identifier *Ident) const {
    auto IdentRef = PM->Builder.build<IdentReference>();
    IdentRef->setIdentifier(Ident);
    return IdentRef;
}

MemberReference *ChangeMemAccessToMemRef::getRefFromMemAccess(MemberAccess *Access) const {
    auto Ref = PM->Builder.build<MemberReference>();
    auto Ident = dyn_cast<Identifier>(Access->getExpr());
    assert(Ident && "Malformed member access");
    Ref->setIdentifier(Ident);
    Ref->setMemberExpr(Access->getMemberExpr());
    return Ref;
}
