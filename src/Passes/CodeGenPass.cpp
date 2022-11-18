//
// Created by Chen on 2022-11-09.
//

#include <fstream>
#include "Passes/CodeGenPass.h"

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
                                      llvm::FunctionType::get(LLVMVoidTy, {LLVMPtrTy, LLVMPtrTy}, false));
    ScanReal = Mod.getOrInsertFunction("rt_scan_real",
                                       llvm::FunctionType::get(LLVMVoidTy, {LLVMPtrTy, LLVMPtrTy}, false));
    ScanChar = Mod.getOrInsertFunction("rt_scan_char",
                                       llvm::FunctionType::get(LLVMVoidTy, {LLVMPtrTy, LLVMPtrTy}, false));
    ScanBool = Mod.getOrInsertFunction("rt_scan_bool",
                                       llvm::FunctionType::get(LLVMVoidTy, {LLVMPtrTy, LLVMPtrTy}, false));
    llvm::Function *MainProd = getMainProcProto();

    GlobalFunction = llvm::Function::Create(
            MainTy, llvm::Function::ExternalLinkage, "main", Mod);

    llvm::BasicBlock *Entry = llvm::BasicBlock::Create(GlobalCtx, "entry", GlobalFunction);
    IR.SetInsertPoint(Entry);

    // Set the current function to the global function (for global variables)
    CurrentFunction = GlobalFunction;

    // Create the buffer pointer
    llvm::StructType *BufferTy = llvm::StructType::create(GlobalCtx);
    BufferTy->setBody({
        LLVMIntTy, LLVMIntTy, LLVMIntTy, LLVMIntTy, LLVMIntTy, llvm::ArrayType::get(LLVMCharTy, 1025)
    });

    // get pointer to the first element of the buffer
    BufferPtr = IR.CreateAlloca(BufferTy, nullptr, "buffer");
    BufferPtr = IR.CreateStructGEP(BufferTy, BufferPtr, 0, "buffer_ptr_ptr");


    // TODO check for main function existing (in error handling)
    llvm::Value *RetVal = IR.CreateCall(MainProd, {});
    IR.CreateRet(RetVal);

    visit(Root);

    // Dump the module to the output file.
    std::ofstream Out(OutputFile);
    llvm::raw_os_ostream OS(Out);
    OS << Mod;
}

