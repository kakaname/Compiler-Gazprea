//
// Created by dhanrajbir on 05/11/22.
//

#include "Passes/ASTBuilderPass.h"
#include "llvm/Support/Casting.h"

using llvm::cast;
using llvm::isa;
using llvm::dyn_cast;

void ASTBuilderPass::runOnAST(ASTPassManager &Manager, ASTNodeT *Root) {
    PM = &Manager;
    assert((Prog = dyn_cast<Program>(Root)) && "The Root must be a program");
    visit(File);
}

std::any ASTBuilderPass::visitFile(GazpreaParser::FileContext *ctx) {
    for (auto Child : ctx->global())
        Prog->addChild(castToNodeVisit(Child));
    return nullptr;
}

std::any ASTBuilderPass::visitSimpleStmt(GazpreaParser::SimpleStmtContext *ctx) {
    // If it is a break statement
    if (ctx->BREAK()) {
        auto BreakStat = PM->Builder.build<Break>();
        return BreakStat;
    }
    // If it is a continue statement
    if (ctx->CONTINUE()) {
        auto ContinueStat = PM->Builder.build<Continue>();
        return ContinueStat;
    }
    // If it is another simple statement
    return visitChildren(ctx);
}

std::any ASTBuilderPass::visitIdentDecl(GazpreaParser::IdentDeclContext *ctx) {
    auto Decl = PM->Builder.build<Declaration>();

    // Mark the decl as const if we see a const type qualifier.
    if (ctx->typeQualifier()->CONST())
        Decl->setConst();

    // Set the type node to null if it is not specified. In this case, it will
    // be inferred by the type inference pass later.
    if (!ctx->type())
        Decl->setIdentTypeNode(nullptr);
    else
        Decl->setIdentTypeNode(castToNodeVisit(ctx->type()));
    // Build the identifier that is being assigned to.
    auto Ident = PM->Builder.build<Identifier>(Decl);
    Ident->setName(ctx->ID()->getText());

    // Set the expression to null if it is omitted, the default initializer
    // pass will make sure to set the init value to null for that type.
    if (!ctx->expr())
        Decl->setInitExpr(nullptr);
    else
        Decl->setInitExpr(castToNodeVisit(ctx->expr()));

    return Decl;
}

std::any ASTBuilderPass::visitAssignment(GazpreaParser::AssignmentContext *ctx) {
    auto Assign = PM->Builder.build<Assignment>();
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setName(ctx->ID()->getText());
    Assign->setIdentifier(Ident);
    Assign->setExpr(castToNodeVisit(ctx->expr()));
    return Assign;
}

std::any ASTBuilderPass::visitIfConditional(GazpreaParser::IfConditionalContext *ctx) {

}

std::any ASTBuilderPass::visitIfElseConditional(GazpreaParser::IfElseConditionalContext *ctx) {

}

std::any ASTBuilderPass::visitInfiniteLoop(GazpreaParser::InfiniteLoopContext *ctx) {

}

std::any ASTBuilderPass::visitWhileLoop(GazpreaParser::WhileLoopContext *ctx) {

}

std::any ASTBuilderPass::visitDoWhileLoop(GazpreaParser::DoWhileLoopContext *ctx) {

}

std::any ASTBuilderPass::visitDomainLoop(GazpreaParser::DomainLoopContext *ctx) {

}

std::any ASTBuilderPass::visitIterDomain(GazpreaParser::IterDomainContext *ctx) {

}

std::any ASTBuilderPass::visitTypeDef(GazpreaParser::TypeDefContext *ctx) {

}

std::any ASTBuilderPass::visitOutput(GazpreaParser::OutputContext *ctx) {

}

std::any ASTBuilderPass::visitInput(GazpreaParser::InputContext *ctx) {

}

std::any ASTBuilderPass::visitReturn(GazpreaParser::ReturnContext *ctx) {

}

std::any ASTBuilderPass::visitTypeQualifier(GazpreaParser::TypeQualifierContext *ctx) {

}

std::any ASTBuilderPass::visitResolvedType(GazpreaParser::ResolvedTypeContext *ctx) {

}

std::any ASTBuilderPass::visitTupleType(GazpreaParser::TupleTypeContext *ctx) {

}

std::any ASTBuilderPass::visitVectorType(GazpreaParser::VectorTypeContext *ctx) {

}

std::any ASTBuilderPass::visitMatrixType(GazpreaParser::MatrixTypeContext *ctx) {

}

std::any ASTBuilderPass::visitIntType(GazpreaParser::IntTypeContext *ctx) {

}

std::any ASTBuilderPass::visitCharType(GazpreaParser::CharTypeContext *ctx) {

}

std::any ASTBuilderPass::visitBooleanType(GazpreaParser::BooleanTypeContext *ctx) {

}

std::any ASTBuilderPass::visitRealType(GazpreaParser::RealTypeContext *ctx) {

}

std::any ASTBuilderPass::visitExpressionOrWildcard(GazpreaParser::ExpressionOrWildcardContext *ctx) {

}

