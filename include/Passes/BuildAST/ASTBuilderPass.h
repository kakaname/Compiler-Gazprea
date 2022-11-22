//
// Created by dhanrajbir on 05/11/22.
//

#ifndef GAZPREABASE_ASTBUILDERPASS_H
#define GAZPREABASE_ASTBUILDERPASS_H

#include "GazpreaBaseVisitor.h"
#include "AST/ASTNodes.h"
#include "Passes/PassManager.h"

using gazprea::GazpreaParser;
using std::string;

class ASTBuilderPass: public gazprea::GazpreaBaseVisitor, public ASTPassIDMixin<ASTBuilderPass> {

    using AnnotationT = map<string, int>;

    ASTPassManager *PM{};

    Program *Prog;

    antlr4::tree::ParseTree *File;

    ASTNodeT *castToNodeVisit(antlr4::tree::ParseTree *Tree) {
        return std::any_cast<ASTNodeT*>(visit(Tree));
    }

    const Type *castToTypeVisit(antlr4::tree::ParseTree *Tree) {
        return std::any_cast<const Type*>(visit(Tree));
    }

    std::any visitFile(GazpreaParser::FileContext *ctx) override;

    std::any visitGlobalIdentDecl(GazpreaParser::GlobalIdentDeclContext *ctx) override;

    std::any visitGlobalIdentDeclStmt(GazpreaParser::GlobalIdentDeclStmtContext *ctx) override;

    std::any visitFunctionDeclrStmt(GazpreaParser::FunctionDeclrStmtContext *ctx) override;

    std::any visitFunctionDefStmt(GazpreaParser::FunctionDefStmtContext *ctx) override;

    std::any visitProcedureDeclrStmt(GazpreaParser::ProcedureDeclrStmtContext *ctx) override;

    std::any visitProcedureDefStmt(GazpreaParser::ProcedureDefStmtContext *ctx) override;

    std::any visitTypeDefStmt(GazpreaParser::TypeDefStmtContext *ctx) override;

    std::any visitIdentDeclStmt(GazpreaParser::IdentDeclStmtContext *ctx) override;

    std::any visitAssignmentStmt(GazpreaParser::AssignmentStmtContext *ctx) override;

    std::any visitConditionalStmt(GazpreaParser::ConditionalStmtContext *ctx) override;

    std::any visitLoopStmt(GazpreaParser::LoopStmtContext *ctx) override;

    std::any visitOutputStmt(GazpreaParser::OutputStmtContext *ctx) override;

    std::any visitInputStmt(GazpreaParser::InputStmtContext *ctx) override;

    std::any visitProcedureCallStmt(GazpreaParser::ProcedureCallStmtContext *ctx) override;

    std::any visitBreakStmt(GazpreaParser::BreakStmtContext *ctx) override;

    std::any visitContinueStmt(GazpreaParser::ContinueStmtContext *ctx) override;

    std::any visitIdentDecl(GazpreaParser::IdentDeclContext *ctx) override;

    std::any visitAssignment(GazpreaParser::AssignmentContext *ctx) override;

    std::any visitStmt(GazpreaParser::StmtContext *ctx) override;

    std::any visitIfConditional(GazpreaParser::IfConditionalContext *ctx) override;

    std::any visitIfElseConditional(GazpreaParser::IfElseConditionalContext *ctx) override;

    std::any visitInfiniteLoop(GazpreaParser::InfiniteLoopContext *ctx) override;

    std::any visitWhileLoop(GazpreaParser::WhileLoopContext *ctx) override;

    std::any visitDomainLoop(GazpreaParser::DomainLoopContext *ctx) override;

    std::any visitDoWhileLoop(GazpreaParser::DoWhileLoopContext *ctx) override;

    std::any visitIterDomain(GazpreaParser::IterDomainContext *ctx) override;

    std::any visitTypeDef(GazpreaParser::TypeDefContext *ctx) override;

    std::any visitOutput(GazpreaParser::OutputContext *ctx) override;

    std::any visitInput(GazpreaParser::InputContext *ctx) override;

    std::any visitReturn(GazpreaParser::ReturnContext *ctx) override;

    // Type nodes.
    std::any visitResolvedType(GazpreaParser::ResolvedTypeContext *ctx) override;

    std::any visitTupleType(GazpreaParser::TupleTypeContext *ctx) override;

    std::any visitVectorType(GazpreaParser::VectorTypeContext *ctx) override;

    std::any visitMatrixType(GazpreaParser::MatrixTypeContext *ctx) override;