llvm::Type *CodeGenPass::getLLVMType(const Type *Ty, bool constPtrCheck) {
    if (!Ty)
        return IR.getVoidTy();

    auto ConstConv = [&](llvm::Type *LLVMTy, bool IsConst) {
        if (IsConst || !constPtrCheck)
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
    return Builder.CreateAlloca(getLLVMType(Ty, false));
}

llvm::Value *CodeGenPass::createStructAlloca(llvm::StructType *Ty) {
    llvm::IRBuilder<> Builder(GlobalCtx);
    llvm::BasicBlock *BB = &CurrentFunction->front();
    Builder.SetInsertPoint(BB);
    return Builder.CreateAlloca(Ty);
}

llvm::Value *CodeGenPass::visitIdentifier(Identifier *Ident) {
    return IR.CreateLoad(SymbolMap[Ident->getReferred()]);
}

llvm::Value *CodeGenPass::visitAssignment(Assignment *Assign) {
    Value *StoreVal = visit(Assign->getExpr());
    // TODO: FIX ME.
//    Value *StoreLoc = SymbolMap[Assign->getIdentifier()->getReferred()];
    // All assignments, including tuple assignments, are lowered to store assignments
//    IR.CreateStore(StoreVal, StoreLoc);
    return nullptr;
}

llvm::Value *CodeGenPass::visitDeclaration(Declaration *Decl) {
    Value *InitValue = visit(Decl->getInitExpr());
    auto DeclType = PM->getAnnotation<ExprTypeAnnotatorPass>(Decl->getInitExpr());
    Value *DeclValue = createAlloca(DeclType);
    IR.CreateStore(InitValue, DeclValue);
    SymbolMap[Decl->getIdentifier()->getReferred()] = DeclValue;
    return nullptr;
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

    if (LeftType->isSameTypeAs(PM->TypeReg.getRealTy())) {
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
            break;
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
    llvm::BasicBlock *LoopBody = llvm::BasicBlock::Create(GlobalCtx, "LoopBody", CurrentFunction);
    llvm::BasicBlock *LoopEnd = llvm::BasicBlock::Create(GlobalCtx, "LoopEnd", CurrentFunction);

    LoopBeginBlocks.push(Header);
    LoopEndBlocks.push(LoopEnd);

    if (Loop->ConditionalBefore)
        IR.CreateBr(Header);
    else
        IR.CreateBr(LoopBody);

    IR.SetInsertPoint(Header);
    Value *Res = visit(Loop->getConditional());
    IR.CreateCondBr(Res, LoopBody, LoopEnd);

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
    int MemberIdx = dyn_cast<IntLiteral>(MemberAcc->getMemberExpr())->getVal();
    auto Expr = visit(MemberAcc->getExpr());
    auto MemberPtr = IR.CreateExtractElement(Expr, MemberIdx-1);
    return IR.CreateLoad(MemberPtr);
}

llvm::Value *CodeGenPass::visitConditional(Conditional *Cond) {

    llvm::BasicBlock *CondHeader = llvm::BasicBlock::Create(GlobalCtx, "CondHeader", CurrentFunction);
    llvm::BasicBlock *CondIf = llvm::BasicBlock::Create(GlobalCtx, "CondIf", CurrentFunction);
    llvm::BasicBlock *CondEnd = llvm::BasicBlock::Create(GlobalCtx, "CondEnd", CurrentFunction);

    IR.SetInsertPoint(CondHeader);
    Value *Res = visit(Cond->getConditional());
    IR.CreateCondBr(Res, CondIf, CondEnd);

    CurrentFunction->getBasicBlockList().push_back(CondIf);
    IR.SetInsertPoint(CondIf);
    visit(Cond->getBlock());
    IR.CreateBr(CondEnd);

    CurrentFunction->getBasicBlockList().push_back(CondEnd);
    IR.SetInsertPoint(CondEnd);

    return nullptr;
}

llvm::Value *CodeGenPass::visitConditionalElse(ConditionalElse *Cond) {

    llvm::BasicBlock *CondHeader = llvm::BasicBlock::Create(GlobalCtx, "CondHeader", CurrentFunction);
    llvm::BasicBlock *CondIf = llvm::BasicBlock::Create(GlobalCtx, "CondIf", CurrentFunction);
    llvm::BasicBlock *CondElse = llvm::BasicBlock::Create(GlobalCtx, "CondElse", CurrentFunction);
    llvm::BasicBlock *CondEnd = llvm::BasicBlock::Create(GlobalCtx, "CondEnd", CurrentFunction);

    IR.SetInsertPoint(CondHeader);
    Value *Res = visit(Cond->getConditional());
    IR.CreateCondBr(Res, CondIf, CondElse);

    CurrentFunction->getBasicBlockList().push_back(CondIf);
    IR.SetInsertPoint(CondIf);
    visit(Cond->getIfBlock());
    IR.CreateBr(CondEnd);

    CurrentFunction->getBasicBlockList().push_back(CondElse);
    IR.SetInsertPoint(CondElse);
    visit(Cond->getElseBlock());
    IR.CreateBr(CondEnd);

    CurrentFunction->getBasicBlockList().push_back(CondEnd);
    IR.SetInsertPoint(CondEnd);

    return nullptr;
}

llvm::Value *CodeGenPass::getCastValue(Value *Val, const Type *SrcTy, const Type *DestTy) {

    // RULES FOR CASTING BASED ON DestTy

    switch (DestTy->getKind()) {
        case Type::TypeKind::T_Bool:
            return IR.CreateICmpNE(Val, llvm::Constant::getNullValue(getLLVMType(SrcTy)));
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
                    return IR.CreateFPTrunc(Val, LLVMIntTy);
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
            Cast->getTargetType());
}

llvm::Value *CodeGenPass::visitExplicitCast(ExplicitCast *Cast) {
    return getCastValue(
            visit(Cast->getExpr()),
            PM->getAnnotation<ExprTypeAnnotatorPass>(Cast->getExpr()),
            Cast->getTargetType());
}

llvm::Value *CodeGenPass::visitFunctionDef(FunctionDef *FuncDef) {

    // Get arg types
    std::vector<llvm::Type *> ParamTypes;
    for (size_t i = 0; i < FuncDef->getParamList()->numOfChildren(); i++) {
        Identifier *Ident = FuncDef->getParamList()->getParamAt(i);
        const Type *IdentTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Ident);
        assert(IdentTy->isConst() && "Non-const argument to function call");
        // All args are passed by value as they are const
        ParamTypes.push_back(getLLVMType(IdentTy));
    }

    // Get function type
    llvm::FunctionType *FuncTy = llvm::FunctionType::get(
            getLLVMType(FuncDef->getRetTy()),
            ParamTypes,
            false);

    // Define a function
    llvm::Function *Func = llvm::Function::Create(
            FuncTy,
            llvm::Function::ExternalLinkage,
            "fn_" + FuncDef->getIdentifier()->getName(),
            Mod);

    // Create a new basic block to start insertion into
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(GlobalCtx, "FuncEntry", Func);
    IR.SetInsertPoint(BB);

    // Set function arguments and set them in the symbol map
    size_t i = 0;
    for (auto &Arg : Func->args()) {
        Arg.setName(FuncDef->getParamList()->getParamAt(i)->getName());
        // Allocate area for the argument
        // This is a redundant step, but will be eliminated with the right pass
        llvm::AllocaInst *Alloca = IR.CreateAlloca(getLLVMType(PM->getAnnotation<ExprTypeAnnotatorPass>(FuncDef->getParamList()->getParamAt(i))), nullptr, Arg.getName());
        IR.CreateStore(&Arg, Alloca);
        // Set the argument in the symbol map
        SymbolMap[FuncDef->getParamList()->getParamAt(i)->getReferred()] = Alloca;
        i++;
    }

    // Set current function
    CurrentFunction = Func;

    // Visit function body
    visit(FuncDef->getBlock());

    CurrentFunction = GlobalFunction;

    // The return is defined inside the function body

}

llvm::Value *CodeGenPass::visitFunctionCall(FunctionCall *FuncCall) {

    // Get the function
    llvm::Function *Func = Mod.getFunction(FuncCall->getIdentifier()->getName());
    assert(Func && "Function not found");

    // Get the arguments
    std::vector<llvm::Value *> Args;
    for (size_t i = 0; i < FuncCall->getArgsList()->numOfChildren(); i++) {
        ASTNodeT *Expr = FuncCall->getArgsList()->getExprAtPos(i);
        const Type *ExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Expr);
        assert(ExprTy->isConst() && "Non-const argument to function call");
        // All args are passed by value as they are const
        Args.push_back(visit(Expr));
    }

    // Call the function
    return IR.CreateCall(Func, Args);
}

