//
// Created by dhanrajbir on 05/11/22.
//

#include "Passes/ASTBuilderPass.h"
#include "llvm/Support/Casting.h"
#include "Types/CompositeTypes.h"
#include "Types/Type.h"
#include "Passes/ScopeResolutionPass.h"

using llvm::cast;
using llvm::isa;
using llvm::dyn_cast;

void ASTBuilderPass::runOnAST(ASTPassManager &Manager, ASTNodeT *Root) {
    PM = &Manager;
    assert(isa<Program>(Root) && "Builder must have the program"
                                 " node passed in as Root");
    Prog = cast<Program>(Root);
    Manager.setResource<ScopeTreeNode>(ScopeTreeNode());
    assert(isa<Program>(Root) && "The Root must be a program");
    visit(File);
}

std::any ASTBuilderPass::visitFile(GazpreaParser::FileContext *ctx) {
    for (auto Child : ctx->global())
        Prog->addChild(castToNodeVisit(Child));
    return nullptr;
}

std::any ASTBuilderPass::visitIdentDecl(GazpreaParser::IdentDeclContext *ctx) {
    auto Decl = PM->Builder.build<Declaration>();
    Decl->setCtx(ctx);
    assert((ctx->typeQualifier() || ctx->type()) && "At least one is needed");

    bool IsConst = (ctx->typeQualifier() && ctx->typeQualifier()->CONST());

    if (IsConst)
        Decl->setConst();

    // If the type is known, we set it.
    if (ctx->type()) {
        auto DeclType = castToTypeVisit(ctx->type());
        if (IsConst)
            DeclType = PM->TypeReg.getConstTypeOf(DeclType);
        Decl->setIdentType(DeclType);
    }

    auto Ident = PM->Builder.build<Identifier>();
    Ident->setCtx(ctx);
    Ident->setName(ctx->ID()->getText());
    Ident->setIdentType(Decl->getIdentType());
    Decl->setIdent(Ident);

    // If there is an init expression, we set it. Else we insert an initialization
    // with null.
    if (!ctx->expr()) {
        auto NullLit = PM->Builder.build<NullLiteral>();
        NullLit->setCtx(ctx);
        Decl->setInitExpr(NullLit);

        if (!Decl->getIdentType())
            throw NullDeclarationInferenceError(Decl);

        auto Null = PM->Builder.build<NullLiteral>();
        Null->setCtx(ctx);
        auto Cast = PM->Builder.build<TypeCast>();
        Cast->setCtx(ctx);
        Cast->setTargetType(Decl->getIdentType());
        Cast->setExpr(Null);
        Decl->setInitExpr(Cast);
    } else
        Decl->setInitExpr(castToNodeVisit(ctx->expr()));

    return cast<ASTNodeT>(Decl);
}

std::any ASTBuilderPass::visitAssignment(GazpreaParser::AssignmentContext *ctx) {
    auto Expr = castToNodeVisit(ctx->expr());
    auto AssignedTo = castToNodeVisit(ctx->lvalue());
    auto Assign = PM->Builder.build<Assignment>();
    Assign->setAssignedTo(AssignedTo);
    Assign->setExpr(Expr);
    Assign->setCtx(ctx);
    return cast<ASTNodeT>(Assign);
}

std::any ASTBuilderPass::visitIfConditional(GazpreaParser::IfConditionalContext *ctx) {
    auto IfStat = PM->Builder.build<Conditional>();
    IfStat->setCtx(ctx);

    // Set the conditional expression
    auto CondExpr = castToNodeVisit(ctx->expr());
    IfStat->setConditional(CondExpr);

    // Set the statement body
    auto StatementBody = castToNodeVisit(ctx->stmt());
    if (!isa<Block>(StatementBody)) {
        auto CondBody = PM->Builder.build<Block>();
        CondBody->setCtx(ctx);
        CondBody->addChild(StatementBody);
        IfStat->setBlock(CondBody);
        return cast<ASTNodeT>(IfStat);
    }

    IfStat->setBlock(dyn_cast<Block>(StatementBody));
    return cast<ASTNodeT>(IfStat);
}

std::any ASTBuilderPass::visitIfElseConditional(GazpreaParser::IfElseConditionalContext *ctx) {
    auto IfElseStat = PM->Builder.build<ConditionalElse>();
    IfElseStat->setCtx(ctx);
    IfElseStat->setConditional(castToNodeVisit(ctx->expr()));

    auto IfBody = castToNodeVisit(ctx->stmt(0));
    if (!isa<Block>(IfBody)) {
        auto IfBodyBlock = PM->Builder.build<Block>();
        IfBodyBlock->setCtx(ctx);
        IfBodyBlock->addChild(IfBody);
        IfElseStat->setIfBlock(IfBodyBlock);
    } else
        IfElseStat->setIfBlock(cast<Block>(IfBody));

    auto ElseBody = castToNodeVisit(ctx->stmt(1));

    if (!isa<Block>(ElseBody)) {
        auto ElseBodyBlock = PM->Builder.build<Block>();
        ElseBodyBlock->setCtx(ctx);
        ElseBodyBlock->addChild(ElseBody);
        IfElseStat->setElseBlock(ElseBodyBlock);
    } else
        IfElseStat->setElseBlock(cast<Block>(ElseBody));

    return cast<ASTNodeT>(IfElseStat);
}

