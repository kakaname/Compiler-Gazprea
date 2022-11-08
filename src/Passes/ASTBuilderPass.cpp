//
// Created by dhanrajbir on 05/11/22.
//

#include "Passes/ASTBuilderPass.h"
#include "llvm/Support/Casting.h"
#include "Types/ScalarTypes.h"
#include "Types/CompositeTypes.h"
#include "Types/Type.h"
#include <memory>

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
    else {
        auto TypeNode = castToNodeVisit(ctx->type());
        Decl->setIdentTypeNode(TypeNode);
        TypeNode->setParent(Decl);
    }

    // Build the identifier that is being assigned to.
    auto Ident = PM->Builder.build<Identifier>(Decl);
    Ident->setName(ctx->ID()->getText());
    Decl->setIdent(Ident);
    Ident->setParent(Decl);

    // Set the expression to null if it is omitted, the default initializer
    // pass will make sure to set the init value to null for that type.
    if (!ctx->expr())
        Decl->setInitExpr(nullptr);
    else {
        auto *Expr = castToNodeVisit(ctx->expr());
        Decl->setInitExpr(Expr);
        Expr->setParent(Decl);
    }

    return Decl;
}

std::any ASTBuilderPass::visitAssignment(GazpreaParser::AssignmentContext *ctx) {
    auto Assign = PM->Builder.build<Assignment>();
    auto Ident = PM->Builder.build<Identifier>();

    Ident->setName(ctx->ID()->getText());
    Assign->setIdentifier(Ident);
    Ident->setParent(Assign);

    auto *Expr = castToNodeVisit(ctx->expr());
    Assign->setExpr(Expr);
    Expr->setParent(Assign);

    return Assign;
}

std::any ASTBuilderPass::visitIfConditional(GazpreaParser::IfConditionalContext *ctx) {
    auto IfStat = PM->Builder.build<Conditional>();

    // Set the conditional expression
    auto CondExpr = castToNodeVisit(ctx->expr());
    IfStat->setConditional(CondExpr);
    CondExpr->setParent(IfStat);

    // Set the statement body
    auto StatementBody = castToNodeVisit(ctx->stmt());
    IfStat->setStatement(StatementBody);
    StatementBody->setParent(IfStat);

    return IfStat;
}

std::any ASTBuilderPass::visitIfElseConditional(GazpreaParser::IfElseConditionalContext *ctx) {
    auto IfElseStat = PM->Builder.build<ConditionalElse>();

    // Set the conditional expression
    auto CondExpr = castToNodeVisit(ctx->expr());
    IfElseStat->setConditional(CondExpr);
    CondExpr->setParent(IfElseStat);

    // Set the statement body
    auto StatementBody = castToNodeVisit(ctx->stmt(0));
    IfElseStat->setStatement(StatementBody);
    StatementBody->setParent(IfElseStat);

    // Set the else statement body
    auto ElseStatementBody = castToNodeVisit(ctx->stmt(1));
    IfElseStat->setElseStatement(ElseStatementBody);
    ElseStatementBody->setParent(IfElseStat);

    return IfElseStat;
}

std::any ASTBuilderPass::visitInfiniteLoop(GazpreaParser::InfiniteLoopContext *ctx) {
    auto Loop = PM->Builder.build<InfiniteLoop>();

    // Set the statement body
    auto StatementBody = castToNodeVisit(ctx->stmt());
    Loop->setStatement(StatementBody);
    StatementBody->setParent(Loop);

    return Loop;
}

std::any ASTBuilderPass::visitWhileLoop(GazpreaParser::WhileLoopContext *ctx) {
    auto Loop = PM->Builder.build<ConditionalLoop>();

    // Set the conditional expression
    auto CondExpr = castToNodeVisit(ctx->expr());
    Loop->setConditional(CondExpr);
    CondExpr->setParent(Loop);

    // Set the statement body
    auto StatementBody = castToNodeVisit(ctx->stmt());
    Loop->setStatement(StatementBody);
    StatementBody->setParent(Loop);

    return Loop;
}

std::any ASTBuilderPass::visitDoWhileLoop(GazpreaParser::DoWhileLoopContext *ctx) {
    auto Loop = PM->Builder.build<ConditionalLoop>();

    // Set the conditional expression
    auto CondExpr = castToNodeVisit(ctx->expr());
    Loop->setConditional(CondExpr);
    CondExpr->setParent(Loop);

    // Set the statement body
    auto StatementBody = castToNodeVisit(ctx->stmt());
    Loop->setStatement(StatementBody);
    StatementBody->setParent(Loop);

    // Set conditional after because this is a do-while loop
    Loop->setConditionalAfter();

    return Loop;
}

// Ignore for part1
std::any ASTBuilderPass::visitDomainLoop(GazpreaParser::DomainLoopContext *ctx) {

}

// Ignore for part1
std::any ASTBuilderPass::visitIterDomain(GazpreaParser::IterDomainContext *ctx) {

}


