//
// Created by youngwo2 on 05/11/22.
//

#include "Passes/Checks/EnsureDefinitionPass.h"

using llvm::isa;

void EnsureDefinitionPass::visitFunctionDef(FunctionDef *Def) {
    auto Name = Def->getIdentifier()->getName();
    if (Name == "main")
        MainTy = Def->getIdentifier()->getIdentType();
    if (Defined.count(Name))
        throw std::runtime_error("Duplicate function definition");
    Declared.insert(Name);
    Defined.insert(Name);
}

void EnsureDefinitionPass::visitProcedureDef(ProcedureDef *Def) {
    auto Name = Def->getIdentifier()->getName();
    if (Name == "main")
        MainTy = Def->getIdentifier()->getIdentType();
    if (Defined.count(Name))
        throw std::runtime_error("Duplicate procedure definition");
    Declared.insert(Name);
    Defined.insert(Name);
}

void EnsureDefinitionPass::visitProcedureDecl(ProcedureDecl *Decl) {
    Declared.insert(Decl->getIdentifier()->getName());
}

void EnsureDefinitionPass::visitFunctionDecl(FunctionDecl *Decl) {
    Declared.insert(Decl->getIdentifier()->getName());
}
