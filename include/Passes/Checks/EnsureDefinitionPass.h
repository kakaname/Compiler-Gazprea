//
// Created by youngwo2 on 05/11/22.
//
#ifndef GAZPREABASE_ENSURESINGLEDEFPASS_H
#define GAZPREABASE_ENSURESINGLEDEFPASS_H


#include <set>
#include <string>

#include "Passes/VisitorPass.h"
#include "Passes/PassManager.h"


using std::string;
using std::set;

struct EnsureDefinitionPass: public VisitorPass<EnsureDefinitionPass, void>  {

    void visitFunctionDef(FunctionDef *Def);
    void visitProcedureDef(ProcedureDef *Def);

    void visitProcedureDecl(ProcedureDecl *Decl);
    void visitFunctionDecl(FunctionDecl *Decl);

    void runOnAST(ASTPassManager &P, ASTNodeT *Root) {
        visit(Root);
        if (Defined != Declared)
            throw std::runtime_error("Some procedures/functions have declarations"
                                     " but no definitions");

        if (!MainTy)
            throw std::runtime_error("There must be a main procedure in the program");

        auto Ty = dyn_cast<ProcedureTy>(MainTy);
        if (!Ty)
            throw std::runtime_error("The main must be a procedure");
        auto CorrectMainTy = P.TypeReg.getProcedureType({}, P.TypeReg.getIntegerTy());
        if (!Ty->isSameTypeAs(CorrectMainTy))
            throw std::runtime_error("The main must have type" + CorrectMainTy->getTypeName());
    };

    set<string> Declared;
    set<string> Defined;

    const Type *MainTy{nullptr};
};
#endif //GAZPREABASE_ENSURESINGLEDEFPASS_H