//
// Created by dhanrajbir on 17/11/22.
//


#include "Passes/EnsureDeclInBeginOfBlock.h"
#include "llvm/Support/Casting.h"

using llvm::isa;

void EnsureDeclInBeginOfBlock::visitBlock(Block *Blk) {
    for (auto Child : *Blk)
        visit(Child);

    auto Idx = 0;
    // Skip all declarations in the beginning.
    while (Idx < Blk->numOfChildren() && isa<Declaration>(
            Blk->getChildAt(Idx++)));

    while (Idx < Blk->numOfChildren())
        if (isa<Declaration>(Blk->getChildAt(Idx++)))
            throw DeclarationPositionError(
                    Blk->getChildAt(Idx-1), "Declarations should be in the beginning");
}
