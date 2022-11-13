//
// Created by Chen on 2022-11-05.
//

#include "Passes/ASTPrinterPass.h"

void ASTPrinterPass::printTree(ASTNodeT *Root) {
    // Print the root of AST
    visit(Root);
    printSubTree(Root, "");
    std::cout << "\n";
    std::cout << "----------------------------------------------------------\n";
}

void ASTPrinterPass::printSubTree(ASTNodeT *Root, const string &Prefix) {

}
