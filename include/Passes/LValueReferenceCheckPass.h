//
// Created by dhanrajbir on 18/11/22.
//

#ifndef GAZPREABASE_LVALUEREFERENCECHECKPASS_H
#define GAZPREABASE_LVALUEREFERENCECHECKPASS_H

#include "VisitorPass.h"
#include "PassManager.h"

struct LValueReferenceCheckPass : VisitorPass<LValueReferenceCheckPass, void> {
    void visitMemberReference(MemberReference *Ref);
    void visitIdentReference(IdentReference *Ref);

    void runOnAST(ASTPassManager &P, ASTNodeT *Root) {
        PM = &P;
        visit(Root);
    }

    ASTPassManager *PM;
};


#endif //GAZPREABASE_LVALUEREFERENCECHECKPASS_H
