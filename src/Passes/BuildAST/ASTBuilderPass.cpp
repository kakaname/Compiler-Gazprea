//
// Created by dhanrajbir on 05/11/22.
//

#include "Passes/BuildAST/ASTBuilderPass.h"
#include "llvm/Support/Casting.h"
#include "Types/CompositeTypes.h"
#include "Types/Type.h"
#include "Passes/BuildAST/ScopeResolutionPass.h"

using llvm::cast;
using llvm::isa;
using llvm::dyn_cast;

void ASTBuilderPass::runOnAST(ASTPassManager &Manager, ASTNodeT *Root) {
    PM = &Manager;
    PM->setResult<SelfT>(ResultT());
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
        NodeToMarkForTypeSize = Decl;
        CurrentIdxToMark = 0;
        auto DeclType = castToTypeVisit(ctx->type());
        NodeToMarkForTypeSize = nullptr;
        CurrentIdxToMark = 0;
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
    auto Body = castToNodeVisit(ctx->stmt());
    if (!isa<Block>(Body)) {
        auto Blk = wrapStmtInBlock(Body);
        Blk->setCtx(ctx);
        IfStat->setBlock(Blk);
        return cast<ASTNodeT>(IfStat);
    }

    IfStat->setBlock(cast<Block>(Body));
    return cast<ASTNodeT>(IfStat);
}

std::any ASTBuilderPass::visitIfElseConditional(GazpreaParser::IfElseConditionalContext *ctx) {
    auto IfElseStat = PM->Builder.build<ConditionalElse>();
    IfElseStat->setCtx(ctx);
    IfElseStat->setConditional(castToNodeVisit(ctx->expr()));

    auto IfBody = castToNodeVisit(ctx->stmt(0));
    if (!isa<Block>(IfBody)) {
        auto IfBodyBlock = wrapStmtInBlock(IfBody);
        IfBodyBlock->setCtx(ctx);
        IfElseStat->setIfBlock(IfBodyBlock);
    } else
        IfElseStat->setIfBlock(cast<Block>(IfBody));

    auto ElseBody = castToNodeVisit(ctx->stmt(1));

    if (!isa<Block>(ElseBody)) {
        auto ElseBodyBlock = wrapStmtInBlock(ElseBody);
        ElseBodyBlock->setCtx(ctx);
        IfElseStat->setElseBlock(ElseBodyBlock);
    } else
        IfElseStat->setElseBlock(cast<Block>(ElseBody));

    return cast<ASTNodeT>(IfElseStat);
}

std::any ASTBuilderPass::visitInfiniteLoop(GazpreaParser::InfiniteLoopContext *ctx) {
    auto Loop = PM->Builder.build<InfiniteLoop>();
    Loop->setCtx(ctx);

    auto Body = castToNodeVisit(ctx->stmt());
    if (!isa<Block>(Body)) {
        auto Blk = wrapStmtInBlock(Body);
        Blk->setCtx(ctx);
        Loop->setBlock(Blk);
        return cast<ASTNodeT>(Loop);
    }
    Loop->setBlock(cast<Block>(Body));
    return cast<ASTNodeT>(Loop);
}

std::any ASTBuilderPass::visitWhileLoop(GazpreaParser::WhileLoopContext *ctx) {
    auto Loop = PM->Builder.build<ConditionalLoop>();
    Loop->setCtx(ctx);

    // Set the conditional expression
    auto CondExpr = castToNodeVisit(ctx->expr());
    Loop->setConditional(CondExpr);

    // Set the statement body
    auto Body = castToNodeVisit(ctx->stmt());
    if (!isa<Block>(Body)) {
        auto Blk = wrapStmtInBlock(Body);
        Blk->setCtx(ctx);
        return cast<ASTNodeT>(Blk);
    }

    Loop->setBlock(dyn_cast<Block>(Body));
    return cast<ASTNodeT>(Loop);
}