    std::any visitIntType(GazpreaParser::IntTypeContext *ctx) override;

    std::any visitCharType(GazpreaParser::CharTypeContext *ctx) override;

    std::any visitBooleanType(GazpreaParser::BooleanTypeContext *ctx) override;

    std::any visitRealType(GazpreaParser::RealTypeContext *ctx) override;

    std::any visitExpressionOrWildcard(GazpreaParser::ExpressionOrWildcardContext *ctx) override;

    std::any visitFunctionDeclr(GazpreaParser::FunctionDeclrContext *ctx) override;

    std::any visitFunctionDefinition(GazpreaParser::FunctionDefinitionContext *ctx) override;

    std::any visitProcedureDeclr(GazpreaParser::ProcedureDeclrContext *ctx) override;

    std::any visitProcedureDefinition(GazpreaParser::ProcedureDefinitionContext *ctx) override;

    std::any visitFunctionCall(GazpreaParser::FunctionCallContext *ctx) override;

    std::any visitProcedureCall(GazpreaParser::ProcedureCallContext *ctx) override;

    std::any visitBlock(GazpreaParser::BlockContext *ctx) override;

    std::any visitExplicitCast(GazpreaParser::ExplicitCastContext *ctx) override;

    std::any visitBoolLiteral(GazpreaParser::BoolLiteralContext *ctx) override;

    std::any visitUnaryExpr(GazpreaParser::UnaryExprContext *ctx) override;

    std::any visitGeneratorExpr(GazpreaParser::GeneratorExprContext *ctx) override;

    std::any visitExpExpr(GazpreaParser::ExpExprContext *ctx) override;

    std::any visitCompExpr(GazpreaParser::CompExprContext *ctx) override;

    std::any visitIdentityLiteral(GazpreaParser::IdentityLiteralContext *ctx) override;

    std::any visitMemberAccess(GazpreaParser::MemberAccessContext *ctx) override;

    std::any visitIdentifier(GazpreaParser::IdentifierContext *ctx) override;

    std::any visitNullLiteral(GazpreaParser::NullLiteralContext *ctx) override;

    std::any visitAddSubExpr(GazpreaParser::AddSubExprContext *ctx) override;

    std::any visitBracketExpr(GazpreaParser::BracketExprContext *ctx) override;

    std::any visitIntLiteral(GazpreaParser::IntLiteralContext *ctx) override;

    std::any visitMulDivModDotProdExpr(GazpreaParser::MulDivModDotProdExprContext *ctx) override;

    std::any visitByExpr(GazpreaParser::ByExprContext *ctx) override;

    std::any visitOrExpr(GazpreaParser::OrExprContext *ctx) override;

    std::any visitFilterExpr(GazpreaParser::FilterExprContext *ctx) override;

    std::any visitCharLiteral(GazpreaParser::CharLiteralContext *ctx) override;

    std::any visitIndexExpr(GazpreaParser::IndexExprContext *ctx) override;

    std::any visitTupleLiteral(GazpreaParser::TupleLiteralContext *ctx) override;

    std::any visitAppendOp(GazpreaParser::AppendOpContext *ctx) override;

    std::any visitFuncCall(GazpreaParser::FuncCallContext *ctx) override;

    std::any visitRangeExpr(GazpreaParser::RangeExprContext *ctx) override;

    std::any visitEqualExpr(GazpreaParser::EqualExprContext *ctx) override;

    std::any visitAndExpr(GazpreaParser::AndExprContext *ctx) override;

    std::any visitRealLiteral(GazpreaParser::RealLiteralContext *ctx) override;

    std::any visitIdentLValue(GazpreaParser::IdentLValueContext *ctx) override;

    std::any visitIndexLValue(GazpreaParser::IndexLValueContext *ctx) override;

    std::any visitMemAccessLValue(GazpreaParser::MemAccessLValueContext *ctx) override;

    std::any visitTupleUnpackLValue(GazpreaParser::TupleUnpackLValueContext *ctx) override;

    std::any visitRealLit1(GazpreaParser::RealLit1Context *ctx) override;

    std::any visitRealLit2(GazpreaParser::RealLit2Context *ctx) override;

    std::any visitRealLit3(GazpreaParser::RealLit3Context *ctx) override;

    Block *wrapStmtInBlock(ASTNodeT *Stmt);

public:

    explicit ASTBuilderPass(antlr4::tree::ParseTree *F): File(F) {};

    void runOnAST(ASTPassManager &Manager, ASTNodeT *Root);
};


#endif //GAZPREABASE_ASTBUILDERPASS_H
