//
// Created by Chen on 2022-11-09.
//

#include <fstream>
#include "Passes/CodeGenPass.h"

#include "llvm/CodeGen/UnreachableBlockElim.h"

#include "llvm/Transforms/Utils/BasicBlockUtils.h"

using llvm::Value;

void CodeGenPass::runOnAST(ASTPassManager &Manager, ASTNodeT *Root) {
    assert(isa<Program>(Root) && "CodeGenPass should run on the entire program");
    PM = &Manager;

    // Set Runtime Functions
    llvm::FunctionType *MainTy = llvm::FunctionType::get(LLVMIntTy, false);

    PrintInt = Mod.getOrInsertFunction("rt_print_int",
                                       llvm::FunctionType::get(LLVMVoidTy, {LLVMIntTy}, false));
    PrintReal = Mod.getOrInsertFunction("rt_print_real",
                                        llvm::FunctionType::get(LLVMVoidTy, {LLVMRealTy}, false));
    PrintChar = Mod.getOrInsertFunction("rt_print_char",
                                        llvm::FunctionType::get(LLVMVoidTy, {LLVMCharTy}, false));
    PrintBool = Mod.getOrInsertFunction("rt_print_bool",
                                        llvm::FunctionType::get(LLVMVoidTy, {LLVMBoolTy}, false));
    ScanInt = Mod.getOrInsertFunction("rt_scan_int",
                                      llvm::FunctionType::get(LLVMIntTy, {}, false));
    ScanReal = Mod.getOrInsertFunction("rt_scan_real",
                                       llvm::FunctionType::get(LLVMRealTy, {}, false));
    ScanChar = Mod.getOrInsertFunction("rt_scan_char",
                                       llvm::FunctionType::get(LLVMCharTy, {}, false));
    ScanBool = Mod.getOrInsertFunction("rt_scan_bool",
                                       llvm::FunctionType::get(LLVMBoolTy, {}, false));

    visit(Root);

    // Dump the module to the output file.
    std::ofstream Out(OutputFile);
    llvm::raw_os_ostream OS(Out);
    OS << Mod;
}

llvm::Type *CodeGenPass::getLLVMType(const Type *Ty) {
    if (!Ty)
        return IR.getVoidTy();

    auto ConstConv = [&](llvm::Type *LLVMTy, bool IsConst) {
        if (IsConst)
            return LLVMTy;
        return cast<llvm::Type>(LLVMTy->getPointerTo());
    };

    switch (Ty->getKind()) {
        case Type::TypeKind::T_Bool:
            return ConstConv(LLVMBoolTy, Ty->isConst());
        case Type::TypeKind::T_Int:
            return ConstConv(LLVMIntTy, Ty->isConst());
        case Type::TypeKind::T_Real:
            return ConstConv(LLVMRealTy, Ty->isConst());
        case Type::TypeKind::T_Char:
            return ConstConv(LLVMCharTy, Ty->isConst());
        case Type::TypeKind::T_Tuple:
            return ConstConv(getLLVMTupleType(
                    cast<TupleTy>(Ty)), Ty->isConst());
        case Type::TypeKind::T_Function:
            return getLLVMFunctionType(cast<FunctionTy>(Ty));
        case Type::TypeKind::T_Procedure:
            return getLLVMProcedureType(cast<ProcedureTy>(Ty));
        default:
            assert(false && "Unknown type");
    }
}

llvm::Type *CodeGenPass::getLLVMTupleType(const TupleTy *Tuple) {
    vector<llvm::Type*> TupleTypes;
    for (const Type *SubTy : Tuple->getMemberTypes())
        TupleTypes.push_back(getLLVMType(PM->TypeReg.getConstTypeOf(SubTy)));
    return llvm::StructType::get(GlobalCtx, TupleTypes);
}

llvm::Value *CodeGenPass::createAlloca(const Type *Ty) {
    llvm::IRBuilder<> Builder(GlobalCtx);
    llvm::BasicBlock *BB = &CurrentFunction->front();
    Builder.SetInsertPoint(BB);
    return Builder.CreateAlloca(getLLVMType(Ty));
}