std::any ASTBuilderPass::visitDoWhileLoop(GazpreaParser::DoWhileLoopContext *ctx) {
    auto Loop = PM->Builder.build<ConditionalLoop>();
    Loop->setCtx(ctx);

    // Set the conditional expression
    auto CondExpr = castToNodeVisit(ctx->expr());
    Loop->setConditional(CondExpr);

    auto Body = castToNodeVisit(ctx->stmt());
    if (!isa<Block>(Body)) {
        auto Blk = wrapStmtInBlock(Body);
        Blk->setCtx(ctx);
        return cast<ASTNodeT>(Blk);
    }

    Loop->setBlock(dyn_cast<Block>(Body));

    // Set conditional after because this is a do-while loop
    Loop->setConditionalAfter();

    return cast<ASTNodeT>(Loop);
}

std::any ASTBuilderPass::visitDomainLoop(GazpreaParser::DomainLoopContext *ctx) {


    // Set the statement body
    auto Body = castToNodeVisit(ctx->stmt());
    Block *Blk;
    if (!isa<Block>(Body)) {
        Blk = wrapStmtInBlock(Body);
        Blk->setCtx(ctx);
    } else {
        Blk = dyn_cast<Block>(Body);
    }

    auto Exprs = ctx->expr();
    auto IDs = ctx->ID();
    auto Size = Exprs.size();

    DomainLoop *Loop;
    for (size_t i = Size - 1; i >= 0; i--) {
        if (i == -1) break;
        Loop = PM->Builder.build<DomainLoop>();
        Loop->setCtx(ctx);
        Loop->setBody(Blk);
        auto thisExpr = ctx->expr(i);
        Loop->setDomain(castToNodeVisit(thisExpr));
        Blk->setParent(Loop);


        // Create Identifier
        auto ID = PM->Builder.build<Identifier>();
        ID->setCtx(ctx);
        ID->setName(IDs[i]->getText());

        Loop->setID(ID);

        Blk = wrapStmtInBlock(Loop);
        Blk->setCtx(ctx);

    }


    return cast<ASTNodeT>(Loop);

}

std::any ASTBuilderPass::visitTypeDef(GazpreaParser::TypeDefContext *ctx) {
    auto &GlobalScope = PM->getResource<ScopeTreeNode>();
    auto BaseType = castToTypeVisit(ctx->type());
    auto NewType = PM->SymTable.defineTypeSymbol(ctx->ID()->getText(), BaseType);
    GlobalScope.declareType(ctx->ID()->getText(), NewType);
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
    auto ResolvedSym = GlobalScope.resolveType(ctx->ID()->getText());
    auto text = ctx->ID()->getText();

    if (!ResolvedSym)
        throw std::runtime_error(text + " type not found.");

    auto TypeSym = dyn_cast<TypeSymbol>(ResolvedSym);
    if (!TypeSym)
        throw std::runtime_error(text + " is not a type.");
    return TypeSym->getType();
}

std::any ASTBuilderPass::visitVectorType(GazpreaParser::VectorTypeContext *ctx) {

    // determine type of inner
    auto Type = castToTypeVisit(ctx->type());

    // determine if we have a known size or wildcard
    auto Size = ctx->expressionOrWildcard();
    // Size must be inferred.
    if (Size->MUL())
        return PM->TypeReg.getVectorType(Type, -1, false);

    // Try to constant fold it.
    long VecSize = -1;
    try {
        VecSize = std::any_cast<long>(Folder.visit(Size->expr()));
    } catch (exception&) {}

    // If the size cannot be folded, then there must an expression
    // specifying the size.
    auto SizeTree = castToNodeVisit(Size->expr());
    auto VecTy = PM->TypeReg.getVectorType(Type, (int) VecSize, false);
    cast<VectorTy>(VecTy)->setSizeExpr(SizeTree);
    return VecTy;
}

