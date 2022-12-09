//
// Created by dhanrajbir on 17/11/22.
//

#include "Passes/BuildAST/ChangeMemAccessToMemRef.h"

void ChangeMemAccessToMemRef::visitProcedureCall(ProcedureCall *Call) {
    auto ArgsList = Call->getArgsList();
    visit(ArgsList);
    auto ProcTy = dyn_cast<ProcedureTy>(Call->getIdentifier()->getIdentType());
    assert(ProcTy && "Malformed procedure");
    for (auto I = 0; I < ProcTy->getNumOfArgs(); I++) {
        if (ProcTy->getParamTypeAt(I)->isConst())
            continue;

        auto Expr = Call->getArgsList()->getExprAtPos(I);

        if (isa<IdentReference>(Expr))
            continue;

        if (isa<MemberReference>(Expr))
            continue;

        if (auto Ident = dyn_cast<Identifier>(Expr)) {
            ArgsList->setExprAtPos(getRefFromIdent(Ident), I);
            continue;
        }

        if (auto Indexing = dyn_cast<Index>(Expr)) {
            ArgsList->setExprAtPos(getRefFromIndex(Indexing), I);
            continue;
        }

        ArgsList->setExprAtPos(getRefFromMemAccess(cast<MemberAccess>(Expr)), I);
    }
}


void ChangeMemAccessToMemRef::visitAssignment(Assignment *Assign) {
    visit(Assign->getExpr());

    auto AssignedTo = Assign->getAssignedTo();
    assert(AssignedTo && "Malformed assignment");

    if (auto Indent = dyn_cast<Identifier>(AssignedTo)) {
        assert(!Indent->getIdentType()->isConst() && "Malformed assignment");

        Assign->setAssignedTo(getRefFromIdent(Indent));
        return;
    }

    if (auto Indexing = dyn_cast<Index>(AssignedTo)) {
        auto Ident = dyn_cast<Identifier>(Indexing->getBaseExpr());
        assert(!Ident->getIdentType()->isConst() && "Malformed assignment");

        Assign->setAssignedTo(getRefFromIndex(Indexing));
        return;
    }

    if (auto MemberAcc = dyn_cast<MemberAccess>(AssignedTo)) {
        auto Ident = dyn_cast<Identifier>(MemberAcc->getMemberExpr());
        assert(!Ident->getIdentType()->isConst() && "Malformed assignment");
        Assign->setAssignedTo(getRefFromMemAccess(MemberAcc));
        return;
    }
}


IdentReference *ChangeMemAccessToMemRef::getRefFromIdent(Identifier *Ident) const {
    auto IdentRef = PM->Builder.build<IdentReference>();
    IdentRef->setIdentifier(Ident);
    return IdentRef;
}

IndexReference *ChangeMemAccessToMemRef::getRefFromIndex(Index *Index) const {
    auto IndexRef = PM->Builder.build<IndexReference>();
    IndexRef->setBaseExpr(Index->getBaseExpr());
    IndexRef->setIndexExpr(Index->getIndexExpr());
    if (dyn_cast<MatrixTy>(PM->getAnnotation<ExprTypeAnnotatorPass>(Index->getBaseExpr())))
        IndexRef->setIndex2Expr(Index->getIndex2Expr());
    return IndexRef;
}

MemberReference *ChangeMemAccessToMemRef::getRefFromMemAccess(MemberAccess *Access) const {
    auto Ref = PM->Builder.build<MemberReference>();
    auto Ident = dyn_cast<Identifier>(Access->getExpr());
    assert(Ident && "Malformed member access");
    Ref->setIdentifier(Ident);
    Ref->setMemberExpr(Access->getMemberExpr());
    return Ref;
}
