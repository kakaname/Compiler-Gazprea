#include "GazpreaLexer.h"
#include "GazpreaParser.h"
#include "llvm/Support/Casting.h"

#include "ANTLRFileStream.h"
#include "CommonTokenStream.h"
#include "tree/ParseTree.h"

#include "Passes/PassManager.h"
#include "Passes/ASTBuilderPass.h"
#include "Types/CompositeTypes.h"
#include "ErrorHandling/exceptions.h"

#include "Passes/ASTPrinterPass.h"
#include "Passes/ScopeResolutionPass.h"
#include "Passes/ConvertIdentMemberAccessToIdxPass.h"
#include "Passes/AssignmentTypeCheckerPass.h"
#include "Passes/CallableArgumentTypeCheckingPass.h"
#include "Passes/EnsureReturnPass.h"
#include "Passes/TupleCompToMemberCompPass.h"
#include "Passes/ASTPrinterPassWithTypes.h"
#include "Passes/ReturnValuePromotionPass.h"
#include "Passes/CodeGenPass.h"
#include "Passes/BadStreamPass.h"
#include "Passes/SimplifyTupleCasting.h"
#include "Passes/NullIdentityTypeCastPass.h"
#include "Passes/ExplicitCastCheckPass.h"
#include "Passes/ContinueAndBreakCheckPass.h"
#include "Passes/ProcedureCallAliasCheckPass.h"
#include "Passes/TupleNotEqualTransformationPass.h"
#include "Passes/ChangeMemAccessToMemRef.h"
#include "Passes/EnsureDefinitionPass.h"
#include "Passes/EnsureValidGlobalInitPass.h"

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
    Manager.registerAnonymousPass(ASTPrinterPass());


    Manager.registerPass(ScopeResolutionPass());
    Manager.registerPass(ExprTypeAnnotatorPass());
    Manager.registerPass(ConvertIdentMemberAccessToIdxPass());

    //
    Manager.registerPass(ExplicitCastCheckPass());
    Manager.registerPass(ContinueAndBreakCheckPass());
    Manager.registerPass(BadStreamPass());
    Manager.registerPass(EnsureDefinitionPass());
    Manager.registerPass(EnsureValidGlobalInitPass());
    Manager.registerPass(AssignmentTypeCheckerPass());
    Manager.registerPass(CallableArgumentTypeCheckingPass());
    Manager.registerPass(EnsureReturnPass());
    Manager.registerPass(ReturnValuePromotionPass());
    Manager.registerPass(ExprTypeAnnotatorPass());
    Manager.registerPass(ProcedureCallAliasCheckPass());
    Manager.registerPass(ASTPrinterPassWithTypes());

    //
    Manager.registerPass(ChangeMemAccessToMemRef());
    Manager.registerPass(ExprTypeAnnotatorPass());
    Manager.registerPass(SimplifyTupleCasting());
    Manager.registerPass(ExprTypeAnnotatorPass());
    Manager.registerPass(TupleNotEqualTransformationPass());
    Manager.registerPass(ExprTypeAnnotatorPass());
    Manager.registerPass(TupleCompToMemberCompPass());
    Manager.registerPass(ExprTypeAnnotatorPass());
    Manager.registerPass(NullIdentityTypeCastPass());
    Manager.registerPass(ExprTypeAnnotatorPass());
    Manager.registerPass(ASTPrinterPass());
    Manager.registerPass(ASTPrinterPassWithTypes());

    Manager.runAllPasses();
//
    auto CG = CodeGenPass("../gazout.ll");
    auto *Root = Manager.getRoot();
    CG.runOnAST(Manager, Root);
    return 0;

}