std::any ASTBuilderPass::visitMatrixType(GazpreaParser::MatrixTypeContext *ctx) {
    auto InnerTy = castToTypeVisit(ctx->type());

    auto RowSizeExpr = ctx->expressionOrWildcard(0);
    auto ColSizeExpr = ctx->expressionOrWildcard(1);

    long Rows = -1;
    long Cols = -1;

    if (RowSizeExpr->expr()) {
        try {Rows = std::any_cast<long>(Folder.visit(RowSizeExpr->expr()));}
        catch (exception&) {};
    }

    if (ColSizeExpr->expr()) {
        try {Cols = std::any_cast<long>(Folder.visit(ColSizeExpr->expr()));}
        catch (exception&) {};
    }

    auto MatTy = PM->TypeReg.getMatrixType(InnerTy, Rows, Cols, false);

    if (RowSizeExpr->expr())
        cast<MatrixTy>(MatTy)->setRowSizeExpr(castToNodeVisit(RowSizeExpr->expr()));

    if (ColSizeExpr->expr())
        cast<MatrixTy>(MatTy)->setColSizeExpr(castToNodeVisit(ColSizeExpr->expr()));

    return MatTy;
}


std::any ASTBuilderPass::visitIntervalType(GazpreaParser::IntervalTypeContext *ctx) {
    auto InnerTy = castToTypeVisit(ctx->type());
    if (!isa<IntegerTy>(InnerTy))
        throw runtime_error("Intervals may only contain integers");
    return PM->TypeReg.getIntervalTy(false);
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
    throw std::runtime_error("Unimplemented: Expression or wildcard");
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


    vector<Type*> ParamTypes;

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

    Type *ProcRetTy{nullptr};
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

    vector<Type*> ParamTypes;

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

    NodeToMarkForTypeSize = Cast;
    auto TargetType = castToTypeVisit(ctx->type());
    NodeToMarkForTypeSize = nullptr;
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

    if (ctx->DOTPROD()) {
        auto DotProd = PM->Builder.build<DotProduct>();
        DotProd->setCtx(ctx);

        // Set the left expression.
        DotProd->setLHS(castToNodeVisit(ctx->expr(0)));

        // Set the right expression.
        DotProd->setRHS(castToNodeVisit(ctx->expr(1)));

        return cast<ASTNodeT>(DotProd);
    }


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
    auto ByExpr = PM->Builder.build<ByOp>();
    ByExpr->setCtx(ctx);

    // Set the left expression.
    ByExpr->setBaseExpr(castToNodeVisit(ctx->expr(0)));

    // Set the right expression.
    ByExpr->setByExpr(castToNodeVisit(ctx->expr(1)));

    return cast<ASTNodeT>(ByExpr);
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
    auto IndexExpr = PM->Builder.build<Index>();
    IndexExpr->setCtx(ctx);

    // Set the left expression.
    IndexExpr->setBaseExpr(castToNodeVisit(ctx->expr(0)));

    // Set the right expression.
    IndexExpr->setIndexExpr(castToNodeVisit(ctx->expr(1)));

    if (ctx->expr(2)) {
        IndexExpr->setIndex2Expr(castToNodeVisit(ctx->expr(2)));
    }

    return cast<ASTNodeT>(IndexExpr);

}


std::any ASTBuilderPass::visitTupleLiteral(GazpreaParser::TupleLiteralContext *ctx) {
    auto TupleLit = PM->Builder.build<TupleLiteral>();
    TupleLit->setCtx(ctx);

    for (auto *Child : ctx->expr())
        TupleLit->addChild(castToNodeVisit(Child));

    return cast<ASTNodeT>(TupleLit);;
}

std::any ASTBuilderPass::visitVectorLiteral(GazpreaParser::VectorLiteralContext *ctx) {
    auto VectorLit = PM->Builder.build<VectorLiteral>();
    VectorLit->setCtx(ctx);

    for (auto *Child : ctx->expr())
        VectorLit->addChild(castToNodeVisit(Child));

    return cast<ASTNodeT>(VectorLit);;
}

