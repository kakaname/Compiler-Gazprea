//
// Created by dhanrajbir on 17/11/22.
//

#ifndef GAZPREABASE_ENSUREDECLINBEGINOFBLOCK_H
#define GAZPREABASE_ENSUREDECLINBEGINOFBLOCK_H


#include "VisitorPass.h"

struct EnsureDeclInBeginOfBlock : public VisitorPass<EnsureDeclInBeginOfBlock, void> {
    void visitBlock(Block *Blk);

    void runOnAST(ASTPassManager &, ASTNodeT *Root) {
        visit(Root);
    }
};

#endif //GAZPREABASE_ENSUREDECLINBEGINOFBLOCK_H
