//
// Created by Chen on 2022-12-08.
//

#include "Passes/BuildAST/AddingFreeNodesForContinueBreak.h"


void AddingFreeNodesForContinueBreak::visitInfiniteLoop(InfiniteLoop *Loop) {
    visit(Loop->getBlock());
}

void AddingFreeNodesForContinueBreak::visitConditionalLoop(ConditionalLoop *Loop) {
    visit(Loop->getBlock());
}

void AddingFreeNodesForContinueBreak::visitDomainLoop(DomainLoop *Loop) {
    visit(Loop);
}