// ignored for part1
std::any ASTBuilderPass::visitAppendOp(GazpreaParser::AppendOpContext *ctx) {
    auto ConcatOp = PM->Builder.build<Concat>();
    ConcatOp->setCtx(ctx);

    // Set the left expression.
    ConcatOp->setLHS(castToNodeVisit(ctx->expr(0)));

    // Set the right expression.
    ConcatOp->setRHS(castToNodeVisit(ctx->expr(1)));

    return cast<ASTNodeT>(ConcatOp);
}

std::any ASTBuilderPass::visitFuncCall(GazpreaParser::FuncCallContext *ctx) {
    return castToNodeVisit(ctx->functionCall());
}

// ignored for part1
std::any ASTBuilderPass::visitRangeExpr(GazpreaParser::RangeExprContext *ctx) {
    auto IntInterval = PM->Builder.build<Interval>();
    IntInterval->setCtx(ctx);

    auto Upper = castToNodeVisit(ctx->expr(0));
    auto Lower = castToNodeVisit(ctx->expr(1));

    IntInterval->setLowerExpr(Upper);
    IntInterval->setUpperExpr(Lower);

    return cast<ASTNodeT>(IntInterval);
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
    vector<Type*> MemberTypes;
    map<string, int> Mappings;
    int Idx = 1;
    for (auto *Member : ctx->tupleTypeDecl()->tupleMemberType()) {
        if (Member->ID()) {
            if (Mappings.count(Member->ID()->getText()))
                throw std::runtime_error("Tuple member with duplicate name");
            Mappings.insert({Member->ID()->getText(), Idx});
        }
        CurrentIdxToMark = Idx - 1;
        MemberTypes.emplace_back(castToTypeVisit(Member->type()));
        ++Idx;
    }
    CurrentIdxToMark = 0;
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
    auto Decl = PM->Builder.build<Declaration>();
    auto Expr = castToNodeVisit(ctx->expr());
    auto Ident = PM->Builder.build<Identifier>();
    if (ctx->type()) {
        auto Type = PM->TypeReg.getConstTypeOf(castToTypeVisit(ctx->type()));
        Decl->setIdentType(Type);
        Ident->setIdentType(Type);
    }
    // TODO intervals here too
    Decl->setCtx(ctx);
    Decl->setInitExpr(Expr);
    Decl->setConst();
    Ident->setCtx(ctx);
    Ident->setName(ctx->ID()->getText());
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
    auto IdxRef = PM->Builder.build<IndexReference>();
    IdxRef->setCtx(ctx);
    IdxRef->setBaseExpr(castToNodeVisit(ctx->expr(0)));
    IdxRef->setIndexExpr(castToNodeVisit(ctx->expr(1)));
    if (ctx->expr(2))
        IdxRef->setIndex2Expr(castToNodeVisit(ctx->expr(2)));
    return cast<ASTNodeT>(IdxRef);
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
    throw std::runtime_error("Unimplemented: TupleUnpack");
}

std::any ASTBuilderPass::visitRealLit1(GazpreaParser::RealLit1Context *ctx) {
    auto RealLit = PM->Builder.build<RealLiteral>();
    RealLit->setCtx(ctx);

    RealLit->setVal(ctx->ExponentialLiteral1()->getText());

    return cast<ASTNodeT>(RealLit);
}

std::any ASTBuilderPass::visitRealLit2(GazpreaParser::RealLit2Context *ctx) {
    auto RealLit = PM->Builder.build<RealLiteral>();
    RealLit->setCtx(ctx);

    RealLit->setVal(ctx->ExponentialLiteral2()->getText());

    return cast<ASTNodeT>(RealLit);
}

std::any ASTBuilderPass::visitRealLit3(GazpreaParser::RealLit3Context *ctx) {
    auto RealLit = PM->Builder.build<RealLiteral>();
    RealLit->setCtx(ctx);

    RealLit->setVal(ctx->ExponentialLiteral3()->getText());

    return cast<ASTNodeT>(RealLit);
}