std::any ASTBuilderPass::visitInfiniteLoop(GazpreaParser::InfiniteLoopContext *ctx) {
    auto Loop = PM->Builder.build<InfiniteLoop>();
    Loop->setCtx(ctx);

    // Set the statement body
    auto LoopBody = castToNodeVisit(ctx->stmt());
    if (!isa<Block>(LoopBody)) {
        auto LoopBlock = PM->Builder.build<Block>();
        LoopBlock->setCtx(ctx);
        LoopBlock->addChild(LoopBody);
        Loop->setBlock(LoopBlock);
        return cast<ASTNodeT>(Loop);
    }
    Loop->setBlock(dyn_cast<Block>(LoopBody));
    return cast<ASTNodeT>(Loop);
}

std::any ASTBuilderPass::visitWhileLoop(GazpreaParser::WhileLoopContext *ctx) {
    auto Loop = PM->Builder.build<ConditionalLoop>();
    Loop->setCtx(ctx);

    // Set the conditional expression
    auto CondExpr = castToNodeVisit(ctx->expr());
    Loop->setConditional(CondExpr);

    // Set the statement body
    auto LoopBody = castToNodeVisit(ctx->stmt());
    if (!isa<Block>(LoopBody)) {
        auto LoopBlock = PM->Builder.build<Block>();
        LoopBlock->setCtx(ctx);
        LoopBlock->addChild(LoopBody);
        Loop->setBlock(LoopBlock);
        return cast<ASTNodeT>(Loop);
    }
    Loop->setBlock(dyn_cast<Block>(LoopBody));
    return cast<ASTNodeT>(Loop);
}

std::any ASTBuilderPass::visitDoWhileLoop(GazpreaParser::DoWhileLoopContext *ctx) {
    auto Loop = PM->Builder.build<ConditionalLoop>();
    Loop->setCtx(ctx);

    // Set the conditional expression
    auto CondExpr = castToNodeVisit(ctx->expr());
    Loop->setConditional(CondExpr);

    auto LoopBody = castToNodeVisit(ctx->stmt());
    if (!isa<Block>(LoopBody)) {
        auto LoopBlock = PM->Builder.build<Block>();
        LoopBlock->setCtx(ctx);
        LoopBlock->addChild(LoopBody);
        Loop->setBlock(LoopBlock);
        Loop->setConditionalAfter();
        return cast<ASTNodeT>(Loop);
    }

    Loop->setBlock(dyn_cast<Block>(LoopBody));

    // Set conditional after because this is a do-while loop
    Loop->setConditionalAfter();

    return cast<ASTNodeT>(Loop);
}

// Ignore for part1
std::any ASTBuilderPass::visitDomainLoop(GazpreaParser::DomainLoopContext *ctx) {
    throw std::runtime_error("Unimplemented");
}

// Ignore for part1
std::any ASTBuilderPass::visitIterDomain(GazpreaParser::IterDomainContext *ctx) {
    throw std::runtime_error("Unimplemented");
}


std::any ASTBuilderPass::visitTypeDef(GazpreaParser::TypeDefContext *ctx) {
    auto &GlobalScope = PM->getResource<ScopeTreeNode>();
    auto BaseType = castToTypeVisit(ctx->type());
    auto NewType = PM->SymTable.defineTypeSymbol(ctx->ID()->getText(), BaseType);
    GlobalScope.declareInScope(ctx->ID()->getText(), NewType);
    auto Value = PM->Builder.build<NoOp>();
    Value->setCtx(ctx);
    return cast<ASTNodeT>(Value);
}


std::any ASTBuilderPass::visitOutput(GazpreaParser::OutputContext *ctx) {
    auto Output = PM->Builder.build<OutStream>();
    Output->setCtx(ctx);
    auto Expr = castToNodeVisit(ctx->expr());
    Output->setOutStreamExpr(Expr);
    return cast<ASTNodeT>(Output);
}


std::any ASTBuilderPass::visitInput(GazpreaParser::InputContext *ctx) {
    auto Input = PM->Builder.build<InStream>();
    Input->setCtx(ctx);
    auto LValue = castToNodeVisit(ctx->lvalue());
    Input->setTarget(LValue);
    return cast<ASTNodeT>(Input);
}

