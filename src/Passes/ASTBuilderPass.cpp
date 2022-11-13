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
    assert(isa<Program>(Root) && "The Root must be a program");
    visit(File);
}

std::any ASTBuilderPass::visitFile(GazpreaParser::FileContext *ctx) {
    for (auto Child : ctx->global())
        Prog->addChild(castToNodeVisit(Child));
    return nullptr;
}

std::any ASTBuilderPass::visitSimpleStmt(GazpreaParser::SimpleStmtContext *ctx) {
    // If it is a break statement
    if (ctx->BREAK())
        return PM->Builder.build<Break>();

    // If it is a continue statement
    if (ctx->CONTINUE())
        return PM->Builder.build<Continue>();

    for (auto *Child : ctx->children)
        if (!Child) return castToNodeVisit(Child);
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
    Decl->setIdent(Ident);

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

    auto *Expr = castToNodeVisit(ctx->expr());
    Assign->setExpr(Expr);

    return Assign;
}

std::any ASTBuilderPass::visitIfConditional(GazpreaParser::IfConditionalContext *ctx) {
    auto IfStat = PM->Builder.build<Conditional>();

    // Set the conditional expression
    auto CondExpr = castToNodeVisit(ctx->expr());
    IfStat->setConditional(CondExpr);

    // Set the statement body
    auto StatementBody = castToNodeVisit(ctx->stmt());
    IfStat->setStatement(StatementBody);

    return IfStat;
}

std::any ASTBuilderPass::visitIfElseConditional(GazpreaParser::IfElseConditionalContext *ctx) {
    auto IfElseStat = PM->Builder.build<ConditionalElse>();

    // Set the conditional expression
    auto CondExpr = castToNodeVisit(ctx->expr());
    IfElseStat->setConditional(CondExpr);

    // Set the statement body
    auto StatementBody = castToNodeVisit(ctx->stmt(0));
    IfElseStat->setStatement(StatementBody);

    // Set the else statement body
    auto ElseStatementBody = castToNodeVisit(ctx->stmt(1));
    IfElseStat->setElseStatement(ElseStatementBody);

    return IfElseStat;
}

std::any ASTBuilderPass::visitInfiniteLoop(GazpreaParser::InfiniteLoopContext *ctx) {
    auto Loop = PM->Builder.build<InfiniteLoop>();

    // Set the statement body
    auto StatementBody = castToNodeVisit(ctx->stmt());
    Loop->setStatement(StatementBody);

    return Loop;
}

std::any ASTBuilderPass::visitWhileLoop(GazpreaParser::WhileLoopContext *ctx) {
    auto Loop = PM->Builder.build<ConditionalLoop>();

    // Set the conditional expression
    auto CondExpr = castToNodeVisit(ctx->expr());
    Loop->setConditional(CondExpr);

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
    return nullptr;
}

// Ignore for part1
std::any ASTBuilderPass::visitIterDomain(GazpreaParser::IterDomainContext *ctx) {
    return nullptr;
}


std::any ASTBuilderPass::visitTypeDef(GazpreaParser::TypeDefContext *ctx) {
    auto TypeDefNode = PM->Builder.build<TypeDef>();

    auto BaseType = castToNodeVisit(ctx->type());
    TypeDefNode->setBaseType(BaseType);

    auto NewType = PM->Builder.build<Identifier>();
    NewType->setName(ctx->ID()->getText());
    TypeDefNode->setAlias(NewType);

    return TypeDefNode;
}


std::any ASTBuilderPass::visitOutput(GazpreaParser::OutputContext *ctx) {
    auto Output = PM->Builder.build<OutStream>();
    auto Expr = castToNodeVisit(ctx->expr());
    Output->setOutStreamExpr(Expr);
    return Output;
}


std::any ASTBuilderPass::visitInput(GazpreaParser::InputContext *ctx) {
    auto Input = PM->Builder.build<InStream>();
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setName(ctx->ID()->getText());
    Input->setIdentifier(Ident);
    
    return Input;
}

std::any ASTBuilderPass::visitReturn(GazpreaParser::ReturnContext *ctx) {
    auto ReturnStatement = PM->Builder.build<Return>();

    // Set the returned expression
    auto ReturnExpr = castToNodeVisit(ctx->expr());
    ReturnStatement->setReturnExpr(ReturnExpr);

    return ReturnStatement;
}