std::any ASTBuilderPass::visitRealLit4(GazpreaParser::RealLit4Context *ctx) {
    auto RealLit = PM->Builder.build<RealLiteral>();
    RealLit->setCtx(ctx);

    RealLit->setVal(ctx->ExponentialLiteral4()->getText());

    return cast<ASTNodeT>(RealLit);
}

std::any ASTBuilderPass::visitRealLit5(GazpreaParser::RealLit5Context *ctx) {
    auto RealLit = PM->Builder.build<RealLiteral>();
    RealLit->setCtx(ctx);

    string realVal = ctx->INTLITERAL()->getText() + ".";
    RealLit->setVal(realVal);
    return cast<ASTNodeT>(RealLit);
}

std::any ASTBuilderPass::visitRealLit6(GazpreaParser::RealLit6Context *ctx) {
    auto RealLit = PM->Builder.build<RealLiteral>();
    RealLit->setCtx(ctx);

    string realVal = "." + ctx->INTLITERAL()->getText();
    RealLit->setVal(realVal);

    return cast<ASTNodeT>(RealLit);
}

std::any ASTBuilderPass::visitRealLit7(GazpreaParser::RealLit7Context *ctx) {
    auto RealLit = PM->Builder.build<RealLiteral>();
    RealLit->setCtx(ctx);

    string realVal = ctx->INTLITERAL(0)->getText() + "." + ctx->INTLITERAL(1)->getText();
    RealLit->setVal(realVal);

    return cast<ASTNodeT>(RealLit);
}

std::any ASTBuilderPass::visitGeneratorExpr(GazpreaParser::GeneratorExprContext *ctx) {
    auto Gen = PM->Builder.build<Generator>();

    auto DomainVar = PM->Builder.build<Identifier>();
    DomainVar->setName(ctx->ID()->getText());
    Gen->setDomainVariable(DomainVar);

    Gen->setDomain(castToNodeVisit(ctx->expr(0)));

    Gen->setExpr(castToNodeVisit(ctx->expr(1)));

    return cast<ASTNodeT>(Gen);
}


std::any ASTBuilderPass::visitMatrixGeneratorExpr(GazpreaParser::MatrixGeneratorExprContext *ctx) {
    auto Gen = PM->Builder.build<MatrixGenerator>();

    auto RowDomainVar = PM->Builder.build<Identifier>();
    RowDomainVar->setName(ctx->ID(0)->getText());
    Gen->setRowDomainVariable(RowDomainVar);

    Gen->setRowDomain(castToNodeVisit(ctx->expr(0)));

    auto ColumnDomainVarIdx = PM->Builder.build<Identifier>();
    ColumnDomainVarIdx->setName(ctx->ID(1)->getText());
    Gen->setColumnDomainVariable(ColumnDomainVarIdx);

    Gen->setColumnDomain(castToNodeVisit(ctx->expr(1)));

    Gen->setExpr(castToNodeVisit(ctx->expr(2)));

    return cast<ASTNodeT>(Gen);
}


std::any ASTBuilderPass::visitFilterExpr(GazpreaParser::FilterExprContext *ctx) {
    auto Filt = PM->Builder.build<Filter>();

    auto DomainVar = PM->Builder.build<Identifier>();
    DomainVar->setName(ctx->ID()->getText());
    Filt->setDomainVariable(DomainVar);

    Filt->setDomain(castToNodeVisit(ctx->expr(0)));

    auto PredList = PM->Builder.build<PredicatedList>();
    for (long long int I = 1; I < ctx->expr().size(); I++) {
        Filt->addChild(castToNodeVisit(ctx->expr(I)));
    }

    Filt->setPredicatedList(PredList);

    return cast<ASTNodeT>(Filt);
}




Block *ASTBuilderPass::wrapStmtInBlock(ASTNodeT *Stmt) {
    if (isa<Declaration>(Stmt))
        throw std::runtime_error("Declaration may only occur"
                                 " inside blocks");
    auto Blk = PM->Builder.build<Block>();
    Blk->addChild(Stmt);
    return Blk;
}