std::any ASTBuilderPass::visitReturn(GazpreaParser::ReturnContext *ctx) {
    auto RetStmt = PM->Builder.build<Return>();
    RetStmt->setCtx(ctx);

    // Set the returned expression
    if (!ctx->expr()) {
        auto NoOpVal = PM->Builder.build<NoOp>();
        NoOpVal->setCtx(ctx);
        RetStmt->setReturnExpr(NoOpVal);
    }
    else
        RetStmt->setReturnExpr(castToNodeVisit(ctx->expr()));

    return cast<ASTNodeT>(RetStmt);
}

std::any ASTBuilderPass::visitResolvedType(GazpreaParser::ResolvedTypeContext *ctx) {
    auto &GlobalScope = PM->getResource<ScopeTreeNode>();
    auto ResolvedSym = GlobalScope.resolve(ctx->ID()->getText());
    auto text = ctx->ID()->getText();
    if (!ResolvedSym)
        throw std::runtime_error(text + " type not found.");
    auto TypeSym = dyn_cast<TypeSymbol>(ResolvedSym);
    if (!TypeSym)
        throw std::runtime_error(text + " is not a type.");
    return TypeSym->getType();
}

// Ignore for part1
std::any ASTBuilderPass::visitVectorType(GazpreaParser::VectorTypeContext *ctx) {
    throw std::runtime_error("Unimplemented");
}

// Ignore for part1
std::any ASTBuilderPass::visitMatrixType(GazpreaParser::MatrixTypeContext *ctx) {
    throw std::runtime_error("Unimplemented");
}

std::any ASTBuilderPass::visitIntType(GazpreaParser::IntTypeContext *ctx) {
    return PM->TypeReg.getIntegerTy(false);
}

// Remains to be done
std::any ASTBuilderPass::visitCharType(GazpreaParser::CharTypeContext *ctx) {
    return PM->TypeReg.getCharTy(false);
}

// Remains to be done
std::any ASTBuilderPass::visitBooleanType(GazpreaParser::BooleanTypeContext *ctx) {
    return PM->TypeReg.getBooleanTy(false);
}

// Remains to be done
std::any ASTBuilderPass::visitRealType(GazpreaParser::RealTypeContext *ctx) {
    return PM->TypeReg.getRealTy(false);
}

// Ignore for part1
std::any ASTBuilderPass::visitExpressionOrWildcard(GazpreaParser::ExpressionOrWildcardContext *ctx) {
    throw std::runtime_error("Unimplemented");
}

std::any ASTBuilderPass::visitFunctionDeclr(GazpreaParser::FunctionDeclrContext *ctx) {
    auto FuncDecl = PM->Builder.build<FunctionDecl>();
    FuncDecl->setCtx(ctx);

    // Set the identifier node
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setCtx(ctx);
    Ident->setName(ctx->funcName->getText());
    FuncDecl->setIdent(Ident);

    for (auto Param : ctx->functionParameter()) {
        auto ParamTy = castToTypeVisit(Param->type());
        FuncDecl->addParam(PM->TypeReg.getConstTypeOf(ParamTy));
    }

    auto FuncRetTy = PM->TypeReg.getConstTypeOf(castToTypeVisit(ctx->type()));
    FuncDecl->setRetTy(FuncRetTy);
    auto FuncType = PM->TypeReg.getFunctionType(FuncDecl->getParamTypes(), FuncRetTy);
    Ident->setIdentType(FuncType);
    return cast<ASTNodeT>(FuncDecl);
}

std::any ASTBuilderPass::visitFunctionDefinition(GazpreaParser::FunctionDefinitionContext *ctx) {
    auto FuncDef = PM->Builder.build<FunctionDef>();
    FuncDef->setCtx(ctx);

    // Set the identifier node
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setCtx(ctx);
    Ident->setName(ctx->ID()->getText());
    FuncDef->setIdent(Ident);


    vector<const Type*> ParamTypes;

    auto ParamList = PM->Builder.build<ParameterList>();
    ParamList->setCtx(ctx);
    for (auto Param : ctx->functionParameter()) {
        auto ParamType = PM->TypeReg.getConstTypeOf(
                castToTypeVisit(Param->type()));
        auto ParamIdent = PM->Builder.build<Identifier>();
        ParamIdent->setCtx(Param);
        ParamIdent->setIdentType(ParamType);
        ParamIdent->setName(Param->ID()->getText());
        ParamList->addParam(ParamIdent);
        ParamTypes.emplace_back(ParamType);
    }

    FuncDef->setParamList(ParamList);

    auto FuncRetTy = PM->TypeReg.getConstTypeOf(castToTypeVisit(ctx->type()));
    FuncDef->setRetTy(FuncRetTy);

    auto FuncTy = PM->TypeReg.getFunctionType(ParamTypes, FuncRetTy);
    FuncDef->getIdentifier()->setIdentType(FuncTy);

    // If the function is in expression format, we change it to block format.
    if (ctx->expr()) {
        auto RetStmt = PM->Builder.build<Return>();
        RetStmt->setCtx(ctx);
        RetStmt->setReturnExpr(castToNodeVisit(ctx->expr()));
        auto FuncBody = PM->Builder.build<Block>();
        FuncBody->setCtx(ctx);
        FuncBody->addChild(RetStmt);
        FuncDef->setBlock(FuncBody);
        return cast<ASTNodeT>(FuncDef);
    }

    FuncDef->setBlock(castToNodeVisit(ctx->block()));
    return cast<ASTNodeT>(FuncDef);
}

