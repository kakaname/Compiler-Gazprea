//
// Created by rajan on 11/10/22.
//

/**
* This pass verifies that the AST for loops is well formed. In particular,
 * it checks that the loop domain is correct and that any break or continue
 * statements are within a loop.
 */



// For part 1, no AST nodes need to be modified.
//void LoopCheckPass::visitInfiniteLoop(InfiniteLoop *Loop) {
//}
//
//void LoopCheckPass::ConditionalLoop(ConditionalLoop *Loop) {
//}

#include "Passes/LoopCheckPass.h"
#include "Common/TreeNode.h"


void LoopCheckPass::visitBreak(Break *Break) {

    // loop through parents to find a loop
    TreeNode *Parent = Break->getParent();
    while (Parent) {
        if (Parent->getKind() == Break::N_AST_Program)
            // if we reach the program node, then we are not in a loop
            throw BreakError(999);
        if (Parent->getKind() == Break::N_AST_ConditionalLoop ||
            Parent->getKind() == Break::N_AST_InfiniteLoop) {

            // if we reach a loop node, then we are in a loop
            PM->setAnnotation<LoopCheckPass>(Break, Parent);
            return;
        }

        Parent = Parent->getParent();
    }

    // if we reach here, then we are also not in a loop or some
    // other error has occurred
    if (!Parent)
        throw BreakError(999);

}

void LoopCheckPass::visitContinue(Continue *Continue) {

    // loop through parents to find a loop
    TreeNode *Parent = Continue->getParent();
    while (Parent) {
        if (Parent->getKind() == Continue::N_AST_Program)
            // if we reach the program node, then we are not in a loop
            // FIXME line number
            throw ContinueError(999);
        if (Parent->getKind() == Continue::N_AST_ConditionalLoop ||
            Parent->getKind() == Continue::N_AST_InfiniteLoop) {

            // if we reach a loop node, then we are in a loop
            PM->setAnnotation<LoopCheckPass>(Continue, Parent);
            return;
        }

        Parent = Parent->getParent();
    }

    // if we reach here, then we are also not in a loop or some
    // other error has occurred
    if (!Parent)
        throw ContinueError(999);

}