std::any ASTBuilderPass::visitResolvedType(GazpreaParser::ResolvedTypeContext *ctx) {
    auto Resolved = PM->Builder.build<ResolvedType>();
    Resolved->setName(ctx->ID()->getText());
    return Resolved;
}

// Ignore for part1
std::any ASTBuilderPass::visitVectorType(GazpreaParser::VectorTypeContext *ctx) {
    return nullptr;
}

// Ignore for part1
std::any ASTBuilderPass::visitMatrixType(GazpreaParser::MatrixTypeContext *ctx) {
    return nullptr;
}

// Remains to be done
std::any ASTBuilderPass::visitIntType(GazpreaParser::IntTypeContext *ctx) {
    return PM->Builder.build<IntegerTypeNode>();
}

// Remains to be done
std::any ASTBuilderPass::visitCharType(GazpreaParser::CharTypeContext *ctx) {
    return PM->Builder.build<CharTypeNode>();
}

// Remains to be done
std::any ASTBuilderPass::visitBooleanType(GazpreaParser::BooleanTypeContext *ctx) {
    return PM->Builder.build<BoolTypeNode>();
}

// Remains to be done
std::any ASTBuilderPass::visitRealType(GazpreaParser::RealTypeContext *ctx) {
    return PM->Builder.build<RealTypeNode>();
}

// Ignore for part1
std::any ASTBuilderPass::visitExpressionOrWildcard(GazpreaParser::ExpressionOrWildcardContext *ctx) {
    return nullptr;
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
    Ident->setName(ctx->funcName->getText());
    FuncDecl->setIdent(Ident);

    // Set parameters list
    auto Params = PM->Builder.build<ParameterList>();
    for (auto *Param : ctx->typeOptionalIdentPair()) {
        auto CalleeParam = PM->Builder.build<CalleeParameter>();
        auto ParamTypeNode = castToNodeVisit(Param->type());
        CalleeParam->setTypeNode(ParamTypeNode);
        auto ParamIdent = PM->Builder.build<Identifier>();
        Params->addParam(CalleeParam);
    }
    FuncDecl->setParameterList(Params);

    // Set returns type node.
    auto ReturnsTypeNode = castToNodeVisit(ctx->type());
    FuncDecl->setReturnsTypeNode(ReturnsTypeNode);

    return FuncDecl;
}

std::any ASTBuilderPass::visitFunctionDefinition(GazpreaParser::FunctionDefinitionContext *ctx) {
    auto FuncDef = PM->Builder.build<FunctionDef>();

    // Set the identifier node
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setName(ctx->ID()->getText());
    FuncDef->setIdent(Ident);

    // Set parameters list
    auto ParamList = PM->Builder.build<ParameterList>();
    for (auto *Param : ctx->typeIdentPair()) {
        auto CalleeParam = PM->Builder.build<CalleeParameter>();
        auto ParamTypeNode = castToNodeVisit(Param->type());
        CalleeParam->setTypeNode(ParamTypeNode);
        auto ParamIdent = PM->Builder.build<Identifier>();
        ParamIdent->setName(Param->ID()->getText());
        CalleeParam->setConst();
        ParamList->addParam(CalleeParam);
    }
    FuncDef->setParamList(ParamList);

    // Set returns type node
    auto ReturnsTypeNode = castToNodeVisit(ctx->type());
    FuncDef->setReturnsType(ReturnsTypeNode);

    // If it is an expression type definition, we simplify it to block type.
    if (ctx->expr()) {
        auto FuncBody = PM->Builder.build<Block>();
        auto RetStmt = PM->Builder.build<Return>();
        RetStmt->setReturnExpr(castToNodeVisit(ctx->expr()));
        FuncBody->addChild(RetStmt);
        FuncDef->setBlock(FuncBody);
        return FuncDef;
    }

    FuncDef->setBlock(castToNodeVisit(ctx->block()));

    return FuncDef;
}

