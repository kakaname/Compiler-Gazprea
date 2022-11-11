//
// Created by 陈 on 2022-11-09.
//

#ifndef GAZPREABASE_CODEGENPASS_H
#define GAZPREABASE_CODEGENPASS_H

#include <map>
#include <vector>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_os_ostream.h"

#include "Passes/PassManager.h"
#include "Passes/VisitorPass.h"
#include "Passes/ExprTypeAnnotatorPass.h"


class CodeGenPass: VisitorPass<CodeGenPass, llvm::Value*> {
public:
    using AnnotationT = llvm::Value*;
private:

    llvm::LLVMContext GlobalCtx;
    llvm::IRBuilder<> IR;
    llvm::Module Mod;

    ASTPassManager *PM;

    // Use to keep track of which llvm values represents which symbols in the
    // program.
    map<unsigned, llvm::Value*> SymbolMap;

    // The file to dump the outputs to.
    const char *OutputFile;

    explicit CodeGenPass(const char *OutFile) : VisitorPass(), GlobalCtx(), IR(GlobalCtx), Mod("gazprea", GlobalCtx), OutputFile(OutFile) {}

    void runOnAST(ASTPassManager &Manager, ASTNodeT &Root);

    llvm::Value *visitProgram(Program *Prog);

    llvm::Value *visitIdentifier(Identifier *Ident);

    llvm::Value *visitAssignment(Assignment *Assign);

    llvm::Value *visitDeclaration(Declaration *Decl);

    llvm::Value *visitBlock(Block *Blk);

    llvm::Value *visitLogicalOp(LogicalOp *Op);

    llvm::Value *visitArithmeticOp(ArithmeticOp *Op);

    llvm::Value *visitIndex(Index *Idx);

    llvm::Value *visitInfiniteLoop(InfiniteLoop *Loop);

    llvm::Value *visitConditionalLoop(ConditionalLoop *Loop);

    // ignored for part1
    llvm::Value *visitDomainLoop(DomainLoop *Loop);

    llvm::Value *visitIntLiteral(IntLiteral *IntLit);

    llvm::Value *visitNullLiteral(NullLiteral *NullLit);

    llvm::Value *visitIdentityLiteral(IdentityLiteral *IdentityLit);

    llvm::Value *visitRealLiteral(RealLiteral *RealLit);

    llvm::Value *visitBoolLiteral(BoolLiteral *BoolLit);

    llvm::Value *visitCharLiteral(CharLiteral *CharLit);

    llvm::Value *visitTupleLiteral(TupleLiteral *TupleLit);

    llvm::Value *visitMemberAccess(MemberAccess *MemberAcc);

    llvm::Value *visitTupleTypeDecl(TupleTypeDecl *TupleTypeDecl);

    llvm::Value *visitConditional(Conditional *Cond);

    llvm::Value *visitConditionalElse(ConditionalElse *Cond);

    llvm::Value *visitTypeCast(TypeCast *Cast);

    llvm::Value *visitBitwiseOp(BitwiseOp *Op);

    llvm::Value *visitUnaryOp(UnaryOp *Op);

    llvm::Value *visitArgsList(ArgsList *List);

    llvm::Value *visitParasList(ParasList *List);

    llvm::Value *visitFunctionDecl(FunctionDecl *FuncDecl);

    llvm::Value *visitFunctionDef(FunctionDef *FuncDef);

    llvm::Value *visitFunctionCall(FunctionCall *FuncCall);

    llvm::Value *visitProcedureDecl(ProcedureDecl *ProcedureDecl);

    llvm::Value *visitProcedureDef(ProcedureDef *ProcedureDef);

    llvm::Value *visitProcedureCall(ProcedureCall *ProcedureCall);

    llvm::Value *visitReturn(Return *Return);

    llvm::Value *visitBreak(Break *Break);

    llvm::Value *visitContinue(Continue *Continue);

    llvm::Value *visitOutStream(OutStream *OutStream);

    llvm::Value *visitInStream(InStream *InStream);

    llvm::Value *visitExplicitCast(ExplicitCast *ExplicitCast);

};


#endif //GAZPREABASE_CODEGENPASS_H
