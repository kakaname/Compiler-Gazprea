//
// Created by 陈 on 2022-11-09.
//

#include "Passes/CodeGenPass.h"
#include "Passes/LoopCheckPass.h"

using llvm::Value;

llvm::Value *CodeGenPass::visitProgram(Program *Prog) {

}

llvm::Value *CodeGenPass::visitIdentifier(Identifier *Ident) {

}

llvm::Value *CodeGenPass::visitAssignment(Assignment *Assign) {

}

llvm::Value *CodeGenPass::visitDeclaration(Declaration *Decl) {

}

llvm::Value *CodeGenPass::visitBlock(Block *Blk) {

}

llvm::Value *CodeGenPass::visitLogicalOp(LogicalOp *Op) {

}

llvm::Value *CodeGenPass::visitArithmeticOp(ArithmeticOp *Op) {

}

llvm::Value *CodeGenPass::visitIndex(Index *Idx) {

}

llvm::Value *CodeGenPass::visitInfiniteLoop(InfiniteLoop *Loop) {
    llvm::BasicBlock *LoopBody = llvm::BasicBlock::Create(GlobalCtx, "LoopBody");
    llvm::BasicBlock *LoopEnd = llvm::BasicBlock::Create(GlobalCtx, "LoopEnd");
    // FIXME this is a hack to annotate the LoopBody and LoopEnd
    PM->setAnnotation<CodeGenPass>(Loop, LoopEnd);
    PM->setAnnotation<CodeGenPass>(Loop->getStatement(), LoopBody);
    IR.CreateBr(LoopBody);

    MainFunc->getBasicBlockList().push_back(LoopBody);
    IR.SetInsertPoint(LoopBody);
    visit(Loop->getStatement());
    IR.CreateBr(LoopBody);
    MainFunc->getBasicBlockList().push_back(LoopEnd);
    IR.SetInsertPoint(LoopEnd);
    return nullptr
}

llvm::Value *CodeGenPass::visitConditionalLoop(ConditionalLoop *Loop) {
    Type* CondType = PM->getAnnotation<ExprTypeAnnotatorPass>(Loop->getConditional())
    assert(CondType == BoolType);
    llvm::BasicBlock *Header = llvm::BasicBlock::Create(GlobalCtx, "LoopHeader", MainFunc);
    llvm::BasicBlock *LoopBody = llvm::BasicBlock::Create(GlobalCtx, "LoopBody");
    llvm::BasicBlock *LoopEnd = llvm::BasicBlock::Create(GlobalCtx, "LoopEnd");
    PM->setAnnotation<CodeGenPass>(Loop, LoopEnd);
    if (Loop->ConditionalBefore) {
        IR.CreateBr(Header);
    } else {
        IR.CreateBr(LoopBody);
    }
    IR.SetInsertPoint(Header);
    Value *Res = visit(Loop->getConditional());
    IR.CreateCondBr(Res, LoopBody, LoopEnd);

    MainFunc->getBasicBlockList().push_back(LoopBody);
    IR.SetInsertPoint(LoopBody);
    visit(Loop->getStatement());
    IR.CreateBr(Header);
    MainFunc->getBasicBlockList().push_back(LoopEnd);
    IR.SetInsertPoint(LoopEnd);
    return nullptr;
}

// ignored for part1
llvm::Value *CodeGenPass::visitDomainLoop(DomainLoop *Loop) {

}

llvm::Value *CodeGenPass::visitIntLiteral(IntLiteral *IntLit) {

}

llvm::Value *CodeGenPass::visitNullLiteral(NullLiteral *NullLit) {

}

llvm::Value *CodeGenPass::visitIdentityLiteral(IdentityLiteral *IdentityLit) {

}

llvm::Value *CodeGenPass::visitRealLiteral(RealLiteral *RealLit) {

}

llvm::Value *CodeGenPass::visitBoolLiteral(BoolLiteral *BoolLit) {

}

llvm::Value *CodeGenPass::visitCharLiteral(CharLiteral *CharLit) {

}

llvm::Value *CodeGenPass::visitTupleLiteral(TupleLiteral *TupleLit) {

}

llvm::Value *CodeGenPass::visitMemberAccess(MemberAccess *MemberAcc) {

}

llvm::Value *CodeGenPass::visitTupleTypeDecl(TupleTypeDecl *TupleTypeDecl) {

}

llvm::Value *CodeGenPass::visitConditional(Conditional *Cond) {

}

llvm::Value *CodeGenPass::visitConditionalElse(ConditionalElse *Cond) {

}

llvm::Value *CodeGenPass::visitTypeCast(TypeCast *Cast) {

}

llvm::Value *CodeGenPass::visitBitwiseOp(BitwiseOp *Op) {

}

llvm::Value *CodeGenPass::visitUnaryOp(UnaryOp *Op) {

}

llvm::Value *CodeGenPass::visitArgsList(ArgsList *List) {

}