std::any ASTBuilderPass::visitTupleTypeDecl(GazpreaParser::TupleTypeDeclContext *ctx) {

}

std::any ASTBuilderPass::visitTypeOptionalIdentPair(GazpreaParser::TypeOptionalIdentPairContext *ctx) {

}

std::any ASTBuilderPass::visitTypeIdentPair(GazpreaParser::TypeIdentPairContext *ctx) {

}

std::any ASTBuilderPass::visitFunctionDeclr(GazpreaParser::FunctionDeclrContext *ctx) {

}

std::any ASTBuilderPass::visitFunctionDefinition(GazpreaParser::FunctionDefinitionContext *ctx) {

}

std::any ASTBuilderPass::visitProcedureDeclr(GazpreaParser::ProcedureDeclrContext *ctx) {

}

std::any ASTBuilderPass::visitProcedureDefinition(GazpreaParser::ProcedureDefinitionContext *ctx) {

}

std::any ASTBuilderPass::visitFunctionCall(GazpreaParser::FunctionCallContext *ctx) {

}

std::any ASTBuilderPass::visitProcedureCall(GazpreaParser::ProcedureCallContext *ctx) {

}

std::any ASTBuilderPass::visitBlock(GazpreaParser::BlockContext *ctx) {

}

std::any ASTBuilderPass::visitExplicitCast(GazpreaParser::ExplicitCastContext *ctx) {

}

std::any ASTBuilderPass::visitBoolLiteral(GazpreaParser::BoolLiteralContext *ctx) {

}

std::any ASTBuilderPass::visitUnaryExpr(GazpreaParser::UnaryExprContext *ctx) {

}

std::any ASTBuilderPass::visitGeneratorExpr(GazpreaParser::GeneratorExprContext *ctx) {

}

std::any ASTBuilderPass::visitExpExpr(GazpreaParser::ExpExprContext *ctx) {

}

std::any ASTBuilderPass::visitCompExpr(GazpreaParser::CompExprContext *ctx) {

}

std::any ASTBuilderPass::visitIdentityLiteral(GazpreaParser::IdentityLiteralContext *ctx) {

}

std::any ASTBuilderPass::visitMemberAccess(GazpreaParser::MemberAccessContext *ctx) {

}

std::any ASTBuilderPass::visitIdentifier(GazpreaParser::IdentifierContext *ctx) {
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setName(ctx->getText());
    return Ident;
}

std::any ASTBuilderPass::visitNullLiteral(GazpreaParser::NullLiteralContext *ctx) {

}

std::any ASTBuilderPass::visitAddSubExpr(GazpreaParser::AddSubExprContext *ctx) {

}

std::any ASTBuilderPass::visitBracketExpr(GazpreaParser::BracketExprContext *ctx) {

}

std::any ASTBuilderPass::visitRealLiteral(GazpreaParser::RealLiteralContext *ctx) {

}

std::any ASTBuilderPass::visitIntLiteral(GazpreaParser::IntLiteralContext *ctx) {

}

std::any ASTBuilderPass::visitMulDivModSSExpr(GazpreaParser::MulDivModSSExprContext *ctx) {

}

std::any ASTBuilderPass::visitByExpr(GazpreaParser::ByExprContext *ctx) {

}

std::any ASTBuilderPass::visitOrExpr(GazpreaParser::OrExprContext *ctx) {

}

std::any ASTBuilderPass::visitFilterExpr(GazpreaParser::FilterExprContext *ctx) {

}

std::any ASTBuilderPass::visitCharLiteral(GazpreaParser::CharLiteralContext *ctx) {

}

std::any ASTBuilderPass::visitIndexExpr(GazpreaParser::IndexExprContext *ctx) {

}

std::any ASTBuilderPass::visitTupleLiteral(GazpreaParser::TupleLiteralContext *ctx) {

}

std::any ASTBuilderPass::visitAppendOp(GazpreaParser::AppendOpContext *ctx) {

}

std::any ASTBuilderPass::visitFuncCall(GazpreaParser::FuncCallContext *ctx) {

}

std::any ASTBuilderPass::visitRangeExpr(GazpreaParser::RangeExprContext *ctx) {

}

std::any ASTBuilderPass::visitEqualExpr(GazpreaParser::EqualExprContext *ctx) {

}

std::any ASTBuilderPass::visitAndExpr(GazpreaParser::AndExprContext *ctx) {

}

std::any ASTBuilderPass::visitRealLit(GazpreaParser::RealLitContext *ctx) {

}

std::any ASTBuilderPass::visitSciRealLiteral(GazpreaParser::SciRealLiteralContext *ctx) {

}

std::any ASTBuilderPass::visitMainReal(GazpreaParser::MainRealContext *ctx) {

}

std::any ASTBuilderPass::visitIntReal(GazpreaParser::IntRealContext *ctx) {

}

std::any ASTBuilderPass::visitDotReal(GazpreaParser::DotRealContext *ctx) {

}