llvm::Value *CodeGenPass::createStructAlloca(llvm::StructType *Ty) {
    llvm::IRBuilder<> Builder(GlobalCtx);
    llvm::BasicBlock *BB = &CurrentFunction->front();
    Builder.SetInsertPoint(BB);
    return Builder.CreateAlloca(Ty);
}

llvm::Value *CodeGenPass::visitIdentifier(Identifier *Ident) {
    auto Val = SymbolMap[Ident->getReferred()];
    if (Val->getType()->isPointerTy())
        return IR.CreateLoad(Val);
    return Val;
}

llvm::Value *CodeGenPass::visitAssignment(Assignment *Assign) {
    auto *Val = visit(Assign->getExpr());
    auto *Loc = visit(Assign->getAssignedTo());
    return IR.CreateStore(Val, Loc);
}

llvm::Value *CodeGenPass::visitDeclaration(Declaration *Decl) {

    auto DeclType = Decl->getIdentifier()->getIdentType();

    if (isa<Program>(Decl->getParent())) {
        // These are global variables, that are only declared here, but later defined in the main function.
        auto GV = declareGlobal(Decl->getIdentifier()->getName(), DeclType);
        SymbolMap[Decl->getIdentifier()->getReferred()] = GV;
        GlobalDecls.push(Decl);
        return nullptr;
    }
    auto InitValue = visit(Decl->getInitExpr());
    // Declarations always get the space for the entire value.
    auto Loc = createAlloca(PM->TypeReg.getConstTypeOf(DeclType));
    IR.CreateStore(InitValue, Loc);
    SymbolMap[Decl->getIdentifier()->getReferred()] = Loc;
    return nullptr;
}

llvm::Value *CodeGenPass::declareGlobal(string name, const Type *Ty) {
    llvm::Type *LLTy = getLLVMType(PM->TypeReg.getConstTypeOf(Ty));
    Mod.getOrInsertGlobal(name, LLTy);
    llvm::GlobalVariable *GV = Mod.getNamedGlobal(name);
    GV->setInitializer(llvm::Constant::getNullValue(LLTy));
    return GV;
}

void CodeGenPass::assignGlobals() {
    // This function should be run at the beginning of the main procedure, to assign the values
    // of the global variables.

    while (!GlobalDecls.empty()) {
        auto Decl = GlobalDecls.front();
        GlobalDecls.pop();
        auto *Val = visit(Decl->getInitExpr());
        auto *Loc = SymbolMap[Decl->getIdentifier()->getReferred()];
        IR.CreateStore(Val, Loc);
    }

}


llvm::Value *CodeGenPass::visitComparisonOp(ComparisonOp *Op) {
    Value *LeftOperand = visit(Op->getLeftExpr());
    Value *RightOperand = visit(Op->getRightExpr());

    // Just an assertion, not needed for code gen.
    auto LeftType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getLeftExpr());
    auto RightType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getRightExpr());
    assert(RightType->isSameTypeAs(LeftType) && "Operation between different types should not"
                                     " have reached the code gen");

    llvm::CmpInst::Predicate Pred;

    if (isa<RealTy>(LeftType)) {
        switch (Op->getOpKind()) {
            case ComparisonOp::GT:
                Pred = llvm::CmpInst::Predicate::FCMP_OGT;
                break;
            case ComparisonOp::LT:
                Pred = llvm::CmpInst::Predicate::FCMP_OLT;
                break;
            case ComparisonOp::LTEQ:
                Pred = llvm::CmpInst::Predicate::FCMP_OLE;
                break;
            case ComparisonOp::GTEQ:
                Pred = llvm::CmpInst::Predicate::FCMP_OGE;
        }
        return IR.CreateFCmp(Pred, LeftOperand, RightOperand);
    }
    switch (Op->getOpKind()) {
        case ComparisonOp::GT:
            Pred = llvm::CmpInst::Predicate::ICMP_SGT;
            break;
        case ComparisonOp::LT:
            Pred = llvm::CmpInst::Predicate::ICMP_SLT;
            break;
        case ComparisonOp::LTEQ:
            Pred = llvm::CmpInst::Predicate::ICMP_SLE;
            break;
        case ComparisonOp::GTEQ:
            Pred = llvm::CmpInst::Predicate::ICMP_SGE;
            break;
    }
    return IR.CreateICmp(Pred, LeftOperand, RightOperand);
}