std::any ASTBuilderPass::visitProcedureDeclr(GazpreaParser::ProcedureDeclrContext *ctx) {
    auto ProcDecl = PM->Builder.build<ProcedureDecl>();
    ProcDecl->setCtx(ctx);

    // Set the identifier node
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setCtx(ctx);
    Ident->setName(ctx->procName->getText());
    ProcDecl->setIdent(Ident);

    for (auto Param : ctx->typeOptionalIdentPair()) {
        bool IsVar = (Param->typeQualifier() && Param->typeQualifier()->VAR());
        auto ParamTy = castToTypeVisit(Param->type());
        if (!IsVar)
            ParamTy = PM->TypeReg.getConstTypeOf(ParamTy);
        ProcDecl->addParamTy(ParamTy);
    }

    const Type *ProcRetTy{nullptr};
    if (ctx->type())
        ProcRetTy = PM->TypeReg.getConstTypeOf(castToTypeVisit(ctx->type()));

    ProcDecl->setRetTy(ProcRetTy);
    auto ProcType = PM->TypeReg.getProcedureType(ProcDecl->getParamTypes(), ProcRetTy);
    Ident->setIdentType(ProcType);

    return cast<ASTNodeT>(ProcDecl);
}

std::any ASTBuilderPass::visitProcedureDefinition(GazpreaParser::ProcedureDefinitionContext *ctx) {
    auto ProcDef = PM->Builder.build<ProcedureDef>();
    ProcDef->setCtx(ctx);

    // Set the identifier node
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setCtx(ctx);
    Ident->setName(ctx->ID()->getText());
    ProcDef->setIdent(Ident);

    vector<const Type*> ParamTypes;

    auto ParamList = PM->Builder.build<ParameterList>();
    ParamList->setCtx(ctx);
    for (auto Param : ctx->typeIdentPair()) {
        bool IsVar = (Param->typeQualifier() && Param->typeQualifier()->VAR());
        auto ParamType = castToTypeVisit(Param->type());
        if (!IsVar)
            ParamType = PM->TypeReg.getConstTypeOf(ParamType);
        auto ParamIdent = PM->Builder.build<Identifier>();
        ParamIdent->setCtx(Param);
        ParamIdent->setName(Param->ID()->getText());
        ParamIdent->setIdentType(ParamType);
        ParamList->addParam(ParamIdent);
        ParamTypes.emplace_back(ParamType);
    }

    ProcDef->setParamList(ParamList);
    ProcDef->setBlock(castToNodeVisit(ctx->block()));
    if (ctx->type())
        ProcDef->setRetTy(
                PM->TypeReg.getConstTypeOf(castToTypeVisit(ctx->type())));

    auto ProcTy = PM->TypeReg.getProcedureType(ParamTypes, ProcDef->getRetTy());
    ProcDef->getIdentifier()->setIdentType(ProcTy);

    return cast<ASTNodeT>(ProcDef);
}

std::any ASTBuilderPass::visitFunctionCall(GazpreaParser::FunctionCallContext *ctx) {
    auto FuncCall = PM->Builder.build<FunctionCall>();
    FuncCall->setCtx(ctx);

    // Set the identifier node
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setCtx(ctx);
    Ident->setName(ctx->ID()->getText());
    FuncCall->setIdent(Ident);

    // Set arguments list
    auto ArgumentsList = PM->Builder.build<ArgsList>();
    ArgumentsList->setCtx(ctx);
    for (auto *Child : ctx->expr())
        ArgumentsList->addChild(castToNodeVisit(Child));

    FuncCall->setArgsList(ArgumentsList);

    return cast<ASTNodeT>(FuncCall);
}

std::any ASTBuilderPass::visitProcedureCall(GazpreaParser::ProcedureCallContext *ctx) {
    auto ProcCall = PM->Builder.build<ProcedureCall>();
    ProcCall->setCtx(ctx);

    // Set the identifier node
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setCtx(ctx);
    Ident->setName(ctx->ID()->getText());
    ProcCall->setIdent(Ident);

    // Set arguments list
    auto ArgumentsList = PM->Builder.build<ArgsList>();
    ArgumentsList->setCtx(ctx);

    for (auto *Child : ctx->expr())
        ArgumentsList->addChild(castToNodeVisit(Child));
    ProcCall->setArgsList(ArgumentsList);

    return cast<ASTNodeT>(ProcCall);
}

std::any ASTBuilderPass::visitBlock(GazpreaParser::BlockContext *ctx) {
    auto *Blk = PM->Builder.build<Block>();
    Blk->setCtx(ctx);

    for (auto *Stmt : ctx->stmt())
        Blk->addChild(castToNodeVisit(Stmt));

    return cast<ASTNodeT>(Blk);
}