std::any ASTBuilderPass::visitTypeDef(GazpreaParser::TypeDefContext *ctx) {
    auto TypeCasting = PM->Builder.build<TypeCast>();

    // Set the old type
    auto OldType = castToNodeVisit(ctx->type());
    TypeCasting->setOldTypeNode(OldType);
    OldType->setParent(TypeCasting);

    // Set the new type
    auto NewType = PM->Builder.build<Identifier>();
    NewType->setName(ctx->ID()->getText());
    TypeCasting->setNewTypeNode(NewType);
    NewType->setParent(TypeCasting);

    return TypeCasting;
}

std::any ASTBuilderPass::visitOutput(GazpreaParser::OutputContext *ctx) {

}

std::any ASTBuilderPass::visitInput(GazpreaParser::InputContext *ctx) {

}

std::any ASTBuilderPass::visitReturn(GazpreaParser::ReturnContext *ctx) {
    auto ReturnStatement = PM->Builder.build<Return>();

    // Set the returned expression
    auto ReturnExpr = castToNodeVisit(ctx->expr());
    ReturnStatement->setReturnExpr(ReturnExpr);
    ReturnExpr->setParent(ReturnStatement);

    return ReturnStatement;
}

std::any ASTBuilderPass::visitResolvedType(GazpreaParser::ResolvedTypeContext *ctx) {
    auto ResolvedType = PM->Builder.build<Identifier>();
    ResolvedType->setName(ctx->ID()->getText());

    return ResolvedType;
}

// Ignore for part1
std::any ASTBuilderPass::visitVectorType(GazpreaParser::VectorTypeContext *ctx) {

}

// Ignore for part1
std::any ASTBuilderPass::visitMatrixType(GazpreaParser::MatrixTypeContext *ctx) {

}

// Remains to be done
std::any ASTBuilderPass::visitIntType(GazpreaParser::IntTypeContext *ctx) {

}

// Remains to be done
std::any ASTBuilderPass::visitCharType(GazpreaParser::CharTypeContext *ctx) {

}

// Remains to be done
std::any ASTBuilderPass::visitBooleanType(GazpreaParser::BooleanTypeContext *ctx) {

}

// Remains to be done
std::any ASTBuilderPass::visitRealType(GazpreaParser::RealTypeContext *ctx) {

}

// Ignore for part1
std::any ASTBuilderPass::visitExpressionOrWildcard(GazpreaParser::ExpressionOrWildcardContext *ctx) {

}

std::any ASTBuilderPass::visitTupleTypeDecl(GazpreaParser::TupleTypeDeclContext *ctx) {
    auto TupleTypeDeclaration = PM->Builder.build<TupleTypeDecl>();

    for (auto *Decl : ctx->typeOptionalIdentPair()) {
        auto Child = castToNodeVisit(Decl);
        TupleTypeDeclaration->addChild(Child);
        Child->setParent(TupleTypeDeclaration);
    }

    return TupleTypeDeclaration;
}

std::any ASTBuilderPass::visitTypeOptionalIdentPair(GazpreaParser::TypeOptionalIdentPairContext *ctx) {
    auto Decl = PM->Builder.build<Declaration>();

    // Mark the decl as const if we see a const type qualifier.
    if (ctx->typeQualifier()->CONST())
        Decl->setConst();

    // Set the type node
    auto TypeNode = castToNodeVisit(ctx->type());
    Decl->setIdentTypeNode(TypeNode);
    TypeNode->setParent(Decl);

    // Set the identifier node to null if it is not specified.
    if (!ctx->ID())
        Decl->setIdent(nullptr);
    else {
        auto Ident = PM->Builder.build<Identifier>(Decl);
        Ident->setName(ctx->ID()->getText());
        Decl->setIdent(Ident);
        Ident->setParent(Decl);
    }

    // No expression for this decl
    Decl->setInitExpr(nullptr);

    return Decl;
}

std::any ASTBuilderPass::visitTypeIdentPair(GazpreaParser::TypeIdentPairContext *ctx) {
    auto Decl = PM->Builder.build<Declaration>();

    // Mark the decl as const if we see a const type qualifier.
    if (ctx->typeQualifier()->CONST())
        Decl->setConst();

    // Set the type node
    auto TypeNode = castToNodeVisit(ctx->type());
    Decl->setIdentTypeNode(TypeNode);
    TypeNode->setParent(Decl);

    // Set the identifier node
    auto Ident = PM->Builder.build<Identifier>(Decl);
    Ident->setName(ctx->ID()->getText());
    Decl->setIdent(Ident);
    Ident->setParent(Decl);

    // No expression for this decl
    Decl->setInitExpr(nullptr);

    return Decl;
}