llvm::Value *CodeGenPass::visitArithmeticOp(ArithmeticOp *Op) {
    Value *LeftOperand = visit(Op->getLeftExpr());
    Value *RightOperand = visit(Op->getRightExpr());

    const Type *LeftType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getLeftExpr());
    const Type *RightType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getRightExpr());
    assert(RightType->isSameTypeAs(LeftType) && "Operation between different types should not"
                                     " have reached the code gen");

    auto RoundingMDS = llvm::MDString::get(GlobalCtx, "round.dynamic");
    auto ExceptionMDS = llvm::MDString::get(GlobalCtx, "fpexcept.strict");
    auto RoundingMD = llvm::MetadataAsValue::get(GlobalCtx, RoundingMDS);
    auto ExceptionMD = llvm::MetadataAsValue::get(GlobalCtx, ExceptionMDS);

    const Type *ResultType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op);
    if (!isa<RealTy>(ResultType)) {
        switch (Op->getOpKind()) {
            case ArithmeticOp::ADD:
                return IR.CreateAdd(LeftOperand, RightOperand);
            case ArithmeticOp::SUB:
                return IR.CreateSub(LeftOperand, RightOperand);
            case ArithmeticOp::MUL:
                return IR.CreateMul(LeftOperand, RightOperand);
            case ArithmeticOp::DIV:
                return IR.CreateSDiv(LeftOperand, RightOperand);
            case ArithmeticOp::MOD:
                return IR.CreateSRem(LeftOperand, RightOperand);
            case ArithmeticOp::EXP:
                LeftOperand = IR.CreateSIToFP(LeftOperand, LLVMRealTy);
                llvm::Value *RetVal = IR.CreateIntrinsic(
                        llvm::Intrinsic::experimental_constrained_powi,
                        {LLVMRealTy, LLVMIntTy, llvm::Type::getMetadataTy(GlobalCtx), llvm::Type::getMetadataTy(GlobalCtx)},
                        {LeftOperand, RightOperand, RoundingMD, ExceptionMD}
                );
                return IR.CreateFPToSI(RetVal, LLVMIntTy);

        }
    } else {
        llvm::Intrinsic::ID IntrinsicID;

        switch (Op->getOpKind()) {
            case ArithmeticOp::ADD:
                IntrinsicID = llvm::Intrinsic::experimental_constrained_fadd;
                break;
            case ArithmeticOp::SUB:
                IntrinsicID = llvm::Intrinsic::experimental_constrained_fsub;
                break;
            case ArithmeticOp::MUL:
                IntrinsicID = llvm::Intrinsic::experimental_constrained_fmul;
                break;
            case ArithmeticOp::DIV:
                IntrinsicID = llvm::Intrinsic::experimental_constrained_fdiv;
                break;
            case ArithmeticOp::MOD:
                IntrinsicID = llvm::Intrinsic::experimental_constrained_frem;
                break;
            case ArithmeticOp::EXP:
                return IR.CreateIntrinsic(
                        llvm::Intrinsic::experimental_constrained_pow,
                        {LLVMRealTy, LLVMRealTy, llvm::Type::getMetadataTy(GlobalCtx), llvm::Type::getMetadataTy(GlobalCtx)},
                        {LeftOperand, RightOperand, RoundingMD, ExceptionMD}
                );

        }
        return IR.CreateConstrainedFPBinOp(IntrinsicID, LeftOperand, RightOperand, nullptr, "", nullptr, llvm::fp::rmDynamic, llvm::fp::ebStrict);
    }
}