// Remains to be done
std::any ASTBuilderPass::visitExplicitCast(GazpreaParser::ExplicitCastContext *ctx) {
    auto Cast = PM->Builder.build<ExplicitCast>();
    Cast->setCtx(ctx);

    auto TargetType = castToTypeVisit(ctx->type());
    Cast->setTargetType(TargetType);

    // Set expression that is being cast.
    Cast->setExpr(castToNodeVisit(ctx->expr()));

    return cast<ASTNodeT>(Cast);
}

std::any ASTBuilderPass::visitBoolLiteral(GazpreaParser::BoolLiteralContext *ctx) {
    auto *BoolLit = PM->Builder.build<BoolLiteral>();
    BoolLit->setCtx(ctx);

    if (ctx->TRUE())
        BoolLit->setTrue();

    return cast<ASTNodeT>(BoolLit);
}

std::any ASTBuilderPass::visitUnaryExpr(GazpreaParser::UnaryExprContext *ctx) {
    auto *UnaryExpr = PM->Builder.build<UnaryOp>();
    UnaryExpr->setCtx(ctx);

    // Set the operator
    if (ctx->ADD())
        UnaryExpr->setOp(UnaryOp::ADD);

    if (ctx->SUB())
        UnaryExpr->setOp(UnaryOp::SUB);

    if (ctx->NOT())
        UnaryExpr->setOp(UnaryOp::NOT);

    // Set the expression
    auto Expr = castToNodeVisit(ctx->expr());
    UnaryExpr->setExpr(Expr);

    return cast<ASTNodeT>(UnaryExpr);
}

// Ignore for part1
std::any ASTBuilderPass::visitGeneratorExpr(GazpreaParser::GeneratorExprContext *ctx) {
    throw std::runtime_error("Unimplemented");
}

std::any ASTBuilderPass::visitExpExpr(GazpreaParser::ExpExprContext *ctx) {
    auto ExpExpr = PM->Builder.build<ArithmeticOp>();
    ExpExpr->setCtx(ctx);

    ExpExpr->setOp(ArithmeticOp::EXP);

    // Set the left expression.
    ExpExpr->setLeftExpr(castToNodeVisit(ctx->expr(0)));

    // Set the right expression.
    ExpExpr->setRightExpr(castToNodeVisit(ctx->expr(1)));

    return cast<ASTNodeT>(ExpExpr);
}

std::any ASTBuilderPass::visitCompExpr(GazpreaParser::CompExprContext *ctx) {
    auto CompExpr = PM->Builder.build<ComparisonOp>();
    CompExpr->setCtx(ctx);

    // Set the operator
    if (ctx->LT())
        CompExpr->setOp(ComparisonOp::LT);

    if (ctx->GT())
        CompExpr->setOp(ComparisonOp::GT);

    if (ctx->LTEQ())
        CompExpr->setOp(ComparisonOp::LTEQ);

    if (ctx->GTEQ())
        CompExpr->setOp(ComparisonOp::GTEQ);

    // Set the left expression.
    CompExpr->setLeftExpr(castToNodeVisit(ctx->expr(0)));

    // Set the right expression.
    CompExpr->setRightExpr(castToNodeVisit(ctx->expr(1)));


    // std::any shenanigans.
    return cast<ASTNodeT>(CompExpr);
}

std::any ASTBuilderPass::visitIdentityLiteral(GazpreaParser::IdentityLiteralContext *ctx) {
    auto IdentLit = PM->Builder.build<IdentityLiteral>();
    IdentLit->setCtx(ctx);
    return cast<ASTNodeT>(IdentLit);
}

std::any ASTBuilderPass::visitMemberAccess(GazpreaParser::MemberAccessContext *ctx) {
    auto MemberAcc = PM->Builder.build<MemberAccess>();
    MemberAcc->setCtx(ctx);

    // Set the identifier
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setCtx(ctx);
    Ident->setName(ctx->ID(0)->getText());
    MemberAcc->setExpr(Ident);

    // When the member is accessed by name.
    if (auto MemberId = ctx->ID(1)) {
        auto IdentExpr = PM->Builder.build<Identifier>();
        IdentExpr->setCtx(ctx);
        IdentExpr->setName(MemberId->getText());
        MemberAcc->setMemberExpr(IdentExpr);
    }
    // When the member is accessed by index.
    if (ctx->INTLITERAL()) {
        auto IntegerLit = PM->Builder.build<IntLiteral>();
        IntegerLit->setCtx(ctx);
        IntegerLit->setVal(ctx->INTLITERAL()->getText());
        MemberAcc->setMemberExpr(IntegerLit);
    }

    return cast<ASTNodeT>(MemberAcc);
}

