//
// Created by rajan on 11/10/22.
//

/**
* This pass verifies that the AST for loops is well formed. In particular,
 * it checks that the loop domain is correct and that any break or continue
 * statements are within a loop.
 */

// For part 1, no AST nodes need to be modified.
//void ContinueAndBreakCheckPass::visitInfiniteLoop(InfiniteLoop *Loop) {
//}
//
//void ContinueAndBreakCheckPass::ConditionalLoop(ConditionalLoop *Loop) {
//}

#include "Passes/ContinueAndBreakCheckPass.h"
#include "Common/TreeNode.h"


void ContinueAndBreakCheckPass::visitBreak(Break *Break) {

    // The break statement must have a loop as it its parent somewhere.
    TreeNode *Curr = Break->getParent();
    while (Curr) {
        if (Curr->isLoopNode())
            return;
        if (isa<Program>(Curr))
            throw std::runtime_error("Break outside the loop");
        Curr = Curr->getParent();
    }
    throw std::runtime_error("Break outside the loop");
}

void ContinueAndBreakCheckPass::visitContinue(Continue *Continue) {

    // loop through parents to find a loop
    TreeNode *Curr = Continue->getParent();
    while (Curr) {
        if (Curr->isLoopNode())
            return;
        if (isa<Program>(Curr))
            throw std::runtime_error("Continue outside the loop");
        Curr = Curr->getParent();
    }
    throw std::runtime_error("Continue outside the loop");

}