llvm::Value *CodeGenPass::visitLogicalOp(LogicalOp *Op) {
    Value *LeftOperand = visit(Op->getLeftExpr());
    Value *RightOperand = visit(Op->getRightExpr());

    const Type *LeftType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getLeftExpr());
    const Type *RightType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getRightExpr());
    assert( RightType->isSameTypeAs(LeftType) && "Operation between different types should not"
                                     " have reached the code gen");

    if (LeftType->isSameTypeAs(PM->TypeReg.getRealTy())) {
        switch (Op->getOpKind()) {
            case LogicalOp::EQ:
                return IR.CreateFCmpOEQ(LeftOperand, RightOperand);
            case LogicalOp::NEQ:
                return IR.CreateFCmpONE(LeftOperand, RightOperand);
            default:
                assert(false && "Invalid logical operation for real type");
        }
    }

    switch (Op->getOpKind()) {
        case LogicalOp::AND:
            return IR.CreateAnd(LeftOperand, RightOperand);
        case LogicalOp::OR:
            return IR.CreateOr(LeftOperand, RightOperand);
        case LogicalOp::XOR:
            return IR.CreateXor(LeftOperand, RightOperand);
        case LogicalOp::EQ:
            return IR.CreateICmpEQ(LeftOperand, RightOperand);
        case LogicalOp::NEQ:
            return IR.CreateICmpNE(LeftOperand, RightOperand);
    }
}

llvm::Value *CodeGenPass::visitUnaryOp(UnaryOp *Op) {
    Value *Operand = visit(Op->getExpr());

    switch (Op->getOpKind()) {
        case UnaryOp::NOT:
            return IR.CreateNot(Operand);
        case UnaryOp::ADD:
            return Operand;
        case UnaryOp::SUB:
            return IR.CreateNeg(Operand);
    }
}

llvm::Value *CodeGenPass::visitIndex(Index *Idx) {
    assert(false && "Indexing not implemented");
}

llvm::Value *CodeGenPass::visitInfiniteLoop(InfiniteLoop *Loop) {
    llvm::BasicBlock *LoopBody = llvm::BasicBlock::Create(GlobalCtx, "LoopBody", CurrentFunction);
    llvm::BasicBlock *LoopEnd = llvm::BasicBlock::Create(GlobalCtx, "LoopEnd", CurrentFunction);

    LoopBeginBlocks.push(LoopBody);
    LoopEndBlocks.push(LoopEnd);

    IR.CreateBr(LoopBody);

    IR.SetInsertPoint(LoopBody);
    visit(Loop->getBlock());
    IR.CreateBr(LoopBody);

    IR.SetInsertPoint(LoopEnd);

    LoopBeginBlocks.pop();
    LoopEndBlocks.pop();
    return nullptr;
}

llvm::Value *CodeGenPass::visitConditionalLoop(ConditionalLoop *Loop) {
    llvm::BasicBlock *Header = llvm::BasicBlock::Create(GlobalCtx, "LoopHeader", CurrentFunction);
    llvm::BasicBlock *LoopBody = llvm::BasicBlock::Create(GlobalCtx, "LoopBody");
    llvm::BasicBlock *LoopEnd = llvm::BasicBlock::Create(GlobalCtx, "LoopEnd", CurrentFunction);

    LoopBeginBlocks.push(Header);
    LoopEndBlocks.push(LoopEnd);

    if (Loop->ConditionalBefore)
        IR.CreateBr(Header);
    else
        IR.CreateBr(LoopBody);

    IR.SetInsertPoint(Header);
    auto Res = visit(Loop->getConditional());
    IR.CreateCondBr(Res, LoopBody, LoopEnd);


    CurrentFunction->getBasicBlockList().push_back(LoopBody);
    IR.SetInsertPoint(LoopBody);
    visit(Loop->getBlock());
    IR.CreateBr(Header);

    IR.SetInsertPoint(LoopEnd);

    LoopBeginBlocks.pop();
    LoopEndBlocks.pop();

    return nullptr;
}

// ignored for part1
llvm::Value *CodeGenPass::visitDomainLoop(DomainLoop *Loop) {
    assert(false && "DomainLoop not implemented");
}

llvm::Value *CodeGenPass::visitIntLiteral(IntLiteral *IntLit) {
    return IR.getInt32(IntLit->getVal());
}

llvm::Value *CodeGenPass::visitNullLiteral(NullLiteral *NullLit) {
    assert(false && "Should not have reached the codegen");
}

llvm::Value *CodeGenPass::visitIdentityLiteral(IdentityLiteral *IdentityLit) {
    assert(false && "Should not have reached the codegen");
}