std::any ASTBuilderPass::visitProcedureDeclr(GazpreaParser::ProcedureDeclrContext *ctx) {
    auto ProcedDecl = PM->Builder.build<ProcedureDecl>();

    // Set the identifier node
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setName(ctx->ID()->getText());
    ProcedDecl->setIdent(Ident);

    auto Params = PM->Builder.build<ParameterList>();
    for (auto *Param : ctx->typeOptionalIdentPair()) {
        auto CalleeParam = PM->Builder.build<CalleeParameter>();
        auto ParamTypeNode = castToNodeVisit(Param->type());
        CalleeParam->setTypeNode(ParamTypeNode);
        auto ParamIdent = PM->Builder.build<Identifier>();
        Params->addParam(CalleeParam);
    }

    ProcedDecl->setParameterList(Params);

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

    // Set parameters list
    auto ParamList = PM->Builder.build<ParameterList>();
    for (auto *Param : ctx->typeIdentPair()) {
        auto CalleeParam = PM->Builder.build<CalleeParameter>();
        auto ParamTypeNode = castToNodeVisit(Param->type());
        CalleeParam->setTypeNode(ParamTypeNode);
        auto ParamIdent = PM->Builder.build<Identifier>();
        ParamIdent->setName(Param->ID()->getText());
        CalleeParam->setConst();
        ParamList->addParam(CalleeParam);
    }

    ProcedDef->setParameterList(ParamList);

    // Set block if it is not null
    auto Block = castToNodeVisit(ctx->block());
    ProcedDef->setBlock(Block);
    Block->setParent(ProcedDef);

    // Set returns type to null if it is not specified
    if (!ctx->type())
        ProcedDef->setReturnsType(nullptr);
    else {
        auto ReturnsTypeNode = castToNodeVisit(ctx->type());
        ProcedDef->setReturnsType(ReturnsTypeNode);
        ReturnsTypeNode->setParent(ProcedDef);
    }

    return ProcedDef;
}

std::any ASTBuilderPass::visitFunctionCall(GazpreaParser::FunctionCallContext *ctx) {
    auto FuncCall = PM->Builder.build<FunctionCall>();

    // Set the identifier node
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setName(ctx->ID()->getText());
    FuncCall->setIdent(Ident);
    Ident->setParent(FuncCall);

    // Set arguments list
    auto ArgumentsList = PM->Builder.build<ArgsList>();
    for (auto *Child : ctx->expr()) {
        auto Expr = castToNodeVisit(Child);
        ArgumentsList->addChild(Expr);
        Expr->setParent(ArgumentsList);
    }
    FuncCall->setArgsList(ArgumentsList);
    ArgumentsList->setParent(FuncCall);

    return FuncCall;
}

std::any ASTBuilderPass::visitProcedureCall(GazpreaParser::ProcedureCallContext *ctx) {
    auto ProcedCall = PM->Builder.build<ProcedureCall>();

    // Set the identifier node
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setName(ctx->ID()->getText());
    ProcedCall->setIdent(Ident);
    Ident->setParent(ProcedCall);

    // Set arguments list
    auto ArgumentsList = PM->Builder.build<ArgsList>();
    for (auto *Child : ctx->expr()) {
        auto Expr = castToNodeVisit(Child);
        ArgumentsList->addChild(Expr);
        Expr->setParent(ArgumentsList);
    }
    ProcedCall->setArgsList(ArgumentsList);
    ArgumentsList->setParent(ProcedCall);

    return ProcedCall;
}

std::any ASTBuilderPass::visitBlock(GazpreaParser::BlockContext *ctx) {
    auto *Blk = PM->Builder.build<Block>();

    for (auto *Stmt : ctx->stmt()) {
        auto Statement = castToNodeVisit(Stmt);
        Blk->addChild(Statement);
        Statement->setParent(Blk);
    }

    return Blk;
}

// Remains to be done
std::any ASTBuilderPass::visitExplicitCast(GazpreaParser::ExplicitCastContext *ctx) {
    auto *ExpliCast = PM->Builder.build<ExplicitCast>();

    // Set the type node
    auto Type = castToNodeVisit(ctx->type());
    ExpliCast->setType(Type);
    Type->setParent(ExpliCast);

    // Set expression
    auto Expr = castToNodeVisit(ctx->expr());
    ExpliCast->setExpr(Expr);
    Expr->setParent(ExpliCast);

    return ExpliCast;
}