std::any ASTBuilderPass::visitIdentifier(GazpreaParser::IdentifierContext *ctx) {
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setCtx(ctx);
    Ident->setName(ctx->getText());
    return cast<ASTNodeT>(Ident);
}

std::any ASTBuilderPass::visitNullLiteral(GazpreaParser::NullLiteralContext *ctx) {
    auto NullLit = PM->Builder.build<NullLiteral>();
    NullLit->setCtx(ctx);
    return cast<ASTNodeT>(NullLit);
}

std::any ASTBuilderPass::visitAddSubExpr(GazpreaParser::AddSubExprContext *ctx) {
    auto Expr = PM->Builder.build<ArithmeticOp>();
    Expr->setCtx(ctx);

    // Set operator
    if (ctx->ADD())
        Expr->setOp(ArithmeticOp::ADD);
    else
        Expr->setOp(ArithmeticOp::SUB);

    // Set the left expression.
    Expr->setLeftExpr(castToNodeVisit(ctx->expr(0)));

    // Set the right expression.
    Expr->setRightExpr(castToNodeVisit(ctx->expr(1)));

    return cast<ASTNodeT>(Expr);
}

std::any ASTBuilderPass::visitBracketExpr(GazpreaParser::BracketExprContext *ctx) {
    return castToNodeVisit(ctx->expr());
}

std::any ASTBuilderPass::visitRealLiteral(GazpreaParser::RealLiteralContext *ctx) {
    return castToNodeVisit(ctx->realLit());
}

std::any ASTBuilderPass::visitIntLiteral(GazpreaParser::IntLiteralContext *ctx) {
    auto IntegerLit = PM->Builder.build<IntLiteral>();
    IntegerLit->setCtx(ctx);
    IntegerLit->setVal(ctx->INTLITERAL()->getText());

    return cast<ASTNodeT>(IntegerLit);
}

std::any ASTBuilderPass::visitMulDivModDotProdExpr(GazpreaParser::MulDivModDotProdExprContext *ctx) {
    auto Expr = PM->Builder.build<ArithmeticOp>();
    Expr->setCtx(ctx);

    // Set operator
    if (ctx->MUL())
        Expr->setOp(ArithmeticOp::MUL);

    if (ctx->DIV())
        Expr->setOp(ArithmeticOp::DIV);

    if (ctx->MOD())
        Expr->setOp(ArithmeticOp::MOD);

    // Set left expression
    Expr->setLeftExpr(castToNodeVisit(ctx->expr(0)));

    // Set right expression
    Expr->setRightExpr(castToNodeVisit(ctx->expr(1)));

    return cast<ASTNodeT>(Expr);
}

// ignored for part1
std::any ASTBuilderPass::visitByExpr(GazpreaParser::ByExprContext *ctx) {
    throw std::runtime_error("Unimplemented");
}

std::any ASTBuilderPass::visitOrExpr(GazpreaParser::OrExprContext *ctx) {
    auto OrExpr = PM->Builder.build<LogicalOp>();
    OrExpr->setCtx(ctx);

    // Set operator
    if (ctx->OR())
        OrExpr->setOp(LogicalOp::OR);
    else
        OrExpr->setOp(LogicalOp::XOR);

    // Set the left expression.
    OrExpr->setLeftExpr(castToNodeVisit(ctx->expr(0)));

    // Set the right expression.
    OrExpr->setRightExpr(castToNodeVisit(ctx->expr(1)));
    return cast<ASTNodeT>(OrExpr);
}


// ignored for part1
std::any ASTBuilderPass::visitFilterExpr(GazpreaParser::FilterExprContext *ctx) {
    throw std::runtime_error("Unimplemented");
}


std::any ASTBuilderPass::visitCharLiteral(GazpreaParser::CharLiteralContext *ctx) {
    auto CharLit = PM->Builder.build<CharLiteral>();
    CharLit->setCtx(ctx);
    std::string CharVal = ctx->CHARLITERAL()->getText();

    if (CharVal.length() == 3) {
        CharLit->setCharacter(CharVal[1]);
    } else {
        char Escape = CharVal[2];
        char Val;
        switch (Escape) {
            case '0':
                Val = 0x00;
                break;
            case 'a':
                Val = 0x07;
                break;
            case 'b':
                Val = 0x08;
                break;
            case 't':
                Val = 0x09;
                break;
            case 'n':
                Val = 0x0A;
                break;
            case 'r':
                Val = 0x0D;
                break;
            case '\"':
                Val = 0x22;
                break;
            case '\'':
                Val = 0x27;
                break;
            case '\\':
                Val = 0x5C;
                break;
            default:
                Val = Escape;
                break;
        }
        CharLit->setCharacter(Val);
    }
    return cast<ASTNodeT>(CharLit);
}

// ignored for part1
std::any ASTBuilderPass::visitIndexExpr(GazpreaParser::IndexExprContext *ctx) {
    throw std::runtime_error("Unimplemented");
}