llvm::Value *CodeGenPass::visitRealLiteral(RealLiteral *RealLit) {
    // TODO verify precision of float
    float Val = RealLit->getVal();
    llvm::APFloat APF(Val);
    return llvm::ConstantFP::get(GlobalCtx, APF);
}

llvm::Value *CodeGenPass::visitBoolLiteral(BoolLiteral *BoolLit) {
    return IR.getInt1(BoolLit->getVal());
}

llvm::Value *CodeGenPass::visitCharLiteral(CharLiteral *CharLit) {
    return IR.getInt8(CharLit->getCharacter());
}

llvm::Value *CodeGenPass::visitTupleLiteral(TupleLiteral *TupleLit) {
    auto TupLoc = createAlloca(
            PM->getAnnotation<ExprTypeAnnotatorPass>(TupleLit));
    int CurrIdx = 0;
    for (auto Child : *TupleLit) {
        auto MemberVal = visit(Child);
        auto MemLoc = IR.CreateGEP(
                TupLoc, {IR.getInt32(0), IR.getInt32(CurrIdx++)});
        IR.CreateStore(MemberVal, MemLoc);
    }
    return IR.CreateLoad(TupLoc);
}

llvm::Value *CodeGenPass::visitMemberAccess(MemberAccess *MemberAcc) {
    // All member expressions should be converted to a tuple access by an index
    // at this point
    auto MemberIdx = dyn_cast<IntLiteral>(MemberAcc->getMemberExpr())->getVal();
    auto Expr = visit(MemberAcc->getExpr());
    return IR.CreateExtractValue(Expr, MemberIdx-1);
}

llvm::Value *CodeGenPass::visitConditional(Conditional *Cond) {

    llvm::BasicBlock *CondHeader = llvm::BasicBlock::Create(GlobalCtx, "CondHeader", CurrentFunction);
    llvm::BasicBlock *CondTrue = llvm::BasicBlock::Create(GlobalCtx, "CondTrue");
    llvm::BasicBlock *CondFalse = llvm::BasicBlock::Create(GlobalCtx, "CondFalse");

    IR.CreateBr(CondHeader);
    IR.SetInsertPoint(CondHeader);
    Value *Res = visit(Cond->getConditional());
    IR.CreateCondBr(Res, CondTrue, CondFalse);
    // Generate code for the true block.
    CurrentFunction->getBasicBlockList().push_back(CondTrue);
    IR.SetInsertPoint(CondTrue);
    visit(Cond->getBlock());

    // Set insert point back to the original.
    CurrentFunction->getBasicBlockList().push_back(CondFalse);
    IR.CreateBr(CondFalse);
    IR.SetInsertPoint(CondFalse);

    return nullptr;
}

llvm::Value *CodeGenPass::visitConditionalElse(ConditionalElse *Cond) {

    llvm::BasicBlock *CondHeader = llvm::BasicBlock::Create(GlobalCtx, "CondHeader", CurrentFunction);
    llvm::BasicBlock *CondTrue = llvm::BasicBlock::Create(GlobalCtx, "CondTrue", CurrentFunction);
    llvm::BasicBlock *CondFalse = llvm::BasicBlock::Create(GlobalCtx, "CondFalse", CurrentFunction);
    llvm::BasicBlock *CondEnd = llvm::BasicBlock::Create(GlobalCtx, "CondEnd", CurrentFunction);

    IR.CreateBr(CondHeader);

    IR.SetInsertPoint(CondHeader);
    Value *Res = visit(Cond->getConditional());
    IR.CreateCondBr(Res, CondTrue, CondFalse);

    // Generate code for the true branch
    IR.SetInsertPoint(CondTrue);
    visit(Cond->getIfBlock());
    IR.CreateBr(CondEnd);

    // Generate code for the false branch.
    IR.SetInsertPoint(CondFalse);
    visit(Cond->getElseBlock());
    IR.CreateBr(CondEnd);

    IR.SetInsertPoint(CondEnd);

    return nullptr;
}