std::any ASTBuilderPass::visitBoolLiteral(GazpreaParser::BoolLiteralContext *ctx) {
    auto *BoolLit = PM->Builder.build<BoolLiteral>();

    if (ctx->TRUE())
        BoolLit->setTrue();
    else if (ctx->FALSE())
        BoolLit->setFalse();

    return BoolLit;
}

std::any ASTBuilderPass::visitUnaryExpr(GazpreaParser::UnaryExprContext *ctx) {
    auto *UnaryExpr = PM->Builder.build<UnaryOp>();

    // Set the operator
    if (ctx->op->getText() == "+")
        UnaryExpr->setOp(UnaryOp::ADD);
    else if (ctx->op->getText() == "-")
        UnaryExpr->setOp(UnaryOp::SUB);
    else if (ctx->op->getText() == "not")
        UnaryExpr->setOp(UnaryOp::NOT);

    // Set the expression
    auto Expr = castToNodeVisit(ctx->expr());
    UnaryExpr->setExpr(Expr);
    Expr->setParent(UnaryExpr);

    return UnaryExpr;
}

// Ignore for part1
std::any ASTBuilderPass::visitGeneratorExpr(GazpreaParser::GeneratorExprContext *ctx) {
    return nullptr;
}

std::any ASTBuilderPass::visitExpExpr(GazpreaParser::ExpExprContext *ctx) {
    auto ExpExpression = PM->Builder.build<ArithmeticOp>();

    // Set the operator
    if (ctx->op->getText() == "^")
        ExpExpression->setOp(ArithmeticOp::EXP);

    // Set the LeftExpression
    auto LeftExpression = castToNodeVisit(ctx->expr(0));
    ExpExpression->setLeftExpr(LeftExpression);
    LeftExpression->setParent(ExpExpression);

    // Set the RightExpression
    auto RightExpression = castToNodeVisit(ctx->expr(1));
    ExpExpression->setRightExpr(RightExpression);
    RightExpression->setParent(ExpExpression);

    return ExpExpression;
}

std::any ASTBuilderPass::visitCompExpr(GazpreaParser::CompExprContext *ctx) {
    auto CompExpression = PM->Builder.build<LogicalOp>();

    // Set the operator
    if (ctx->op->getText() == "<")
        CompExpression->setOp(LogicalOp::LT);
    else if (ctx->op->getText() == ">")
        CompExpression->setOp(LogicalOp::GT);
    else if (ctx->op->getText() == "<=")
        CompExpression->setOp(LogicalOp::LTEQ);
    else if (ctx->op->getText() == ">=")
        CompExpression->setOp(LogicalOp::GTEQ);

    // Set the LeftExpression
    auto LeftExpression = castToNodeVisit(ctx->expr(0));
    CompExpression->setLeftExpr(LeftExpression);
    LeftExpression->setParent(CompExpression);

    // Set the RightExpression
    auto RightExpression = castToNodeVisit(ctx->expr(1));
    CompExpression->setRightExpr(RightExpression);
    RightExpression->setParent(CompExpression);

    return CompExpression;
}

std::any ASTBuilderPass::visitIdentityLiteral(GazpreaParser::IdentityLiteralContext *ctx) {
    return PM->Builder.build<IdentityLiteral>();
}

std::any ASTBuilderPass::visitMemberAccess(GazpreaParser::MemberAccessContext *ctx) {
    auto MemberAcc = PM->Builder.build<MemberAccess>();

    // Set the identifier
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setName(ctx->ID(0)->getText());
    MemberAcc->setIdent(Ident);
    MemberAcc->setParent(Ident);

    // Set the member access
    if (ctx->ID(1)) {
        auto IdentExpr = PM->Builder.build<Identifier>();
        Ident->setName(ctx->ID(1)->getText());
        MemberAcc->setMemberExpr(IdentExpr);
        IdentExpr->setParent(MemberAcc);
    }
    else if (ctx->INTLITERAL()) {
        auto IntegerLit = PM->Builder.build<IntLiteral>();
        IntegerLit->setVal(ctx->INTLITERAL()->getText());
        MemberAcc->setMemberExpr(IntegerLit);
        IntegerLit->setParent(MemberAcc);
    }

    return MemberAcc;
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
    auto AddSubExpression = PM->Builder.build<ArithmeticOp>();

    // Set operator
    if (ctx->op->getText() == "+")
        AddSubExpression->setOp(ArithmeticOp::ADD);
    else if (ctx->op->getText() == "-")
        AddSubExpression->setOp(ArithmeticOp::SUB);

    // Set LeftExpression
    auto LeftExpression = castToNodeVisit(ctx->expr(0));
    AddSubExpression->setLeftExpr(LeftExpression);
    LeftExpression->setParent(AddSubExpression);

    // Set RightExpression
    auto RightExpression = castToNodeVisit(ctx->expr(1));
    AddSubExpression->setRightExpr(RightExpression);
    RightExpression->setParent(AddSubExpression);

    return AddSubExpression;
}

