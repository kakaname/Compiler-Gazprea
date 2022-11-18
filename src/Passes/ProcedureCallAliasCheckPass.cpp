//
// Created by dhanrajbir on 16/11/22.
//

#include "Passes/ProcedureCallAliasCheckPass.h"
#include "Passes/ExprTypeAnnotatorPass.h"

void ProcedureCallAliasCheckPass::visitProcedureCall(ProcedureCall *Call) {
    auto ProcTy = dyn_cast<ProcedureTy>(Call->getIdentifier()->getIdentType());
    assert(ProcTy && "Procedure call not calling procedures?");

    set<pair<const Symbol*, int>> UsedSymbols;
    auto Args = Call->getArgsList();
    for (auto I = 0; I < Args->numOfChildren(); I++) {
        // Don't need to check const args.
        if (ProcTy->getParamTypeAt(I)->isConst())
            continue;

        auto ArgExpr = Call->getArgsList()->getExprAtPos(I);
        auto ArgExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(ArgExpr);

        if (ArgExprTy->isConst())
            throw ConstantArgumentError(Call, I + 1, Call->getIdentifier()->getName());

        if (auto MemAccess = dyn_cast<MemberAccess>(ArgExpr)) {
            auto Ident = dyn_cast<Identifier>(MemAccess->getExpr());
            auto MemIdx = dyn_cast<IntLiteral>(MemAccess->getMemberExpr());
            assert(Ident && MemIdx && "Non transformed tuple access reached here.");

            if (UsedSymbols.count({Ident->getReferred(), 0})
                || UsedSymbols.count({Ident->getReferred(), MemIdx->getVal()}))
                throw AliasError(Call);


            UsedSymbols.insert({Ident->getReferred(), MemIdx->getVal()});
            continue;
        }

        if (auto Ident = dyn_cast<Identifier>(ArgExpr)) {
            pair Key = {Ident->getReferred(), 0};
            if (UsedSymbols.count(Key))
                throw AliasError(Call);
            UsedSymbols.insert(Key);
            continue;
        }

        assert("Trying to bind an r-value to a var argument?");
    }
}