std::any ASTBuilderPass::visitTupleLiteral(GazpreaParser::TupleLiteralContext *ctx) {
    auto TupleLit = PM->Builder.build<TupleLiteral>();
    TupleLit->setCtx(ctx);

    for (auto *Child : ctx->expr())
        TupleLit->addChild(castToNodeVisit(Child));

    // std::any shenanigans.
    return cast<ASTNodeT>(TupleLit);;
}

// ignored for part1
std::any ASTBuilderPass::visitAppendOp(GazpreaParser::AppendOpContext *ctx) {
    throw std::runtime_error("Unimplemented");
}

std::any ASTBuilderPass::visitFuncCall(GazpreaParser::FuncCallContext *ctx) {
    return castToNodeVisit(ctx->functionCall());
}

// ignored for part1
std::any ASTBuilderPass::visitRangeExpr(GazpreaParser::RangeExprContext *ctx) {
    throw std::runtime_error("Unimplemented");
}


std::any ASTBuilderPass::visitEqualExpr(GazpreaParser::EqualExprContext *ctx) {
    auto Expr = PM->Builder.build<LogicalOp>();
    Expr->setCtx(ctx);

    // Set operator
    if (ctx->EQEQ())
        Expr->setOp(LogicalOp::EQ);
    else
        Expr->setOp(LogicalOp::NEQ);

    // Set left expression
    Expr->setLeftExpr(castToNodeVisit(ctx->expr(0)));

    // Set right expression
    Expr->setRightExpr(castToNodeVisit(ctx->expr(1)));

    return cast<ASTNodeT>(Expr);
}

std::any ASTBuilderPass::visitAndExpr(GazpreaParser::AndExprContext *ctx) {
    auto AndExpr = PM->Builder.build<LogicalOp>();
    AndExpr->setCtx(ctx);
    AndExpr->setOp(LogicalOp::AND);

    // Set the left expression.
    AndExpr->setLeftExpr(castToNodeVisit(ctx->expr(0)));

    // Set right expression.
    AndExpr->setRightExpr(castToNodeVisit(ctx->expr(1)));

    return cast<ASTNodeT>(AndExpr);
}

std::any ASTBuilderPass::visitTupleType(GazpreaParser::TupleTypeContext *ctx) {
    vector<const Type*> MemberTypes;
    map<string, int> Mappings;
    int Idx = 1;
    for (auto *Member : ctx->tupleTypeDecl()->tupleMemberType()) {
        if (Member->ID())
            Mappings.insert({Member->ID()->getText(), Idx});
        MemberTypes.emplace_back(castToTypeVisit(Member->type()));
        ++Idx;
    }
    return PM->TypeReg.getTupleType(MemberTypes, Mappings, false);
}

std::any ASTBuilderPass::visitBreakStmt(GazpreaParser::BreakStmtContext *ctx) {
    auto BreakStmt = PM->Builder.build<Break>();
    BreakStmt->setCtx(ctx);
    return cast<ASTNodeT>(BreakStmt);
}

std::any ASTBuilderPass::visitContinueStmt(GazpreaParser::ContinueStmtContext *ctx) {
    auto ContinueStmt = PM->Builder.build<Continue>();
    ContinueStmt->setCtx(ctx);
    return cast<ASTNodeT>(ContinueStmt);
}

std::any ASTBuilderPass::visitStmt(GazpreaParser::StmtContext *ctx) {
    if (ctx->simpleStmt())
        return castToNodeVisit(ctx->simpleStmt());

    return castToNodeVisit(ctx->block());
}

std::any ASTBuilderPass::visitGlobalIdentDecl(GazpreaParser::GlobalIdentDeclContext *ctx) {
    auto Type = PM->TypeReg.getConstTypeOf(castToTypeVisit(ctx->type()));
    auto Expr = castToNodeVisit(ctx->expr());
    auto Decl = PM->Builder.build<Declaration>();
    Decl->setCtx(ctx);
    Decl->setIdentType(Type);
    Decl->setInitExpr(Expr);
    Decl->setConst();
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setCtx(ctx);
    Ident->setName(ctx->ID()->getText());
    Ident->setIdentType(Type);
    Decl->setIdent(Ident);
    return cast<ASTNodeT>(Decl);
}

std::any ASTBuilderPass::visitGlobalIdentDeclStmt(GazpreaParser::GlobalIdentDeclStmtContext *ctx) {
    return castToNodeVisit(ctx->globalIdentDecl());
}

std::any ASTBuilderPass::visitFunctionDeclrStmt(GazpreaParser::FunctionDeclrStmtContext *ctx) {
    return castToNodeVisit(ctx->functionDeclr());
}

std::any ASTBuilderPass::visitFunctionDefStmt(GazpreaParser::FunctionDefStmtContext *ctx) {
    return castToNodeVisit(ctx->functionDefinition());
}

std::any ASTBuilderPass::visitProcedureDeclrStmt(GazpreaParser::ProcedureDeclrStmtContext *ctx) {
    return castToNodeVisit(ctx->procedureDeclr());
}