std::any ASTBuilderPass::visitBracketExpr(GazpreaParser::BracketExprContext *ctx) {
    return visit(ctx->expr());
}

std::any ASTBuilderPass::visitRealLiteral(GazpreaParser::RealLiteralContext *ctx) {
    return visit(ctx->realLit());
}

std::any ASTBuilderPass::visitIntLiteral(GazpreaParser::IntLiteralContext *ctx) {
    auto IntegerLit = PM->Builder.build<IntLiteral>();
    IntegerLit->setVal(ctx->INTLITERAL()->getText());

    return IntegerLit;
}

std::any ASTBuilderPass::visitMulDivModSSExpr(GazpreaParser::MulDivModSSExprContext *ctx) {
    auto MulDivModSSExpression = PM->Builder.build<ArithmeticOp>();

    // Set operator
    if (ctx->op->getText() == "*")
        MulDivModSSExpression->setOp(ArithmeticOp::MUL);
    else if (ctx->op->getText() == "/")
        MulDivModSSExpression->setOp(ArithmeticOp::DIV);
    else if (ctx->op->getText() == "%")
        MulDivModSSExpression->setOp(ArithmeticOp::MOD);
    else if (ctx->op->getText() == "**")
        MulDivModSSExpression->setOp(ArithmeticOp::SS);

    // Set left expression
    auto LeftExpression = castToNodeVisit(ctx->expr(0));
    MulDivModSSExpression->setLeftExpr(LeftExpression);
    LeftExpression->setParent(MulDivModSSExpression);

    // Set right expression
    auto RightExpression = castToNodeVisit(ctx->expr(1));
    MulDivModSSExpression->setRightExpr(RightExpression);
    RightExpression->setParent(MulDivModSSExpression);

    return MulDivModSSExpression;
}

// ignored for part1
std::any ASTBuilderPass::visitByExpr(GazpreaParser::ByExprContext *ctx) {
    return nullptr;
}

std::any ASTBuilderPass::visitOrExpr(GazpreaParser::OrExprContext *ctx) {
    auto OrExpr = PM->Builder.build<BitwiseOp>();

    // Set operator
    if (ctx->op->getText() == "or")
        OrExpr->setOp(BitwiseOp::OR);
    else if (ctx->op->getText() == "xor")
        OrExpr->setOp(BitwiseOp::XOR);

    // Set LeftExpression
    auto LeftExpression = castToNodeVisit(ctx->expr(0));
    OrExpr->setLeftExpr(LeftExpression);
    LeftExpression->setParent(OrExpr);

    // Set RightExpression
    auto RightExpression = castToNodeVisit(ctx->expr(1));
    OrExpr->setRightExpr(RightExpression);
    RightExpression->setParent(OrExpr);

    return OrExpr;
}


// ignored for part1
std::any ASTBuilderPass::visitFilterExpr(GazpreaParser::FilterExprContext *ctx) {
    return nullptr;
}


std::any ASTBuilderPass::visitCharLiteral(GazpreaParser::CharLiteralContext *ctx) {
    auto CharLit = PM->Builder.build<CharLiteral>();
    CharLit->setCharacter(ctx->CHARLITERAL()->getText());

    return CharLit;
}

// ignored for part1
std::any ASTBuilderPass::visitIndexExpr(GazpreaParser::IndexExprContext *ctx) {
    return nullptr;
}


