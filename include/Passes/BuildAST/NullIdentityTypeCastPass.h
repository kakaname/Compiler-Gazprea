//
// Created by Chen on 2022-11-15.
//

#ifndef GAZPREABASE_NULLIDENTITYTYPECASTPASS_H
#define GAZPREABASE_NULLIDENTITYTYPECASTPASS_H

#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"
#include "Symbol/Symbol.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"

struct NullIdentityTypeCastPass: public VisitorPass<NullIdentityTypeCastPass, void>  {
    ASTPassManager *PM;

    void visitTypeCast(TypeCast *Cast);

    ASTNodeT *getScalarLiteral(Type::TypeKind Kind, bool IsNull);

    void runOnAST(ASTPassManager &PManager, ASTNodeT *Root) {
        assert(isa<Program>(Root) && "ExplicitCastCheckPass should run on the entire program");
        PM = &PManager;
        visit(Root);
    }
};


#endif //GAZPREABASE_NULLIDENTITYTYPECASTPASS_H