llvm::Value *CodeGenPass::visitProcedureDef(ProcedureDef *ProcedureDef) {

    llvm::Function *Func = Mod.getFunction("pd_" + ProcedureDef->getIdentifier()->getName());
    if (!Func) {

        // Get arg types
        std::vector<llvm::Type *> ParamTypes;
        for (size_t i = 0; i < ProcedureDef->getParamList()->numOfChildren(); i++) {
            Identifier *Ident = ProcedureDef->getParamList()->getParamAt(i);
            const Type *IdentTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Ident);

            // Constant arguments are passed by value
            if (IdentTy->isConst()) {
                ParamTypes.push_back(getLLVMType(IdentTy));

                // Variable arguments are passed by reference
            } else {
                ParamTypes.push_back(llvm::PointerType::get(getLLVMType(IdentTy), 0));
            }
        }

        // Get function type
        llvm::FunctionType *ProcedureTy = llvm::FunctionType::get(
                getLLVMType(ProcedureDef->getRetTy()),
                ParamTypes,
                false);

        // Define a function
        Func = llvm::Function::Create(
                ProcedureTy,
                llvm::Function::ExternalLinkage,
                "pd_" + ProcedureDef->getIdentifier()->getName(),
                Mod);
    }
    // Create a new basic block to start insertion into
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(GlobalCtx, "ProcEntry", Func);
    IR.SetInsertPoint(BB);

    // Set function arguments and set them in the symbol map
    size_t i = 0;
    for (auto &Arg : Func->args()) {
        Identifier *Ident = ProcedureDef->getParamList()->getParamAt(i);
        const Type *IdentTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Ident);
        Arg.setName(Ident->getName());
        if (IdentTy->isConst()) {
            // Allocate area for the argument
            // This is a redundant step, but will be eliminated with the right pass
            llvm::AllocaInst *Alloca = IR.CreateAlloca(getLLVMType(PM->getAnnotation<ExprTypeAnnotatorPass>(Ident)), nullptr, Arg.getName());
            IR.CreateStore(&Arg, Alloca);
            // Set the argument in the symbol map
            SymbolMap[Ident->getReferred()] = Alloca;
        } else {
            // Set the argument in the symbol map
            // TODO confirm that this memory manipulation is allowed
            SymbolMap[Ident->getReferred()] = &Arg;
        }
        i++;
    }



    // Set current function
    CurrentFunction = Func;

    // Visit function body
    visit(ProcedureDef->getBlock());

    CurrentFunction = GlobalFunction;

    // The return is defined inside the function body
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
        return IR.CreateRetVoid();
    return IR.CreateRet(visit(Return->getReturnExpr()));
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

    CurrentFunction->getBasicBlockList().push_back(AfterContinue);
    IR.SetInsertPoint(AfterContinue);
    return nullptr;

}

