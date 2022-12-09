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
    llvm::StructType *LLVMMatrixTy;
    llvm::StructType *LLVMIntervalTy;
    llvm::PointerType *LLVMVectorPtrTy;
    llvm::PointerType *LLVMMatrixPtrTy;

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
    llvm::FunctionCallee PowInt;
    llvm::FunctionCallee Malloc;
    llvm::FunctionCallee VectorNew;
    llvm::FunctionCallee VectorEmptyCopy;
    llvm::FunctionCallee VectorConcat;
    llvm::FunctionCallee VectorDotProductInt;
    llvm::FunctionCallee VectorDotProductReal;
    llvm::FunctionCallee VectorAccessInt;
    llvm::FunctionCallee VectorAccessFloat;
    llvm::FunctionCallee VectorAccessChar;
    llvm::FunctionCallee VectorSetInt;
    llvm::FunctionCallee VectorSetFloat;
    llvm::FunctionCallee VectorSetChar;
    llvm::FunctionCallee VectorViewScalar;
    llvm::FunctionCallee VectorViewVector;
    llvm::FunctionCallee VectorCreateDeepCopy;
    llvm::FunctionCallee VectorBy;
    llvm::FunctionCallee VectorNot;
    llvm::FunctionCallee VectorSub;
    llvm::FunctionCallee PrintVector;
    llvm::FunctionCallee PrintString;
    llvm::FunctionCallee VectorLogical;
    llvm::FunctionCallee VectorEq;
    llvm::FunctionCallee VectorArith;
    llvm::FunctionCallee VectorComp;
    llvm::FunctionCallee VectorOOB;
    llvm::FunctionCallee PrintMatrix;
    llvm::FunctionCallee MatrixNew;
    llvm::FunctionCallee MatrixEmptyFromVec;
    llvm::FunctionCallee MatrixPopulateRow;
    llvm::FunctionCallee MatrixAccessInt;
    llvm::FunctionCallee MatrixAccessFloat;
    llvm::FunctionCallee MatrixAccessChar;
    llvm::FunctionCallee MatrixSetChar;
    llvm::FunctionCallee MatrixSetInt;
    llvm::FunctionCallee MatrixSetFloat;
    llvm::FunctionCallee MatrixViewScalar;
    llvm::FunctionCallee MatrixViewVector;
    llvm::FunctionCallee MatrixViewMatrix;
    llvm::FunctionCallee MatrixNot;
    llvm::FunctionCallee MatrixSub;
    llvm::FunctionCallee MatrixArith;
    llvm::FunctionCallee MatrixComp;
    llvm::FunctionCallee MatrixLogical;
    llvm::FunctionCallee MatrixEq;
    llvm::FunctionCallee MatrixCreateDeepCopy;
    llvm::FunctionCallee MatrixCopy;
    llvm::FunctionCallee VectorCopy;
    llvm::FunctionCallee MatrixSetVector;
    llvm::FunctionCallee MatrixMul;
    llvm::FunctionCallee LengthBuiltIn;
    llvm::FunctionCallee RowBuiltIn;
    llvm::FunctionCallee ColBuiltIn;


    // Casting functions
    llvm::FunctionCallee GetSameVectorAs;
    llvm::FunctionCallee GetSameMatrixAs;
    llvm::FunctionCallee GetCastedVector;
    llvm::FunctionCallee GetCastedMatrix;
    llvm::FunctionCallee GetVectorWithValue;
    llvm::FunctionCallee GetMatrixWithValue;
    llvm::FunctionCallee GetIntVectorFromInterval;
    llvm::FunctionCallee GetRealVectorFromInterval;

    // Init runtime
    llvm::FunctionCallee InitRuntimeStream;


    // Filter functions
    llvm::FunctionCallee WriteValFromVecTo;
    llvm::FunctionCallee InitFilterExpr;
    llvm::FunctionCallee UpdateFilterAtPos;
    llvm::FunctionCallee FilterEndIteration;
    llvm::FunctionCallee GetCompletedFilterAt;
    llvm::FunctionCallee ShutdownFilterExprBuilder;

    // Copy ops
    llvm::FunctionCallee GetVectorCopy;
    llvm::FunctionCallee GetMatrixCopy;


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

    CodeGenPass() = delete;

    CodeGenPass(CodeGenPass&&) = default;

    CodeGenPass(const CodeGenPass&) = delete;

    explicit CodeGenPass(const char *OutFile) : GlobalCtx(), IR(GlobalCtx), Mod("gazprea", GlobalCtx), OutputFile(OutFile),
        LLVMIntTy(llvm::Type::getInt64Ty(GlobalCtx)), LLVMBoolTy(llvm::Type::getInt1Ty(GlobalCtx)),
        LLVMCharTy(llvm::Type::getInt8Ty(GlobalCtx)), LLVMRealTy(llvm::Type::getFloatTy(GlobalCtx)),
        LLVMVoidTy(llvm::Type::getVoidTy(GlobalCtx)), LLVMPtrTy(llvm::Type::getInt8PtrTy(GlobalCtx)),
        LLVMVectorTy(llvm::StructType::get(GlobalCtx, {LLVMIntTy, LLVMPtrTy, LLVMIntTy, LLVMPtrTy})),
        LLVMMatrixTy(llvm::StructType::get(GlobalCtx, {LLVMIntTy, LLVMIntTy, LLVMPtrTy, LLVMIntTy, LLVMPtrTy})),
        LLVMIntervalTy(llvm::StructType::get(GlobalCtx, {LLVMIntTy, LLVMIntTy})),
        LLVMVectorPtrTy(LLVMVectorTy->getPointerTo()), LLVMMatrixPtrTy(LLVMMatrixTy->getPointerTo()) {};

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
    llvm::Value *visitDomainLoop(DomainLoop *Loop);
    llvm::Value *visitGenerator(Generator *Gen);
    llvm::Value *visitMatrixGenerator(MatrixGenerator *Gen);
    llvm::Value *visitIntLiteral(IntLiteral *IntLit);
    static llvm::Value *visitNullLiteral(NullLiteral *NullLit);
    static llvm::Value *visitIdentityLiteral(IdentityLiteral *IdentityLit);
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
    llvm::Value *visitFilter(Filter *Flt);
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
    llvm::Value *visitStringLiteral(StringLiteral *String);
    llvm::Value *visitBlock(Block *Blk);
    llvm::Value *visitInterval(Interval *Interval);
    llvm::Value *visitConcat(Concat *Con);
    llvm::Value *visitDotProduct(DotProduct *Dot);
    llvm::Value *visitByOp(ByOp *By);
    llvm::Value *visitBuiltInLen(LengthFunc *Len);
    llvm::Value *visitBuiltInRow(RowFunc *Row);
    llvm::Value *visitBuiltInCol(ColFunc *Col);


    uint64_t TypeKindMapToVectorTypeInRuntime(Type::TypeKind Kind);
    llvm::Value *createAlloca(Type *Ty);
    llvm::Value *CreateVectorStruct(enum Type::TypeKind TyKind, uint64_t size, bool malloc = false);
    llvm::Value *CreateStringStruct(uint64_t size, bool malloc = false);
    llvm::Value *CreateVectorMallocPtrAccess(llvm::Value *VecPtr, const VectorTy *VecTy);
    llvm::Value *CreateVectorMallocPtrAccess(llvm::Value *VecPtr, VectorTy *VecTy);
    llvm::Value *CreateVectorPointerBitCast(llvm::Value *VecPtr, enum Type::TypeKind TyKind);
    llvm::Value *getCastValue(llvm::Value *Val, Type *SrcTy, Type *DestTy);
    llvm::Type *getLLVMTupleType(TupleTy *Tuple);
    llvm::Type *getLLVMFunctionType(FunctionTy *FuncTy);
    llvm::Type *getLLVMProcedureType(ProcedureTy *ProcTy);
    llvm::Type *getLLVMType(Type *Ty);
    llvm::Value *declareGlobal(const string &Name, Type *Ty);
    void assignGlobals();

    llvm::Function *getOrInsertFunction(Type *Ty, const string &Name);
};


#endif //GAZPREABASE_CODEGENPASS_H