std::any ASTBuilderPass::visitTupleLiteral(GazpreaParser::TupleLiteralContext *ctx) {
    auto TupleLit = PM->Builder.build<TupleLiteral>();

    for (auto *Child : ctx->expr()) {
        auto Expr = castToNodeVisit(Child);
        TupleLit->addChild(Expr);
        Expr->setParent(TupleLit);
    }

    return TupleLit;
}

// ignored for part1
std::any ASTBuilderPass::visitAppendOp(GazpreaParser::AppendOpContext *ctx) {
    return nullptr;
}

std::any ASTBuilderPass::visitFuncCall(GazpreaParser::FuncCallContext *ctx) {
    return visit(ctx->functionCall());
}

// ignored for part1
std::any ASTBuilderPass::visitRangeExpr(GazpreaParser::RangeExprContext *ctx) {
    return nullptr;
}


std::any ASTBuilderPass::visitEqualExpr(GazpreaParser::EqualExprContext *ctx) {
    auto EqualExpr = PM->Builder.build<LogicalOp>();

    // Set operator
    if (ctx->op->getText() == "==")
        EqualExpr->setOp(LogicalOp::EQEQ);
    else if (ctx->op->getText() == "!=")
        EqualExpr->setOp(LogicalOp::NEQ);

    // Set left expression
    auto LeftExpression = castToNodeVisit(ctx->expr(0));
    EqualExpr->setLeftExpr(LeftExpression);
    LeftExpression->setParent(EqualExpr);

    // Set right expression
    auto RightExpression = castToNodeVisit(ctx->expr(1));
    EqualExpr->setRightExpr(RightExpression);
    RightExpression->setParent(EqualExpr);

    return EqualExpr;
}

std::any ASTBuilderPass::visitAndExpr(GazpreaParser::AndExprContext *ctx) {
    auto AndExpr = PM->Builder.build<BitwiseOp>();

    // Set operator
    if (ctx->op->getText() == "and")
        AndExpr->setOp(BitwiseOp::AND);

    // Set LeftExpression
    auto LeftExpression = castToNodeVisit(ctx->expr(0));
    AndExpr->setLeftExpr(LeftExpression);
    LeftExpression->setParent(AndExpr);

    // Set RightExpression
    auto RightExpression = castToNodeVisit(ctx->expr(1));
    AndExpr->setRightExpr(RightExpression);
    RightExpression->setParent(AndExpr);

    return AndExpr;
}

std::any ASTBuilderPass::visitSciRealLiteral(GazpreaParser::SciRealLiteralContext *ctx) {
    auto RealLit = PM->Builder.build<RealLiteral>();
    auto *FullRealLit = cast<RealLiteral>(castToNodeVisit(ctx->fullRealLiteral()));

    string RealString = std::to_string(FullRealLit->getVal()) + "e" + ctx->INTLITERAL()->getText();
    RealLit->setVal(RealString);

    return RealLit;
}

std::any ASTBuilderPass::visitMainReal(GazpreaParser::MainRealContext *ctx) {
    auto RealLit = PM->Builder.build<RealLiteral>();
    string RealString = ctx->INTLITERAL(0)->getText() + "." + ctx->INTLITERAL(1)->getText();
    RealLit->setVal(RealString);

    return RealLit;
}

std::any ASTBuilderPass::visitIntReal(GazpreaParser::IntRealContext *ctx) {
    auto RealLit = PM->Builder.build<RealLiteral>();
    string RealString = ctx->INTLITERAL()->getText() + ".";
    RealLit->setVal(RealString);

    return RealLit;
}

std::any ASTBuilderPass::visitDotReal(GazpreaParser::DotRealContext *ctx) {
    auto RealLit = PM->Builder.build<RealLiteral>();
    string RealString = "." + ctx->INTLITERAL()->getText();
    RealLit->setVal(RealString);

    return RealLit;
}

std::any ASTBuilderPass::visitTupleType(GazpreaParser::TupleTypeContext *ctx) {
    auto Tuple = PM->Builder.build<TupleTypeDecl>();
    for (auto *Member : ctx->tupleTypeDecl()->typeOptionalIdentPair()) {
        Tuple->addChild(castToNodeVisit(Member));
        Member->
    }
    return Tuple;
}
