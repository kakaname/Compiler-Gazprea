//
// Created by dhanrajbir on 15/11/22.
//

#ifndef GAZPREABASE_CONVERTIDENTMEMBERACCESSTOIDXPASS_H
#define GAZPREABASE_CONVERTIDENTMEMBERACCESSTOIDXPASS_H

#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"

struct ConvertIdentMemberAccessToIdxPass : VisitorPass<ConvertIdentMemberAccessToIdxPass, void> {
    void visitMemberAccess(MemberAccess *Access);
    void visitMemberReference(MemberReference *Ref);
    void runOnAST(ASTPassManager &M, ASTNodeT *Root) {
        PM = &M;
        visit(Root);
    }

    ASTPassManager *PM;
};

#endif //GAZPREABASE_CONVERTIDENTMEMBERACCESSTOIDXPASS_H
