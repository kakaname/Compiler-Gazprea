//
// Created by Chen on 2022-11-09.
//

#ifndef GAZPREABASE_CODEGENPASS_H
#define GAZPREABASE_CODEGENPASS_H

#include <map>
#include <vector>
#include <stack>
#include <queue>

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_os_ostream.h"

#include "Passes/PassManager.h"
#include "Passes/VisitorPass.h"
#include "Passes/BuildAST/ExprTypeAnnotatorPass.h"
#include "Symbol/Symbol.h"


// NOTE: all pointers are stored as i8* in the IR and bitcasted to the correct type when used

struct CodeGenPass: public VisitorPass<CodeGenPass, llvm::Value*> {
    using AnnotationT = llvm::Value*;

    ASTPassManager *PM{};

    llvm::LLVMContext GlobalCtx;
    llvm::IRBuilder<> IR;
    llvm::Module Mod;

    llvm::Type *LLVMIntTy;
    llvm::Type *LLVMBoolTy;
    llvm::Type *LLVMCharTy;
    llvm::Type *LLVMRealTy;
    llvm::Type *LLVMVoidTy;
    llvm::PointerType *LLVMPtrTy;
    llvm::StructType *LLVMVectorTy;

    llvm::StructType *LLVMIntervalTy;

    llvm::Function *CurrentFunction{};
    llvm::Function *GlobalFunction{};
    llvm::Function *MainFunction{};

    // Runtime functions
    llvm::FunctionCallee PrintInt;
    llvm::FunctionCallee PrintReal;
    llvm::FunctionCallee PrintChar;
    llvm::FunctionCallee PrintBool;
    llvm::FunctionCallee ScanInt;
    llvm::FunctionCallee ScanReal;
    llvm::FunctionCallee ScanChar;
    llvm::FunctionCallee ScanBool;
    llvm::FunctionCallee Malloc;
    llvm::FunctionCallee VectorConcat;
    llvm::FunctionCallee VectorDotProductInt;
    llvm::FunctionCallee VectorDotProductReal;
    llvm::FunctionCallee VectorBy;
    llvm::FunctionCallee VectorNot;
    llvm::FunctionCallee VectorSub;
    llvm::FunctionCallee PrintVector;
    llvm::FunctionCallee VectorLogical;
    llvm::FunctionCallee VectorEq;
    llvm::FunctionCallee VectorArith;
    llvm::FunctionCallee VectorComp;

    // Runtime buffer location
    llvm::Value *BufferPtr;

    // Use to keep track of which llvm values represents which symbols in the
    // program.
    map<const Symbol*, llvm::Value*> SymbolMap;

    // Used to keep track of which loop we are currently in for breaks/continues
    std::stack<llvm::BasicBlock*> LoopEndBlocks;
    std::stack<llvm::BasicBlock*> LoopBeginBlocks;

    // Used to keep track of global declarations that later will
    // be assigned in the main function
    std::queue<Declaration*> GlobalDecls;

    // The file to dump the outputs to.
    const char *OutputFile;

    explicit CodeGenPass(const char *OutFile) : GlobalCtx(), IR(GlobalCtx), Mod("gazprea", GlobalCtx), OutputFile(OutFile),
        LLVMIntTy(llvm::Type::getInt32Ty(GlobalCtx)), LLVMBoolTy(llvm::Type::getInt1Ty(GlobalCtx)),
        LLVMCharTy(llvm::Type::getInt8Ty(GlobalCtx)), LLVMRealTy(llvm::Type::getFloatTy(GlobalCtx)),
        LLVMVoidTy(llvm::Type::getVoidTy(GlobalCtx)), LLVMPtrTy(llvm::Type::getInt8PtrTy(GlobalCtx)),
        LLVMVectorTy(llvm::StructType::get(GlobalCtx, {LLVMIntTy, LLVMIntTy, LLVMIntTy, LLVMPtrTy})), LLVMIntervalTy(llvm::StructType::get(GlobalCtx, {LLVMIntTy, LLVMIntTy})) {}

    void runOnAST(ASTPassManager &Manager, ASTNodeT *Root);

    llvm::Value *visitIdentifier(Identifier *Ident);
    llvm::Value *visitAssignment(Assignment *Assign);
    llvm::Value *visitDeclaration(Declaration *Decl);
    llvm::Value *visitComparisonOp(ComparisonOp *Op);

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
    llvm::Value *visitConditional(Conditional *Cond);
    llvm::Value *visitConditionalElse(ConditionalElse *Cond);
    llvm::Value *visitTypeCast(TypeCast *Cast);
    llvm::Value *visitExplicitCast(ExplicitCast *ExplicitCast);
    llvm::Value *visitUnaryOp(UnaryOp *Op);
    llvm::Value *visitFunctionDef(FunctionDef *FuncDef);
    llvm::Value *visitFunctionDecl(FunctionDecl *Decl);
    llvm::Value *visitProcedureDecl(ProcedureDecl *Decl);
    llvm::Value *visitFunctionCall(FunctionCall *FuncCall);
    llvm::Value *visitProcedureDef(ProcedureDef *Def);
    llvm::Value *visitProcedureCall(ProcedureCall *ProcedureCall);
    llvm::Value *visitReturn(Return *Return);
    llvm::Value *visitBreak(Break *Break);
    llvm::Value *visitContinue(Continue *Continue);
    llvm::Value *visitOutStream(OutStream *OutStream);
    llvm::Value *visitInStream(InStream *InStream);
    llvm::Value *visitIdentReference(IdentReference *Ref);
    llvm::Value *visitIndexReference(IndexReference *Ref);
    llvm::Value *visitMemberReference(MemberReference *Ref);
    llvm::Value *visitVectorLiteral(VectorLiteral *VecLit);
    llvm::Value *visitBlock(Block *Blk);
    llvm::Value *visitInterval(Interval *Interval);
    llvm::Value *visitConcat(Concat *Con);
    llvm::Value *visitDotProduct(DotProduct *Dot);
    llvm::Value *visitByOp(ByOp *By);

    llvm::Value *createAlloca(const Type *Ty);
    llvm::Value *CreateVectorStruct(enum Type::TypeKind TyKind, uint32_t size, bool malloc = false);
    llvm::Value *CreateVectorMallocPtrAccess(llvm::Value *VecPtr, const VectorTy *VecTy);
    llvm::Value *CreateVectorPointerBitCast(llvm::Value *VecPtr, enum Type::TypeKind TyKind);
    llvm::Value *getCastValue(llvm::Value *Val, const Type *SrcTy, const Type *DestTy);
    llvm::Type *getLLVMTupleType(const TupleTy *Tuple);
    llvm::Type *getLLVMFunctionType(const FunctionTy *FuncTy);
    llvm::Type *getLLVMProcedureType(const ProcedureTy *ProcTy);
    llvm::Type *getLLVMVectorType(const VectorTy *VecTy);
    llvm::Type *getLLVMType(const Type *Ty);
    llvm::Value *declareGlobal(const string &Name, const Type *Ty);
    void assignGlobals();

    llvm::Function *getOrInsertFunction(const Type *Ty, const string &Name);
};


#endif //GAZPREABASE_CODEGENPASS_H