std::any ASTBuilderPass::visitFunctionDeclr(GazpreaParser::FunctionDeclrContext *ctx) {
    auto FuncDecl = PM->Builder.build<FunctionDecl>();

    // Set the identifier node
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setName(ctx->ID()->getText());
    FuncDecl->setIdent(Ident);
    Ident->setParent(FuncDecl);

    // Set parameters list
    auto ParametersList = PM->Builder.build<ParasList>();
    for (auto *Decl : ctx->typeOptionalIdentPair()) {
        auto Declaration = castToNodeVisit(Decl);
        ParametersList->addChild(Declaration);
        Declaration->setParent(ParametersList);
    }
    FuncDecl->setParasList(ParametersList);
    ParametersList->setParent(FuncDecl);

    // Set returns type node
    auto ReturnsTypeNode = castToNodeVisit(ctx->type());
    FuncDecl->setReturnsType(ReturnsTypeNode);
    ReturnsTypeNode->setParent(FuncDecl);

    return FuncDecl;
}

std::any ASTBuilderPass::visitFunctionDefinition(GazpreaParser::FunctionDefinitionContext *ctx) {
    auto FuncDef = PM->Builder.build<FunctionDef>();

    // Set the identifier node
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setName(ctx->ID()->getText());
    FuncDef->setIdent(Ident);
    Ident->setParent(FuncDef);

    // Set parameters list
    auto ParametersList = PM->Builder.build<ParasList>();
    for (auto *Decl : ctx->typeIdentPair()) {
        auto Declaration = castToNodeVisit(Decl);
        ParametersList->addChild(Declaration);
        Declaration->setParent(ParametersList);
    }
    FuncDef->setParasList(ParametersList);
    ParametersList->setParent(FuncDef);

    // Set returns type node
    auto ReturnsTypeNode = castToNodeVisit(ctx->type());
    FuncDef->setReturnsType(ReturnsTypeNode);
    ReturnsTypeNode->setParent(FuncDef);

    // Set expression if it is not null
    if (ctx->expr()) {
        auto Expr = castToNodeVisit(ctx->expr());
        FuncDef->setExpr(Expr);
        Expr->setParent(FuncDef);
    }

    // Set block if it is not null
    if (ctx->block()) {
        auto Block = castToNodeVisit(ctx->block());
        FuncDef->setBlock(Block);
        Block->setParent(FuncDef);
    }

    return FuncDef;
}

std::any ASTBuilderPass::visitProcedureDeclr(GazpreaParser::ProcedureDeclrContext *ctx) {
    auto ProcedDecl = PM->Builder.build<ProcedureDecl>();

    // Set the identifier node
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setName(ctx->ID()->getText());
    ProcedDecl->setIdent(Ident);
    Ident->setParent(ProcedDecl);

    // Set parameters list
    auto ParametersList = PM->Builder.build<ParasList>();
    for (auto *Decl : ctx->typeOptionalIdentPair()) {
        auto Declaration = castToNodeVisit(Decl);
        ParametersList->addChild(Declaration);
        Declaration->setParent(ParametersList);
    }
    ProcedDecl->setParasList(ParametersList);
    ParametersList->setParent(ProcedDecl);

    // Set returns type to null if it is not specified
    if (!ctx->type())
        ProcedDecl->setReturnsType(nullptr);
    else {
        auto ReturnsTypeNode = castToNodeVisit(ctx->type());
        ProcedDecl->setReturnsType(ReturnsTypeNode);
        ReturnsTypeNode->setParent(ProcedDecl);
    }

    return ProcedDecl;
}

std::any ASTBuilderPass::visitProcedureDefinition(GazpreaParser::ProcedureDefinitionContext *ctx) {
    auto ProcedDef = PM->Builder.build<ProcedureDef>();

    // Set the identifier node
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setName(ctx->ID()->getText());
    ProcedDef->setIdent(Ident);
    Ident->setParent(ProcedDef);

    // Set parameters list
    auto ParametersList = PM->Builder.build<ParasList>();
    for (auto *Decl : ctx->typeIdentPair()) {
        auto Declaration = castToNodeVisit(Decl);
        ParametersList->addChild(Declaration);
        Declaration->setParent(ParametersList);
    }
    ProcedDef->setParasList(ParametersList);
    ParametersList->setParent(ProcedDef);

    // Set returns type node
    auto ReturnsTypeNode = castToNodeVisit(ctx->type());
    ProcedDef->setReturnsType(ReturnsTypeNode);
    ReturnsTypeNode->setParent(ProcedDef);

    // Set block if it is not null
    auto Block = castToNodeVisit(ctx->block());
    ProcedDef->setBlock(Block);
    Block->setParent(ProcedDef);

    return ProcedDef;
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
    return PM->Builder.build<IdentityLiteral>();
}

std::any ASTBuilderPass::visitMemberAccess(GazpreaParser::MemberAccessContext *ctx) {

}

std::any ASTBuilderPass::visitIdentifier(GazpreaParser::IdentifierContext *ctx) {
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setName(ctx->getText());
    return Ident;
}

std::any ASTBuilderPass::visitNullLiteral(GazpreaParser::NullLiteralContext *ctx) {
    return PM->Builder.build<NullLiteral>();
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