llvm::Value *CodeGenPass::visitOutStream(OutStream *Stream) {
    Value *ValToOut = visit(Stream->getOutStreamExpr());
    const Type *ValType = PM->getAnnotation<ExprTypeAnnotatorPass>(Stream->getOutStreamExpr());
    assert(ValType->isOutputTy() && "Invalid output stream type");
    switch (ValType->getKind()) {
        case Type::TypeKind::T_Char:
            IR.CreateCall(PrintChar, {ValToOut});
            break;
        case Type::TypeKind::T_Int:
            IR.CreateCall(PrintInt, {ValToOut});
            break;
        case Type::TypeKind::T_Bool:
            IR.CreateCall(PrintBool, {ValToOut});
            break;
        case Type::TypeKind::T_Real:
            IR.CreateCall(PrintReal, {ValToOut});
            break;
        default:
            assert(false && "Invalid type for outstream");
    }
    return nullptr;
}

llvm::Value *CodeGenPass::visitInStream(InStream *InStream) {
    const Type *IdentTy = PM->getAnnotation<ExprTypeAnnotatorPass>(InStream->getTarget());
    assert(IdentTy->isInputTy() && "Invalid input stream type");
    Value *StoreLoc = visit(InStream->getTarget());

    switch (IdentTy->getKind()) {
        case Type::TypeKind::T_Char:
            IR.CreateCall(ScanChar, {StoreLoc, BufferPtr});
            break;
        case Type::TypeKind::T_Int:
            IR.CreateCall(ScanInt, {StoreLoc, BufferPtr});
            break;
        case Type::TypeKind::T_Bool:
            IR.CreateCall(ScanBool, {StoreLoc, BufferPtr});
            break;
        case Type::TypeKind::T_Real:
            IR.CreateCall(ScanReal, {StoreLoc, BufferPtr});
            break;
        default:
            assert(false && "Invalid type for outstream");
    }
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

llvm::Function *CodeGenPass::getMainProcProto() {
    llvm::FunctionType *FT = llvm::FunctionType::get(LLVMIntTy, {}, false);
    return llvm::Function::Create(FT, llvm::Function::ExternalLinkage, "pd_main", &Mod);
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
