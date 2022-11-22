//
// Created by dhanrajbir on 18/11/22.
//

#ifndef GAZPREABASE_SUBEXPRESSIONCACHESET_H
#define GAZPREABASE_SUBEXPRESSIONCACHESET_H

#include <map>
#include <llvm/IR/Value.h>

#include "Passes/PassManagerResource.h"
#include "AST/ASTNodes.h"

using std::set;
using std::map;

struct SubExpressionCacheSet : ResourceIdMixin<SubExpressionCacheSet> {

    void addCachedNode(ASTNodeT *N) {
        CachedNodes.insert(N);
    }

    bool shouldCache(ASTNodeT *N) const {
        return CachedNodes.count(N);
    }

    void setValueFor(ASTNodeT *N, llvm::Value *V) {
        Cache[N] = V;
    }

    llvm::Value *getCached(ASTNodeT *N) {
        auto Res = Cache.find(N);
        if (Res == Cache.end())
            return nullptr;
        return Res->second;
    }

    map<ASTNodeT*, llvm::Value*> Cache;
    set<ASTNodeT*> CachedNodes;
};

#endif //GAZPREABASE_SUBEXPRESSIONCACHESET_H