llvm::Value *CodeGenPass::getCastValue(Value *Val, const Type *SrcTy, const Type *DestTy) {

    // RULES FOR CASTING BASED ON DestTy

    if (SrcTy->isSameTypeAs(DestTy))
        return Val;

    switch (DestTy->getKind()) {
        case Type::TypeKind::T_Bool:
            return IR.CreateICmpNE(Val, llvm::Constant::getNullValue(
                    getLLVMType(PM->TypeReg.getConstTypeOf(SrcTy))));
        case Type::TypeKind::T_Char:
            // TODO fix char
            switch (SrcTy->getKind()) {
                case Type::TypeKind::T_Int:
                    return IR.CreateTrunc(Val, LLVMCharTy);
                case Type::TypeKind::T_Bool:
                    return IR.CreateZExt(Val, LLVMCharTy);
                default:
                    assert(false && "Invalid cast");
            }
        case Type::TypeKind::T_Int:
            switch (SrcTy->getKind()) {
                case Type::TypeKind::T_Char:
                case Type::TypeKind::T_Bool:
                    return IR.CreateZExt(Val, LLVMIntTy);
                case Type::TypeKind::T_Real:
                    return IR.CreateFPToSI(Val, LLVMIntTy);
                default:
                    assert(false && "Invalid cast");
            }
        case Type::TypeKind::T_Real:
            switch (SrcTy->getKind()) {
                case Type::TypeKind::T_Int:
                    return IR.CreateSIToFP(Val, LLVMRealTy);
                case Type::TypeKind::T_Char:
                case Type::TypeKind::T_Bool:
                    return IR.CreateUIToFP(Val, LLVMRealTy);
                default:
                    assert(false && "Invalid cast");
            }
        default:
            assert(false && "Invalid cast");
    }
}

llvm::Value *CodeGenPass::visitTypeCast(TypeCast *Cast) {
    return getCastValue(
            visit(Cast->getExpr()),
            PM->getAnnotation<ExprTypeAnnotatorPass>(Cast->getExpr()),
            PM->TypeReg.getConstTypeOf(Cast->getTargetType()));
}

llvm::Value *CodeGenPass::visitExplicitCast(ExplicitCast *Cast) {
    return getCastValue(
            visit(Cast->getExpr()),
            PM->getAnnotation<ExprTypeAnnotatorPass>(Cast->getExpr()),
            PM->TypeReg.getConstTypeOf(Cast->getTargetType()));
}

llvm::Value *CodeGenPass::visitFunctionDef(FunctionDef *Def) {

    auto FuncName = Def->getIdentifier()->getName();
    auto FuncTy = Def->getIdentifier()->getIdentType();
    auto Func = getOrInsertFunction(FuncTy, FuncName);

    // Create a new basic block to start insertion into
    llvm::BasicBlock *Entry = llvm::BasicBlock::Create(GlobalCtx, "FuncEntry", Func);
    llvm::BasicBlock *Body = llvm::BasicBlock::Create(GlobalCtx, "FuncBody", Func);
    IR.SetInsertPoint(Body);

    // Set function arguments and set them in the symbol map
    auto ParamList = Def->getParamList();
    for (auto I = 0; I < ParamList->numOfChildren(); I++) {
        auto Param = ParamList->getParamAt(I);
        SymbolMap[Param->getReferred()] = Func->getArg(I);
    }
    CurrentFunction = Func;


    // Visit function body
    visit(Def->getBlock());

    IR.CreateUnreachable();

    IR.SetInsertPoint(Entry);
    IR.CreateBr(Body);

}

llvm::Value *CodeGenPass::visitFunctionCall(FunctionCall *FuncCall) {

    // Get the function
    llvm::Function *Func = Mod.getFunction(FuncCall->getIdentifier()->getName());
    assert(Func && "Function not found");

    // Get the arguments
    std::vector<llvm::Value *> Args;
    for (auto Expr : *FuncCall->getArgsList())
        Args.push_back(visit(Expr));

    // Call the function
    return IR.CreateCall(Func, Args);
}

