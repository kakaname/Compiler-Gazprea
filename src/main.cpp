#include "GazpreaLexer.h"
#include "GazpreaParser.h"
#include "llvm/Support/Casting.h"

#include "ANTLRFileStream.h"
#include "CommonTokenStream.h"
#include "tree/ParseTree.h"

#include "Passes/PassManager.h"
#include "Passes/BuildAST/ASTBuilderPass.h"
#include "Types/CompositeTypes.h"
#include "ErrorHandling/exceptions.h"

#include "Passes/BuildAST/ScopeResolutionPass.h"
#include "Passes/BuildAST/ConvertIdentMemberAccessToIdxPass.h"
#include "Passes/Checks/AssignmentTypeCheckerPass.h"
#include "Passes/Checks/CallableArgumentTypeCheckingPass.h"
#include "Passes/Checks/EnsureReturnPass.h"
#include "Passes/Transformations/TupleCompToMemberCompPass.h"
#include "Passes/Utils/ASTPrinterPassWithTypes.h"
#include "Passes/Checks/ReturnValuePromotionPass.h"
#include "Codegen/CodeGenPass.h"
#include "Passes/Checks/BadStreamPass.h"
#include "Passes/Transformations/SimplifyTupleCasting.h"
#include "Passes/BuildAST/NullIdentityTypeCastPass.h"
#include "Passes/Checks/ExplicitCastCheckPass.h"
#include "Passes/Checks/ContinueAndBreakCheckPass.h"
#include "Passes/Checks/ProcedureCallAliasCheckPass.h"
#include "Passes/Transformations/TupleNotEqualTransformationPass.h"
#include "Passes/BuildAST/ChangeMemAccessToMemRef.h"
#include "Passes/Checks/EnsureDefinitionPass.h"
#include "Passes/Checks/EnsureValidGlobalInitPass.h"
#include "Passes/Checks/LValueReferenceCheckPass.h"
#include "Passes/BuildAST/ConvertFuncCallNodesToProcCallPass.h"
#include "Passes/Transformations/SubExpressionCacheSet.h"
#include "Passes/Transformations/BubbleGlobalDeclarationPass.h"
#include "Passes/Utils/ASTPrinterPass.h"
#include "Passes/Transformations/SimplifyCompositeTypeCasting.h"
#include "Passes/Transformations/TupleUnpackToAssign.h"

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
  if (argc < 3) {
    std::cout << "Missing required argument.\n"
              << "Required arguments: <input file path> <output file path>\n";
    return 1;
  }

  // Open the file then parse and lex it.
  antlr4::ANTLRFileStream afs;
  afs.loadFromFile(argv[1]);
  gazprea::GazpreaLexer lexer(&afs);
  antlr4::CommonTokenStream tokens(&lexer);
  gazprea::GazpreaParser parser(&tokens);

  // Setup error listener
  SyntaxErrorListener error_listener;
  parser.removeErrorListeners();
  parser.addErrorListener(&error_listener);

  // Get the root of the parse tree. Use your base rule name.
  antlr4::tree::ParseTree *tree = parser.file();

    ASTPassManager Manager;
    Manager.registerPass(ASTBuilderPass(tree));

    // Set the resource for the cache set.
    Manager.setResource<SubExpressionCacheSet>(
        SubExpressionCacheSet());

    Manager.registerPass(ScopeResolutionPass());
//    Manager.registerAnonymousPass(ASTPrinterPass());
    Manager.registerPass(ExprTypeAnnotatorPass());
//    Manager.registerPass(ASTPrinterPassWithTypes());
    Manager.registerPass(ConvertIdentMemberAccessToIdxPass());
//    Manager.registerPass(ASTPrinterPassWithTypes());
    Manager.registerPass(ExprTypeAnnotatorPass());
//    Manager.registerPass(ASTPrinterPassWithTypes());

    Manager.registerPass(ConvertFuncCallNodesToProcCallPass());
    Manager.registerPass(EnsureReturnPass());
    Manager.registerPass(ExprTypeAnnotatorPass());
    Manager.registerPass(ExplicitCastCheckPass());
    Manager.registerPass(ContinueAndBreakCheckPass());
    Manager.registerPass(BadStreamPass());
    Manager.registerPass(LValueReferenceCheckPass());
    Manager.registerPass(EnsureDefinitionPass());
    Manager.registerPass(EnsureValidGlobalInitPass());
    Manager.registerPass(ExprTypeAnnotatorPass());
    Manager.registerPass(AssignmentTypeCheckerPass());
    Manager.registerPass(CallableArgumentTypeCheckingPass());
    Manager.registerPass(ReturnValuePromotionPass());
    Manager.registerPass(ExprTypeAnnotatorPass());
    Manager.registerPass(ProcedureCallAliasCheckPass());

    Manager.registerPass(ChangeMemAccessToMemRef());
    Manager.registerPass(ExprTypeAnnotatorPass());
//    Manager.registerPass(ASTPrinterPassWithTypes());
    Manager.registerPass(NullIdentityTypeCastPass());
    Manager.registerPass(ExprTypeAnnotatorPass());
    Manager.registerPass(SimplifyTupleCasting());
    Manager.registerPass(ExprTypeAnnotatorPass());
    Manager.registerPass(TupleNotEqualTransformationPass());
    Manager.registerPass(ExprTypeAnnotatorPass());
    Manager.registerPass(TupleCompToMemberCompPass());
    Manager.registerPass(TupleUnpackToAssignTransformationPass());
    Manager.registerPass(ExprTypeAnnotatorPass());
    Manager.registerPass(NullIdentityTypeCastPass());
    Manager.registerPass(ExprTypeAnnotatorPass());
    Manager.registerAnonymousPass(BubbleGlobalDeclarationPass());
//    Manager.registerPass(ASTPrinterPassWithTypes());

    Manager.runAllPasses();
    auto CG = CodeGenPass(argv[2]);
    CG.runOnAST(Manager, Manager.getRoot());
    return 0;

}
