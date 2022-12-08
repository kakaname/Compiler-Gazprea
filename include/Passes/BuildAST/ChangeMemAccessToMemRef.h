//
// Created by dhanrajbir on 17/11/22.
//

#ifndef GAZPREABASE_CHANGEMEMACCESSTOMEMREF_H
#define GAZPREABASE_CHANGEMEMACCESSTOMEMREF_H

#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"
#include "Symbol/Symbol.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"

struct ChangeMemAccessToMemRef : public VisitorPass<ChangeMemAccessToMemRef, void> {
    void visitProcedureCall(ProcedureCall *Call);
    void visitAssignment(Assignment *Assign);


    IdentReference *getRefFromIdent(Identifier *Ident) const;

    IndexReference *getRefFromIndex(Index *Index) const;

    MemberReference *getRefFromMemAccess(MemberAccess *Access) const;

    void runOnAST(ASTPassManager &P, ASTNodeT *Root) {
        PM = &P;
        visit(Root);
    }

    ASTPassManager *PM;
};

#endif //GAZPREABASE_CHANGEMEMACCESSTOMEMREF_H