llvm::Value *CodeGenPass::visitProcedureDef(ProcedureDef *Def) {

    auto ProcName = Def->getIdentifier()->getName();
    auto ProcTy = Def->getIdentifier()->getIdentType();
    auto Proc = getOrInsertFunction(ProcTy, ProcName);

    // Create a new basic block to start insertion into
    llvm::BasicBlock *Entry = llvm::BasicBlock::Create(GlobalCtx, "ProcEntry", Proc);
    llvm::BasicBlock *Body = llvm::BasicBlock::Create(GlobalCtx, "ProcBody", Proc);

    IR.SetInsertPoint(Body);


    // Set function arguments and set them in the symbol map
    auto ParamList = Def->getParamList();
    for (auto I = 0; I < ParamList->numOfChildren(); I++) {
        auto Param = ParamList->getParamAt(I);
        SymbolMap[Param->getReferred()] = Proc->getArg(I);
    }
    CurrentFunction = Proc;

    if (ProcName == "main") {
        assignGlobals();
    }

    // Visit function body
    visit(Def->getBlock());

    IR.CreateUnreachable();
    
    IR.SetInsertPoint(Entry);
    IR.CreateBr(Body);

    CurrentFunction = GlobalFunction;

}

llvm::Value *CodeGenPass::visitProcedureCall(ProcedureCall *Call) {
    // TODO: Handle stream_state;
    // Get the function
    llvm::Function *Func = Mod.getFunction(Call->getIdentifier()->getName());
    assert(Func && "Function not found");

    // Get the arguments
    std::vector<llvm::Value *> Args;
    for (auto Child : *Call->getArgsList())
        Args.emplace_back(visit(Child));

    return IR.CreateCall(Func, Args);
}

llvm::Value *CodeGenPass::visitReturn(Return *Return) {
    // No-op means that the procedure does not have a return type.
    if (isa<NoOp>(Return->getReturnExpr()))
        IR.CreateRetVoid();
    else
        IR.CreateRet(visit(Return->getReturnExpr()));

    llvm::BasicBlock *AfterRet = llvm::BasicBlock::Create(GlobalCtx, "AfterRet", CurrentFunction);
    IR.SetInsertPoint(AfterRet);
}

llvm::Value *CodeGenPass::visitBreak(Break *Break) {
    llvm::BasicBlock *AfterBreak = llvm::BasicBlock::Create(GlobalCtx, "AfterBreak", CurrentFunction);
    llvm::BasicBlock *LoopEnd = LoopEndBlocks.top();

    IR.CreateBr(LoopEnd);

    IR.SetInsertPoint(AfterBreak);
    return nullptr;
}

llvm::Value *CodeGenPass::visitContinue(Continue *Continue) {

    llvm::BasicBlock *AfterContinue = llvm::BasicBlock::Create(GlobalCtx, "AfterContinue", CurrentFunction);
    llvm::BasicBlock *LoopEnd = LoopBeginBlocks.top();

    IR.CreateBr(LoopEnd);
    IR.SetInsertPoint(AfterContinue);
    return nullptr;
}

llvm::Value *CodeGenPass::visitOutStream(OutStream *Stream) {
    Value *ValToOut = visit(Stream->getOutStreamExpr());
    const Type *ValType = PM->getAnnotation<ExprTypeAnnotatorPass>(Stream->getOutStreamExpr());
    assert(ValType->isOutputTy() && "Invalid output stream type");
    switch (ValType->getKind()) {
        case Type::TypeKind::T_Char:
            return IR.CreateCall(PrintChar, {ValToOut});
        case Type::TypeKind::T_Int:
            return IR.CreateCall(PrintInt, {ValToOut});
        case Type::TypeKind::T_Bool:
            return IR.CreateCall(PrintBool, {ValToOut});
        case Type::TypeKind::T_Real:
            return IR.CreateCall(PrintReal, {ValToOut});
        default:
            assert(false && "Invalid type for out-stream");
    }
}

