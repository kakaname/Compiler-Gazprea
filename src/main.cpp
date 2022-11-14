#include "GazpreaLexer.h"
#include "GazpreaParser.h"
#include "llvm/Support/Casting.h"

#include "ANTLRFileStream.h"
#include "CommonTokenStream.h"
#include "tree/ParseTree.h"
#include "tree/ParseTreeWalker.h"

#include "Passes/PassManager.h"
#include "Passes/ASTBuilderPass.h"

#include "Common/TestPasses.h"
#include "Types/CompositeTypes.h"
#include "ErrorHandling/exceptions.h"

#include "Symbol/SymbolTable.h"

#include <iostream>
#include <fstream>

using llvm::dyn_cast;
using llvm::isa;
using llvm::cast;

class SyntaxErrorListener: public antlr4::BaseErrorListener {
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token * offendingSymbol,
                     size_t line, size_t charPositionInLine, const std::string &msg,
                     std::exception_ptr e) override {
        std::vector<std::string> rule_stack = ((antlr4::Parser*) recognizer)->getRuleInvocationStack();
        // The rule_stack may be used for determining what rule and context the error has occurred in.
        // You may want to print the stack along with the error message, or use the stack contents to
        // make a more detailed error message.

        throw SyntaxError(msg); // Throw our exception with ANTLR's error message. You can customize this as appropriate.
    }
};

int main(int argc, char **argv) {
//  if (argc < 3) {
//    std::cout << "Missing required argument.\n"
//              << "Required arguments: <input file path> <output file path>\n";
//    return 1;
//  }

  // Open the file then parse and lex it.
  antlr4::ANTLRFileStream afs;
  afs.loadFromFile("../test_gaz");
  gazprea::GazpreaLexer lexer(&afs);
  antlr4::CommonTokenStream tokens(&lexer);
  gazprea::GazpreaParser parser(&tokens);

  // Setup error listener
  SyntaxErrorListener error_listener;
  parser.removeErrorListeners();
  parser.addErrorListener(&error_listener);

  // Get the root of the parse tree. Use your base rule name.
  antlr4::tree::ParseTree *tree = parser.file();


  // HOW TO USE A VISITOR
  // Make the visitor
//   ASTBuilderPass visitor;
  // Visit the tree
//   visitor.visit(tree);

  // HOW TO WRITE OUT.
  // std::ofstream out(argv[2]);
  // out << "This is out...\n";

//    Type *T = &Vec;

//    auto *V = dyn_cast<VectorTy>(T);
//    std::cout << V->isConst() << std::endl;
//    TreeNodeBuilder Builder;
//    auto *Assign = Builder.build<Assignment>();
    ASTPassManager Manager;
    Manager.registerPass(ASTBuilderPass(tree));
//    Manager.registerPass(SetsInt());
//  Manager.registerPass(SetsCustomResult());
//  Manager.registerAnonymousPass(PrintsResults());
//  Manager.registerPass(GetsResultInvalidated());
//  Manager.registerAnonymousPass(InvalidatesResults());
    Manager.runAllPasses();
    return 0;

}