llvm::Value *CodeGenPass::visitParasList(ParasList *List) {

}

llvm::Value *CodeGenPass::visitFunctionDecl(FunctionDecl *FuncDecl) {

}

llvm::Value *CodeGenPass::visitFunctionDef(FunctionDef *FuncDef) {

}

llvm::Value *CodeGenPass::visitFunctionCall(FunctionCall *FuncCall) {

}

llvm::Value *CodeGenPass::visitProcedureDecl(ProcedureDecl *ProcedureDecl) {

}

llvm::Value *CodeGenPass::visitProcedureDef(ProcedureDef *ProcedureDef) {

}

llvm::Value *CodeGenPass::visitProcedureCall(ProcedureCall *ProcedureCall) {
    // The stream state function is a special case
    // so it is handled here
    if (Ident->getName() == "stream_state")  {
        // load the stream state
        Value *StreamState = IR.CreateLoad(StreamStateLoc);
        // TODO setup return value
    }
}

llvm::Value *CodeGenPass::visitReturn(Return *Return) {

}

llvm::Value *CodeGenPass::visitBreak(Break *Break) {
    llvm::BasicBlock *AfterBreak = llvm::BasicBlock::Create(GlobalCtx, "AfterBreak");
    TreeNode *Loop = PM->getAnnotation<LoopCheckPass>(Break);
    // FIXME check if this works
    auto *LoopEnd = static_cast<llvm::BasicBlock *>(PM->getAnnotation<CodeGenPass>(Loop));
    IR.CreateBr(LoopEnd);
    MainFunc->getBasicBlockList().push_back(AfterBreak);
    IR.SetInsertPoint(AfterBreak);
    return nullptr;

}

llvm::Value *CodeGenPass::visitContinue(Continue *Continue) {

    llvm::BasicBlock *AfterBreak = llvm::BasicBlock::Create(GlobalCtx, "AfterContinue");
    TreeNode *Loop = PM->getAnnotation<LoopCheckPass>(Continue);
    // FIXME check if this works
    if (Loop->getKind() == TreeNode::N_AST_InfiniteLoop) {
        auto *ILoop = dynamic_cast<InfiniteLoop *>(Loop);
        auto *LoopBody = static_cast<llvm::BasicBlock *>(PM->getAnnotation<CodeGenPass>(ILoop->getStatement()));
        IR.CreateBr(LoopBody);
    } else if (Loop->getKind() == TreeNode::N_AST_ConditionalLoop) {
        auto *CLoop = dynamic_cast<ConditionalLoop *>(Loop);
        auto *LoopBody = static_cast<llvm::BasicBlock *>(PM->getAnnotation<CodeGenPass>(CLoop->getStatement()));
        IR.CreateBr(LoopBody);
    } else {
        assert(false && "Unknown loop type");
    }
    MainFunc->getBasicBlockList().push_back(AfterBreak);
    IR.SetInsertPoint(AfterBreak);
    return nullptr;

}

llvm::Value *CodeGenPass::visitOutStream(OutStream *OutStream) {
    Value *ValToOut = visit(Stream->getOutStreamExpr());
    Type ValType = PM->getAnnotation<ExprTypeAnnotatorPass>(*Stream->getOutStreamExpr());
    // TODO depends on the TypeRegistry Implementation
    if (ValType == CharType) {
        // TODO enforce ValToOut = 0/1 for null and identity
        IR.CreateCall(PrintCharFunc, {ValToOut});
    } else if (ValType == IntegerType) {
        IR.CreateCall(PrintIntFunc, {ValToOut});
    } else if (ValType == RealType) {
        IR.CreateCall(PrintRealFunc, {ValToOut});
    } else if (ValType == BoolType) {
        IR.CreateCall(PrintBoolFunc, {ValToOut});
    } else {
        // TODO error handling
        assert(false && "Cannot output non-output type");
    }
}

llvm::Value *CodeGenPass::visitInStream(InStream *InStream) {
    Type IdentTy = PM->getAnnotation<ExprTypeAnnotatorPass>(*Stream->getInStreamIdent());
    Value *StoreLoc = SymbolMap[Stream->getInStreamIdent()->getSymbolId()];

    if (IdentTy == CharType) {
        IR.CreateCall(ReadCharFunc, {StoreLoc, StreamStateLoc, Buffer});
    } else if (IdentTy == IntegerType) {
        IR.CreateCall(ReadIntFunc, {StoreLoc, StreamStateLoc, Buffer});
    } else if (IdentTy == RealType) {
        IR.CreateCall(ReadRealFunc, {StoreLoc, StreamStateLoc, Buffer});
    } else if (IdentTy == BoolType) {
        IR.CreateCall(ReadBoolFunc, {StoreLoc, StreamStateLoc, Buffer});
    } else {
        assert(false && "Cannot input non-input type");
    }
}

llvm::Value *CodeGenPass::visitExplicitCast(ExplicitCast *ExplicitCast) {

}