std::any ASTBuilderPass::visitProcedureDefStmt(GazpreaParser::ProcedureDefStmtContext *ctx) {
    return castToNodeVisit(ctx->procedureDefinition());
}

std::any ASTBuilderPass::visitTypeDefStmt(GazpreaParser::TypeDefStmtContext *ctx) {
    return castToNodeVisit(ctx->typeDef());
}

std::any ASTBuilderPass::visitIdentDeclStmt(GazpreaParser::IdentDeclStmtContext *ctx) {
    return castToNodeVisit(ctx->identDecl());
}

std::any ASTBuilderPass::visitAssignmentStmt(GazpreaParser::AssignmentStmtContext *ctx) {
    return castToNodeVisit(ctx->assignment());
}

std::any ASTBuilderPass::visitConditionalStmt(GazpreaParser::ConditionalStmtContext *ctx) {
    return castToNodeVisit(ctx->conditional());
}

std::any ASTBuilderPass::visitLoopStmt(GazpreaParser::LoopStmtContext *ctx) {
    return castToNodeVisit(ctx->loop());
}

std::any ASTBuilderPass::visitOutputStmt(GazpreaParser::OutputStmtContext *ctx) {
    return castToNodeVisit(ctx->output());
}

std::any ASTBuilderPass::visitInputStmt(GazpreaParser::InputStmtContext *ctx) {
    return castToNodeVisit(ctx->input());
}

std::any ASTBuilderPass::visitProcedureCallStmt(GazpreaParser::ProcedureCallStmtContext *ctx) {
    return castToNodeVisit(ctx->procedureCall());
}

std::any ASTBuilderPass::visitIdentLValue(GazpreaParser::IdentLValueContext *ctx) {
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setCtx(ctx);
    Ident->setName(ctx->ID()->getText());
    auto IdentRef = PM->Builder.build<IdentReference>();
    IdentRef->setIdentifier(Ident);
    return cast<ASTNodeT>(IdentRef);
}

std::any ASTBuilderPass::visitIndexLValue(GazpreaParser::IndexLValueContext *ctx) {
    throw std::runtime_error("Unimplemented");
}

std::any ASTBuilderPass::visitMemAccessLValue(GazpreaParser::MemAccessLValueContext *ctx) {
    auto MemberRef = PM->Builder.build<MemberReference>();
    MemberRef->setCtx(ctx);
    // Set the identifier
    auto Ident = PM->Builder.build<Identifier>();
    Ident->setName(ctx->ID(0)->getText());
    MemberRef->setIdentifier(Ident);

    // When the member is accessed by name.
    if (auto MemberId = ctx->ID(1)) {
        auto IdentExpr = PM->Builder.build<Identifier>();
        IdentExpr->setName(MemberId->getText());
        MemberRef->setMemberExpr(IdentExpr);
    }
    // When the member is accessed by index.
    if (ctx->INTLITERAL()) {
        auto IntegerLit = PM->Builder.build<IntLiteral>();
        IntegerLit->setVal(ctx->INTLITERAL()->getText());
        MemberRef->setMemberExpr(IntegerLit);
    }

    return cast<ASTNodeT>(MemberRef);
}

std::any ASTBuilderPass::visitTupleUnpackLValue(GazpreaParser::TupleUnpackLValueContext *ctx) {
    throw std::runtime_error("Unimplemented");
}

std::any ASTBuilderPass::visitRealLit1(GazpreaParser::RealLit1Context *ctx) {
    auto RealLit = PM->Builder.build<RealLiteral>();
    RealLit->setCtx(ctx);

    string RealString = ctx->INTLITERAL(0)->getText() + ".";
    if (ctx->INTLITERAL().size() == 2)
        RealString += ctx->INTLITERAL(1)->getText();
    if (ctx->ExponentialLiteral())
        RealString += ctx->ExponentialLiteral()->getText();

    RealLit->setVal(RealString);

    return cast<ASTNodeT>(RealLit);
}

std::any ASTBuilderPass::visitRealLit2(GazpreaParser::RealLit2Context *ctx) {
    auto RealLit = PM->Builder.build<RealLiteral>();
    RealLit->setCtx(ctx);
    string RealString = ctx->INTLITERAL()->getText() + ".";
    if (ctx->ExponentialLiteral())
        RealString += ctx->ExponentialLiteral()->getText();

    RealLit->setVal(RealString);

    return cast<ASTNodeT>(RealLit);
}

std::any ASTBuilderPass::visitRealLit3(GazpreaParser::RealLit3Context *ctx) {
    auto RealLit = PM->Builder.build<RealLiteral>();
    RealLit->setCtx(ctx);
    string RealString = ctx->INTLITERAL()->getText();
    RealString += ctx->ExponentialLiteral()->getText();

    RealLit->setVal(RealString);

    return cast<ASTNodeT>(RealLit);
}
