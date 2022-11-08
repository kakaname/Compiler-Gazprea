//
// Created by dhanrajbir on 11/10/22.
//

#ifndef VCALCBASE_SYMBOLTABLEDUMMYPASS_H
#define VCALCBASE_SYMBOLTABLEDUMMYPASS_H

#include "Passes/PassManager.h"
#include "SymbolTable.h"

// This pass serves to store a SymbolTable instance into the pass manager.
// This pass does no traversal/transformations.
struct SymbolTableDummyPass : public ASTPassIDMixin<SymbolTableDummyPass> {
    using ResultT = SymbolTable;
    void runOnAST(ASTPassManager &PM, ASTNodeT&) {
        auto SymTab = SymbolTable();
        PM.setResult<SymbolTableDummyPass>(std::forward<ResultT>(SymTab));
    };

    SymbolTableDummyPass(SymbolTableDummyPass &&) = default;
    SymbolTableDummyPass() = default;
};

#endif //VCALCBASE_SYMBOLTABLEDUMMYPASS_H
