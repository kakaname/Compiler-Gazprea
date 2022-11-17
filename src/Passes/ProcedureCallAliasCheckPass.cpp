//
// Created by dhanrajbir on 16/11/22.
//

#include "Passes/ProcedureCallAliasCheckPass.h"
#include "Passes/ExprTypeAnnotatorPass.h"

void ProcedureCallAliasCheckPass::visitFunctionCall(FunctionCall *Call) {
    auto ProcTy = dyn_cast<ProcedureTy>(Call->getIdentifier()->getIdentType());
    // We don't care about function calls.
    if (!ProcTy)
        return;

    set<pair<const Symbol*, int>> UsedSymbols;
    auto Args = Call->getArgsList();
    for (auto I = 0; I < Args->numOfChildren(); I++) {
        // Don't need to check const args.
        if (ProcTy->getParamTypeAt(I)->isConst())
            continue;

        auto ArgExpr = Call->getArgsList()->getExprAtPos(I);
        auto ArgExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(ArgExpr);
        assert(!ArgExprTy->isConst() &&
            "Trying to bind a const reference to a var reference");

        if (auto MemAccess = dyn_cast<MemberAccess>(ArgExpr)) {
            auto Ident = dyn_cast<Identifier>(MemAccess->getExpr());
            auto MemIdx = dyn_cast<IntLiteral>(MemAccess->getMemberExpr());
            assert(Ident && MemIdx && "Non transformed tuple access reached here.");

            if (UsedSymbols.count({Ident->getReferred(), 0})
                || UsedSymbols.count({Ident->getReferred(), MemIdx->getVal()}))
                assert(false && "Aliasing between mutable members.");


            UsedSymbols.insert({Ident->getReferred(), MemIdx->getVal()});
            continue;
        }

        if (auto Ident = dyn_cast<Identifier>(ArgExpr)) {
            pair Key = {Ident->getReferred(), 0};
            if (UsedSymbols.count(Key))
                assert(false && "Aliasing between mutable members.");
            UsedSymbols.insert(Key);
            continue;
        }

        assert("Trying to bind an r-value to a var argument?");
    }
}