llvm::Value *CodeGenPass::visitInStream(InStream *InStream) {
    const Type *IdentTy = PM->getAnnotation<ExprTypeAnnotatorPass>(InStream->getTarget());
    assert(IdentTy->isInputTy() && "Invalid input stream type");
    Value *StoreLoc = visit(InStream->getTarget());
    Value *ReadVal;

    switch (IdentTy->getKind()) {
        case Type::TypeKind::T_Char:
            ReadVal = IR.CreateCall(ScanChar);
            break;
        case Type::TypeKind::T_Int:
            ReadVal = IR.CreateCall(ScanInt);
            break;
        case Type::TypeKind::T_Bool:
            ReadVal = IR.CreateCall(ScanBool);
            break;
        case Type::TypeKind::T_Real:
            ReadVal = IR.CreateCall(ScanReal);
            break;
        default:
            assert(false && "Invalid type for in-stream");
    }
    IR.CreateStore(ReadVal, StoreLoc);
    return nullptr;

}

llvm::Type *CodeGenPass::getLLVMFunctionType(const FunctionTy *FuncTy) {
    vector<llvm::Type*> ParamTypes;
    for (auto Ty: FuncTy->getParamTypes())
        ParamTypes.emplace_back(getLLVMType(Ty));
    return llvm::cast<llvm::Type>(
            llvm::FunctionType::get(getLLVMType(FuncTy->getRetType()), ParamTypes, false));
}

llvm::Type *CodeGenPass::getLLVMProcedureType(const ProcedureTy *ProcTy) {
    vector<llvm::Type*> ParamTypes;
    for (auto Ty: ProcTy->getParamTypes())
        ParamTypes.emplace_back(getLLVMType(Ty));
    return llvm::cast<llvm::Type>(
            llvm::FunctionType::get(getLLVMType(ProcTy->getRetTy()), ParamTypes, false));

}

llvm::Value *CodeGenPass::visitIdentReference(IdentReference *Ref) {
    return SymbolMap[Ref->getIdentifier()->getReferred()];
}

llvm::Value *CodeGenPass::visitMemberReference(MemberReference *Ref) {
    auto MemIdx = dyn_cast<IntLiteral>(Ref->getMemberExpr());
    assert(MemIdx && "Only int literals should reach here");
    auto StructLoc = SymbolMap[Ref->getIdentifier()->getReferred()];
    auto MemLoc = IR.CreateGEP(StructLoc, {IR.getInt32(0), IR.getInt32(MemIdx->getVal() - 1)});
    return MemLoc;
}

llvm::Function *CodeGenPass::getOrInsertFunction(const Type *Ty, const string &Name) {

    if (auto Func = Mod.getFunction(Name))
        return Func;

    auto FuncTy = dyn_cast<FunctionTy>(Ty);
    auto ProcTy = dyn_cast<ProcedureTy>(Ty);
    assert(ProcTy || FuncTy);
    vector<const Type *> ParamTys;
    llvm::Type *RetTy;
    if (FuncTy) {
        ParamTys = FuncTy->getParamTypes();
        RetTy = getLLVMType(FuncTy->getRetType());
    }
    else {
        ParamTys = ProcTy->getParamTypes();
        RetTy = getLLVMType(ProcTy->getRetTy());
    }

    vector<llvm::Type*> LLVMParamTys;
    auto BuildLLVMTypes = [&](const Type *T) {
        LLVMParamTys.emplace_back(getLLVMType(T));};
    std::for_each(ParamTys.begin(), ParamTys.end(), BuildLLVMTypes);

    auto LLVMFuncTy = llvm::FunctionType::get(RetTy, LLVMParamTys, false);
    return llvm::Function::Create(LLVMFuncTy, llvm::Function::ExternalLinkage,
                                  Name, Mod);

}

llvm::Value *CodeGenPass::visitFunctionDecl(FunctionDecl *Decl) {
    auto FuncName = Decl->getIdentifier()->getName();
    auto FuncTy = Decl->getIdentifier()->getIdentType();
    return getOrInsertFunction(FuncTy, FuncName);
}

llvm::Value *CodeGenPass::visitProcedureDecl(ProcedureDecl *Decl) {
    auto ProcName = Decl->getIdentifier()->getName();
    auto ProcTy = Decl->getIdentifier()->getIdentType();
    return getOrInsertFunction(ProcTy, ProcName);
}

llvm::Value *CodeGenPass::visitBlock(Block *Blk) {
    for (auto Child: *Blk)
        visit(Child);
}