//
// Created by Chen on 2022-11-09.
//

#include <fstream>
#include "Codegen/CodeGenPass.h"
#include "Passes/Transformations/SubExpressionCacheSet.h"
#include "Codegen/CastOperation.h"

using llvm::Value;

void CodeGenPass::runOnAST(ASTPassManager &Manager, ASTNodeT *Root) {
    assert(isa<Program>(Root) && "CodeGenPass should run on the entire program");
    PM = &Manager;

    // Set Runtime Functions

    PrintInt = Mod.getOrInsertFunction(
            "rt_print_int", llvm::FunctionType::get(
                    LLVMVoidTy, {LLVMIntTy}, false));

    PrintReal = Mod.getOrInsertFunction(
            "rt_print_real", llvm::FunctionType::get(
                    LLVMVoidTy, {LLVMRealTy}, false));

    PrintChar = Mod.getOrInsertFunction(
            "rt_print_char", llvm::FunctionType::get(
                    LLVMVoidTy, {LLVMCharTy}, false));

    PrintBool = Mod.getOrInsertFunction(
            "rt_print_bool", llvm::FunctionType::get(
                    LLVMVoidTy, {LLVMBoolTy}, false));

    ScanInt = Mod.getOrInsertFunction(
            "rt_scan_int", llvm::FunctionType::get(
                    LLVMIntTy, {}, false));

    ScanReal = Mod.getOrInsertFunction(
            "rt_scan_real", llvm::FunctionType::get(
                    LLVMRealTy, {}, false));

    ScanChar = Mod.getOrInsertFunction(
            "rt_scan_char", llvm::FunctionType::get(
                    LLVMCharTy, {}, false));

    ScanBool = Mod.getOrInsertFunction(
            "rt_scan_bool", llvm::FunctionType::get(
                    LLVMBoolTy, {}, false));
    Malloc = Mod.getOrInsertFunction(
            "malloc", llvm::FunctionType::get(
                    LLVMPtrTy, {LLVMIntTy}, false));
    VectorConcat = Mod.getOrInsertFunction(
            "rt_vector_concat", llvm::FunctionType::get(
                    LLVMVectorTy, {LLVMVectorTy->getPointerTo(), LLVMVectorTy->getPointerTo()}, false));
    VectorDotProductInt = Mod.getOrInsertFunction(
            "rt_vector_dotproduct_int", llvm::FunctionType::get(
                    LLVMIntTy, {LLVMVectorTy->getPointerTo(), LLVMVectorTy->getPointerTo()}, false));
    VectorDotProductReal = Mod.getOrInsertFunction(
            "rt_vector_dotproduct_real", llvm::FunctionType::get(
                    LLVMRealTy, {LLVMVectorTy->getPointerTo(), LLVMVectorTy->getPointerTo()}, false));
    VectorBy = Mod.getOrInsertFunction(
            "rt_vector_by", llvm::FunctionType::get(
                    LLVMVectorTy, {LLVMVectorTy->getPointerTo(), LLVMIntTy}, false));
    VectorNot = Mod.getOrInsertFunction(
            "rt_vector_not", llvm::FunctionType::get(
                    LLVMVectorTy, {LLVMVectorTy->getPointerTo()}, false));
    VectorSub = Mod.getOrInsertFunction(
            "rt_vector_sub", llvm::FunctionType::get(
                    LLVMVectorTy, {LLVMVectorTy->getPointerTo()}, false));
    PrintVector = Mod.getOrInsertFunction(
            "rt_print_vector", llvm::FunctionType::get(
                    LLVMVoidTy, {LLVMVectorTy->getPointerTo()}, false));
    VectorEq = Mod.getOrInsertFunction(
            "rt_vector_eq", llvm::FunctionType::get(
                    LLVMCharTy, {LLVMVectorTy->getPointerTo(), LLVMVectorTy->getPointerTo(), LLVMIntTy}, false));
    VectorLogical = Mod.getOrInsertFunction(
            "rt_vector_logical", llvm::FunctionType::get(
                    LLVMVectorTy, {LLVMVectorTy->getPointerTo(), LLVMVectorTy->getPointerTo(), LLVMIntTy}, false));
    VectorArith = Mod.getOrInsertFunction(
            "rt_vector_arith", llvm::FunctionType::get(
                    LLVMVectorTy, {LLVMVectorTy->getPointerTo(), LLVMVectorTy->getPointerTo(), LLVMIntTy}, false));
    VectorComp = Mod.getOrInsertFunction(
            "rt_vector_comp", llvm::FunctionType::get(
                    LLVMVectorTy, {LLVMVectorTy->getPointerTo(), LLVMVectorTy->getPointerTo(), LLVMIntTy}, false));

    visit(Root);

    // Dump the module to the output file.
    std::ofstream Out(OutputFile);
    llvm::raw_os_ostream OS(Out);
    OS << Mod;
}

llvm::Type *CodeGenPass::getLLVMType(Type *Ty) {
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
        case Type::TypeKind::T_Interval:
            return ConstConv(LLVMIntervalTy, Ty->isConst());
        case Type::TypeKind::T_Tuple:
            return ConstConv(getLLVMTupleType(
                    cast<TupleTy>(Ty)), Ty->isConst());
        case Type::TypeKind::T_Function:
            return getLLVMFunctionType(cast<FunctionTy>(Ty));
        case Type::TypeKind::T_Procedure:
            return getLLVMProcedureType(cast<ProcedureTy>(Ty));
        case Type::TypeKind::T_Vector:
            return ConstConv(LLVMVectorTy, Ty->isConst());
        default:
            throw std::runtime_error("Unknown type in backend");
    }
}

llvm::Type *CodeGenPass::getLLVMTupleType(TupleTy *Tuple) {
    vector<llvm::Type*> TupleTypes;
    for (auto SubTy : Tuple->getMemberTypes())
        TupleTypes.push_back(getLLVMType(PM->TypeReg.getConstTypeOf(SubTy)));
    return llvm::StructType::get(GlobalCtx, TupleTypes);
}

llvm::Value *CodeGenPass::createAlloca(Type *Ty) {
    llvm::IRBuilder<> Builder(GlobalCtx);
    llvm::BasicBlock *BB = &CurrentFunction->front();
    Builder.SetInsertPoint(BB);
    return Builder.CreateAlloca(getLLVMType(Ty));
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


    // FIXME hotfix for bool assignment into vector
    auto *ValTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Assign->getExpr());
    if (isa<IndexReference>(Assign->getAssignedTo()) &&
        ValTy->isSameTypeAs(PM->TypeReg.getBooleanTy())) {
        Val = IR.CreateZExt(Val, LLVMCharTy);
    }
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

llvm::Value *CodeGenPass::declareGlobal(const string &Name, Type *Ty) {
    llvm::Type *LLTy = getLLVMType(PM->TypeReg.getConstTypeOf(Ty));
    Mod.getOrInsertGlobal(Name, LLTy);
    llvm::GlobalVariable *GV = Mod.getNamedGlobal(Name);
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
    auto LTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getLeftExpr());
//    auto RTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getRightExpr());
//    assert(RTy->isSameTypeAs(LTy) && "Operation between different types should"
//                                     " not have reached the code gen");

    llvm::CmpInst::Predicate Pred;

    if (isa<RealTy>(LTy)) {
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
    } else if (isa<VectorTy>(LTy)) {
        // TODO temporary alloc
        auto LeftVec = IR.CreateAlloca(LLVMVectorTy);
        auto RightVec = IR.CreateAlloca(LLVMVectorTy);
        IR.CreateStore(LeftOperand, LeftVec);
        IR.CreateStore(RightOperand, RightVec);

        return IR.CreateCall(VectorComp, {LeftVec, RightVec, llvm::ConstantInt::get(LLVMIntTy, Op->getOpKind())});

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

//    auto LTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getLeftExpr());
//    auto RTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getRightExpr());
//    assert(RTy->isSameTypeAs(LTy) && "Operation between different types should "
//                                     "not have reached the code gen");

    auto RoundingMDS = llvm::MDString::get(GlobalCtx, "round.dynamic");
    auto ExceptionMDS = llvm::MDString::get(GlobalCtx, "fpexcept.strict");
    auto RoundingMD = llvm::MetadataAsValue::get(GlobalCtx, RoundingMDS);
    auto ExceptionMD = llvm::MetadataAsValue::get(GlobalCtx, ExceptionMDS);

    const Type *ResultType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op);
    if (isa<IntegerTy>(ResultType)) {
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
                        {LLVMRealTy, LLVMIntTy, llvm::Type::getMetadataTy(
                                GlobalCtx), llvm::Type::getMetadataTy(
                                        GlobalCtx)},
                        {LeftOperand, RightOperand, RoundingMD, ExceptionMD}
                );
                return IR.CreateFPToSI(RetVal, LLVMIntTy);

        }
    } else if (isa<RealTy>(ResultType)) {
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
                        {LLVMRealTy, LLVMRealTy, llvm::Type::getMetadataTy(
                                GlobalCtx),
                         llvm::Type::getMetadataTy(GlobalCtx)},
                        {LeftOperand, RightOperand, RoundingMD, ExceptionMD}
                );

        }
        return IR.CreateConstrainedFPBinOp(
                IntrinsicID, LeftOperand, RightOperand, nullptr, "", nullptr,
                llvm::fp::rmDynamic, llvm::fp::ebStrict);
    } else if (isa<IntervalTy>(ResultType)) {
        llvm::Value *Left1, *Left2, *Right1, *Right2;
        llvm::Value *Mul1, *Mul2, *Mul3, *Mul4;
        llvm::Value *MulArray;
        llvm::Value *Result1, *Result2;
        Left1 = IR.CreateExtractValue(LeftOperand, {0});
        Left2 = IR.CreateExtractValue(LeftOperand, {1});
        Right1 = IR.CreateExtractValue(RightOperand, {0});
        Right2 = IR.CreateExtractValue(RightOperand, {1});
        switch (Op->getOpKind()) {
            case ArithmeticOp::ADD:
                Result1 = IR.CreateAdd(Left1, Right1);
                Result2 = IR.CreateAdd(Left2, Right2);
                break;
            case ArithmeticOp::SUB:
                Result1 = IR.CreateSub(Left1, Right2);
                Result2 = IR.CreateSub(Left2, Right1);
                break;
            case ArithmeticOp::MUL:
                Mul1 = IR.CreateMul(Left1, Right1);
                Mul2 = IR.CreateMul(Left1, Right2);
                Mul3 = IR.CreateMul(Left2, Right1);
                Mul4 = IR.CreateMul(Left2, Right2);
                MulArray = llvm::ConstantVector::get(
{IR.getInt32(0), IR.getInt32(0), IR.getInt32(0),
                         IR.getInt32(0)});
                MulArray = IR.CreateInsertElement(MulArray, Mul1, IR.getInt32(0));
                MulArray = IR.CreateInsertElement(MulArray, Mul2, IR.getInt32(1));
                MulArray = IR.CreateInsertElement(MulArray, Mul3, IR.getInt32(2));
                MulArray = IR.CreateInsertElement(MulArray, Mul4, IR.getInt32(3));
                Result1 = IR.CreateIntrinsic(
                        llvm::Intrinsic::experimental_vector_reduce_smin,
                        {llvm::VectorType::get(LLVMIntTy, 4)},
                        {MulArray});
                Result2 = IR.CreateIntrinsic(
                        llvm::Intrinsic::experimental_vector_reduce_smax,
                        {llvm::VectorType::get(LLVMIntTy, 4)},
                        {MulArray});
                break;
            default:
                throw std::runtime_error("Not implemented");
        }
        llvm::Value *Result = llvm::ConstantStruct::get(LLVMIntervalTy, {IR.getInt32(0), IR.getInt32(0)});
        Result = IR.CreateInsertValue(Result, Result1, {0});
        Result = IR.CreateInsertValue(Result, Result2, {1});
        return Result;
    } else if (isa<VectorTy>(ResultType)) {
        auto LeftVec = IR.CreateAlloca(LLVMVectorTy);
        auto RightVec = IR.CreateAlloca(LLVMVectorTy);
        IR.CreateStore(LeftOperand, LeftVec);
        IR.CreateStore(RightOperand, RightVec);
        return IR.CreateCall(VectorArith, {LeftVec, RightVec, IR.getInt32(Op->getOpKind())});
    }
    throw std::runtime_error("Unknown type in arithmetic expression");
}

llvm::Value *CodeGenPass::visitLogicalOp(LogicalOp *Op) {
    Value *LeftOperand = visit(Op->getLeftExpr());
    Value *RightOperand = visit(Op->getRightExpr());

    auto LeftType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getLeftExpr());
    auto RightType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getRightExpr());
    if (!RightType->isSameTypeAs(LeftType))
        throw std::runtime_error("Operation between different types should not"
                                     " have reached the code gen");

    if (isa<RealTy>(LeftType)) {
        switch (Op->getOpKind()) {
            case LogicalOp::EQ:
                return IR.CreateFCmpOEQ(LeftOperand, RightOperand);
            case LogicalOp::NEQ:
                return IR.CreateFCmpONE(LeftOperand, RightOperand);
            default:
                throw std::runtime_error("Invalid logical operation for real type");
        }
    } else if (isa<IntervalTy>(LeftType)) {
        llvm::Value *Left1, *Left2, *Right1, *Right2;
        llvm::Value *Result1, *Result2;
        Left1 = IR.CreateExtractValue(LeftOperand, {0});
        Left2 = IR.CreateExtractValue(LeftOperand, {1});
        Right1 = IR.CreateExtractValue(RightOperand, {0});
        Right2 = IR.CreateExtractValue(RightOperand, {1});
        switch (Op->getOpKind()) {
            case LogicalOp::EQ:
                Result1 = IR.CreateICmpEQ(Left1, Right1);
                Result2 = IR.CreateICmpEQ(Left2, Right2);
                return IR.CreateAnd(Result1, Result2);
            case LogicalOp::NEQ:
                Result1 = IR.CreateICmpNE(Left1, Right1);
                Result2 = IR.CreateICmpNE(Left2, Right2);
                return IR.CreateOr(Result1, Result2);
            default:
                throw std::runtime_error("Invalid logical operation for interval type");
        }
    } else if (isa<VectorTy>(LeftType)) {
        // TODO temporary story for vector type
        auto LeftVec = IR.CreateAlloca(LLVMVectorTy);
        auto RightVec = IR.CreateAlloca(LLVMVectorTy);
        IR.CreateStore(LeftOperand, LeftVec);
        IR.CreateStore(RightOperand, RightVec);
        llvm::Value *Result;

        switch (Op->getOpKind()) {
            case LogicalOp::EQ:
            case LogicalOp::NEQ:
                Result = IR.CreateCall(VectorEq, {LeftVec, RightVec, llvm::ConstantInt::get(LLVMIntTy, Op->getOpKind())});
                return IR.CreateICmpNE(Result, llvm::ConstantInt::get(LLVMCharTy, 0));
            case LogicalOp::AND:
            case LogicalOp::OR:
            case LogicalOp::XOR:
                return IR.CreateCall(VectorLogical, {LeftVec, RightVec, llvm::ConstantInt::get(LLVMIntTy, Op->getOpKind())});
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

    const Type *ResultType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getExpr());

    if (isa<IntervalTy>(ResultType)) {
        llvm::Value *Result;
        switch (Op->getOpKind()) {
            case UnaryOp::SUB:
                // TODO ask Deric about semantics
                llvm::Value *Left, *Right;
                Left = IR.CreateExtractValue(Operand, {0});
                Right = IR.CreateExtractValue(Operand, {1});
                Result = llvm::ConstantStruct::get(LLVMIntervalTy, {IR.getInt32(0), IR.getInt32(0)});
                Result = IR.CreateInsertValue(Result, Right, {0});
                Result = IR.CreateInsertValue(Result, Left, {1});
                return Result;
            case UnaryOp::ADD:
                return Operand;
            default:
                throw std::runtime_error("Invalid unary operation for interval type");
        }
    } else if (isa<VectorTy>(ResultType)) {
        llvm::Value *Result;

        // TODO fix temporary store
        llvm::Value *Temp = IR.CreateAlloca(Operand->getType());
        IR.CreateStore(Operand, Temp);

        switch (Op->getOpKind()) {
            case UnaryOp::NOT:
                return IR.CreateCall(VectorNot, {Temp});
            case UnaryOp::ADD:
                return Operand;
            case UnaryOp::SUB:
                return IR.CreateCall(VectorSub, {Temp});
            default:
                assert(false && "Invalid unary operation for vector type");
        }
    }

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
    llvm::Value *Vec = visit(Idx->getBaseExpr());
    llvm::Value *Index = visit(Idx->getIndexExpr());

    // get types of the base expression and the index expression
    auto BaseType = PM->getAnnotation<ExprTypeAnnotatorPass>(Idx->getBaseExpr());
    auto IndexType = PM->getAnnotation<ExprTypeAnnotatorPass>(Idx->getIndexExpr());
    if (!IndexType->isSameTypeAs(PM->TypeReg.getIntegerTy()))
        throw std::runtime_error("Index must be an integer");

    if (BaseType->getKind() != Type::TypeKind::T_Vector)
        throw std::runtime_error("Base must be a vector");

    // TODO Check that the index is within the bounds of the array

    // Get malloc pointer from array struct
    llvm::Value *MallocPtr = CreateVectorMallocPtrAccess(Vec, dyn_cast<VectorTy>(BaseType));

    // Get the element pointer
    llvm::Value *ElementPtr = IR.CreateInBoundsGEP(MallocPtr, Index);

    // Get the element
    llvm::Value *Element = IR.CreateLoad(ElementPtr);

    // Update if the element is a bool
    // TODO move to casting logic
    if (dyn_cast<VectorTy>(BaseType)->getInnerTy()->isSameTypeAs(PM->TypeReg.getBooleanTy())) {
        Element = IR.CreateICmpNE(Element, llvm::ConstantInt::get(LLVMCharTy, 0));
    }

    return Element;
}

llvm::Value *CodeGenPass::visitInfiniteLoop(InfiniteLoop *Loop) {
    llvm::BasicBlock *LoopBody = llvm::BasicBlock::Create(
            GlobalCtx, "loop_body", CurrentFunction);
    llvm::BasicBlock *LoopEnd = llvm::BasicBlock::Create(
            GlobalCtx, "loop_end", CurrentFunction);

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
    llvm::BasicBlock *Header = llvm::BasicBlock::Create(
            GlobalCtx, "loop_header", CurrentFunction);
    llvm::BasicBlock *LoopBody = llvm::BasicBlock::Create(
            GlobalCtx, "loop_body");
    llvm::BasicBlock *LoopEnd = llvm::BasicBlock::Create(
            GlobalCtx, "loop_end", CurrentFunction);

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
    auto Val =  IR.getInt32(IntLit->getVal());
    return Val;
}

llvm::Value *CodeGenPass::visitNullLiteral(NullLiteral*) {
    throw runtime_error("Should not have reached the codegen");
}

llvm::Value *CodeGenPass::visitIdentityLiteral(IdentityLiteral*) {
    throw runtime_error("Should not have reached the codegen");
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
    auto &Cache = PM->getResource<SubExpressionCacheSet>();
    auto Val = Cache.getCached(MemberAcc->getExpr());
    auto Expr = Val ? Val : visit(MemberAcc->getExpr());
    if (Cache.shouldCache(MemberAcc->getExpr()))
        Cache.setValueFor(MemberAcc->getExpr(), Expr);
    auto MemberIdx = dyn_cast<IntLiteral>(MemberAcc->getMemberExpr())->getVal();
    return IR.CreateExtractValue(Expr, MemberIdx-1);
}

llvm::Value *CodeGenPass::visitConditional(Conditional *Cond) {

    llvm::BasicBlock *CondHeader = llvm::BasicBlock::Create(
            GlobalCtx, "cond_header", CurrentFunction);
    llvm::BasicBlock *CondTrue = llvm::BasicBlock::Create(
            GlobalCtx, "cond_true");
    llvm::BasicBlock *CondFalse = llvm::BasicBlock::Create(
            GlobalCtx, "cond_false");

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

    llvm::BasicBlock *CondHeader = llvm::BasicBlock::Create(
            GlobalCtx, "cond_header", CurrentFunction);
    llvm::BasicBlock *CondTrue = llvm::BasicBlock::Create(
            GlobalCtx, "cond_true", CurrentFunction);
    llvm::BasicBlock *CondFalse = llvm::BasicBlock::Create(
            GlobalCtx, "cond_false", CurrentFunction);
    llvm::BasicBlock *CondEnd = llvm::BasicBlock::Create(
            GlobalCtx, "cond_end", CurrentFunction);

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

llvm::Value *CodeGenPass::getCastValue(Value *Val, Type *SrcTy, Type *DestTy) {

    // RULES FOR CASTING BASED ON DestTy

    if (SrcTy->isSameTypeAs(DestTy))
        return Val;

    switch (DestTy->getKind()) {
        case Type::TypeKind::T_Bool:
            return IR.CreateICmpNE(Val, llvm::Constant::getNullValue(
                    getLLVMType(PM->TypeReg.getConstTypeOf(SrcTy))));
        case Type::TypeKind::T_Char:
            // TODO fix char
            return CastOperation<Type::T_Char>::doCast(IR, Val, SrcTy);
        case Type::TypeKind::T_Int:
            return CastOperation<Type::T_Int>::doCast(IR, Val, SrcTy);
        case Type::TypeKind::T_Real:
            return CastOperation<Type::T_Real>::doCast(IR, Val, SrcTy);
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
    llvm::BasicBlock *Entry = llvm::BasicBlock::Create(
            GlobalCtx, "func_entry", Func);
    llvm::BasicBlock *Body = llvm::BasicBlock::Create(
            GlobalCtx, "func_body", Func);
    IR.SetInsertPoint(Body);

    // Set function arguments and set them in the symbol map
    auto ParamList = Def->getParamList();
    for (size_t I = 0; I < ParamList->numOfChildren(); I++) {
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
    if (!Func)
        throw std::runtime_error("Function not found");

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
    llvm::BasicBlock *Entry = llvm::BasicBlock::Create(
            GlobalCtx, "proc_entry", Proc);
    llvm::BasicBlock *Body = llvm::BasicBlock::Create(
            GlobalCtx, "proc_body", Proc);

    IR.SetInsertPoint(Body);

    // Set function arguments and set them in the symbol map
    auto ParamList = Def->getParamList();
    for (auto I = 0; I < ParamList->numOfChildren(); I++) {
        auto Param = ParamList->getParamAt(I);
        SymbolMap[Param->getReferred()] = Proc->getArg(I);
    }
    CurrentFunction = Proc;

    if (ProcName == "main")
        assignGlobals();

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
    if (!Func)
        throw std::runtime_error("Function not found");

    // Get the arguments
    std::vector<llvm::Value *> Args;
    for (auto Child : *Call->getArgsList())
        Args.emplace_back(visit(Child));

    return IR.CreateCall(Func, Args);
}

llvm::Value *CodeGenPass::visitReturn(Return *Ret) {
    // No-op means that the procedure does not have a return type.
    if (isa<NoOp>(Ret->getReturnExpr()))
        IR.CreateRetVoid();
    else
        IR.CreateRet(visit(Ret->getReturnExpr()));

    llvm::BasicBlock *AfterRet = llvm::BasicBlock::Create(
            GlobalCtx, "after_ret", CurrentFunction);
    IR.SetInsertPoint(AfterRet);
    return nullptr;
}

llvm::Value *CodeGenPass::visitBreak(Break *Break) {
    llvm::BasicBlock *AfterBreak = llvm::BasicBlock::Create(
            GlobalCtx, "after_break", CurrentFunction);
    llvm::BasicBlock *LoopEnd = LoopEndBlocks.top();

    IR.CreateBr(LoopEnd);

    IR.SetInsertPoint(AfterBreak);
    return nullptr;
}

llvm::Value *CodeGenPass::visitContinue(Continue *Continue) {

    llvm::BasicBlock *AfterContinue = llvm::BasicBlock::Create(
            GlobalCtx, "after_continue", CurrentFunction);
    llvm::BasicBlock *LoopEnd = LoopBeginBlocks.top();

    IR.CreateBr(LoopEnd);
    IR.SetInsertPoint(AfterContinue);
    return nullptr;
}

llvm::Value *CodeGenPass::visitOutStream(OutStream *Stream) {
    Value *ValToOut = visit(Stream->getOutStreamExpr());
    auto ValType = PM->getAnnotation<ExprTypeAnnotatorPass>(Stream->getOutStreamExpr());
    assert(ValType->isOutputTy() && "Invalid output stream type");
    if (ValType->getKind() == Type::T_Vector) {
        // TODO temporary store
        auto Vec = IR.CreateAlloca(LLVMVectorTy);
        IR.CreateStore(ValToOut, Vec);
        return IR.CreateCall(PrintVector, {Vec});
    }

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
            throw runtime_error("Invalid type for out-stream");
    }
}

llvm::Value *CodeGenPass::visitInStream(InStream *InStream) {
    const Type *IdentTy = PM->getAnnotation<ExprTypeAnnotatorPass>(InStream->getTarget());
    if(!IdentTy->isInputTy())
        throw std::runtime_error("Invalid input stream type");
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
            throw std::runtime_error("Invalid type for in-stream");
    }
    IR.CreateStore(ReadVal, StoreLoc);
    return nullptr;

}

llvm::Type *CodeGenPass::getLLVMFunctionType(FunctionTy *FuncTy) {
    vector<llvm::Type*> ParamTypes;
    for (auto Ty: FuncTy->getParamTypes())
        ParamTypes.emplace_back(getLLVMType(Ty));
    return llvm::cast<llvm::Type>(
            llvm::FunctionType::get(getLLVMType(FuncTy->getRetType()), ParamTypes, false));
}

llvm::Type *CodeGenPass::getLLVMProcedureType(ProcedureTy *ProcTy) {
    vector<llvm::Type*> ParamTypes;
    for (auto Ty: ProcTy->getParamTypes())
        ParamTypes.emplace_back(getLLVMType(Ty));
    return llvm::cast<llvm::Type>(
            llvm::FunctionType::get(getLLVMType(ProcTy->getRetTy()), ParamTypes, false));

}

llvm::Value *CodeGenPass::visitIdentReference(IdentReference *Ref) {
    return SymbolMap[Ref->getIdentifier()->getReferred()];
}

llvm::Value *CodeGenPass::visitIndexReference(IndexReference *Ref) {
    Value *Vec = visit(Ref->getBaseExpr());
    Value *Idx = visit(Ref->getIndexExpr());

    // get types of the base expression and the index expression
    auto BaseType = PM->getAnnotation<ExprTypeAnnotatorPass>(Ref->getBaseExpr());
    auto IndexType = PM->getAnnotation<ExprTypeAnnotatorPass>(Ref->getIndexExpr());

    if (!isa<IntegerTy>(IndexType))
        throw std::runtime_error("Index must be an integer");

    if (!isa<VectorTy>(BaseType))
        throw std::runtime_error("Base must be a vector");

    // TODO Check that the index is within the bounds of the array

    // Get malloc pointer from array struct
    llvm::Value *MallocPtr = CreateVectorMallocPtrAccess(Vec, dyn_cast<VectorTy>(BaseType));

    // Get the element pointer
    return IR.CreateInBoundsGEP(MallocPtr, Idx);
}

llvm::Value *CodeGenPass::visitMemberReference(MemberReference *Ref) {
    auto MemIdx = dyn_cast<IntLiteral>(Ref->getMemberExpr());
    if (!MemIdx)
        throw std::runtime_error("Only int literals should reach here");
    auto StructLoc = SymbolMap[Ref->getIdentifier()->getReferred()];
    return IR.CreateGEP(StructLoc, {
        IR.getInt32(0), IR.getInt32(MemIdx->getVal() - 1)});
}

llvm::Function *CodeGenPass::getOrInsertFunction(Type *Ty,
                                                 const string &Name) {

    if (auto Func = Mod.getFunction(Name))
        return Func;

    auto FuncTy = dyn_cast<FunctionTy>(Ty);
    auto ProcTy = dyn_cast<ProcedureTy>(Ty);
    if (!(ProcTy || FuncTy))
        throw std::runtime_error("Tried to insert something that is not a function");

    auto ParamTys = FuncTy ? FuncTy->getParamTypes()
            : ProcTy->getParamTypes();

    auto RetTy = FuncTy ? getLLVMType(FuncTy->getRetType())
            : getLLVMType(ProcTy->getRetTy());

    vector<llvm::Type*> LLVMParamTys;
    auto BuildLLVMTypes = [&](Type *T) {
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
    size_t ChildCount = Blk->numOfChildren();
    for (size_t I = 0; I < ChildCount; I++) {
        auto Child = Blk->getChildAt(I);
        visit(Child);
    }
    return nullptr;
    // TODO free unnecessary vectors
}

llvm::Value *CodeGenPass::visitVectorLiteral(VectorLiteral *VecLit) {
    auto VecTy = dyn_cast<VectorTy>(PM->getAnnotation<ExprTypeAnnotatorPass>(VecLit));
    assert(VecTy && "Invalid vector type");

    auto VecSize = VecTy->getSize();
    assert(VecSize >= 0 && "All vector literals should have a size");

    llvm::Value *Result = CreateVectorStruct(VecTy->getInnerTy()->getKind(), VecSize, true);
    auto MallocPtr = CreateVectorMallocPtrAccess(Result, VecTy);

    // store the elements in the vector
    for (int i = 0; i < VecSize; i++) {
        auto Elem = VecLit->getChildAt(i);
        auto ElemVal = visit(Elem);
        auto ElemPtr = IR.CreateInBoundsGEP(MallocPtr, {IR.getInt32(i)});
        if (VecTy->getInnerTy()->isSameTypeAs(PM->TypeReg.getBooleanTy()))
            ElemVal = IR.CreateZExt(ElemVal, IR.getInt8Ty());
        IR.CreateStore(ElemVal, ElemPtr);
    }

    return Result;

}

llvm::Value *CodeGenPass::CreateVectorStruct(enum Type::TypeKind TyKind, uint32_t size, bool malloc) {
    uint32_t InnerTyEnum;
    uint32_t InnerTySize;
    switch (TyKind) {
        case Type::TypeKind::T_Bool:
            InnerTyEnum = 0;
            InnerTySize = 1;
            break;
        case Type::TypeKind::T_Char:
            InnerTyEnum = 1;
            InnerTySize = 1;
            break;
        case Type::TypeKind::T_Int:
            InnerTyEnum = 2;
            InnerTySize = 4;
            break;
        case Type::TypeKind::T_Real:
            InnerTyEnum = 3;
            InnerTySize = 4;
            break;
        default:
            throw std::runtime_error("Invalid vector inner type");
    }

    llvm::Value *Result = llvm::ConstantStruct::get(
            LLVMVectorTy, {
                    IR.getInt32(size),
                    IR.getInt32(0),
                    IR.getInt32(InnerTyEnum),
                    llvm::ConstantPointerNull::get(LLVMPtrTy)
            });

    if (malloc) {
        // malloc space for the vector
        auto MallocCall = IR.CreateCall(Malloc, {IR.getInt32(size * InnerTySize)});

        // store the malloced pointer in the vector
        Result = IR.CreateInsertValue(Result, MallocCall, {3});
    }

    return Result;
}

llvm::Value *CodeGenPass::CreateVectorPointerBitCast(llvm::Value *VecPtr, enum Type::TypeKind TyKind) {
    // By default, all pointers to the data/malloc area of a vector are of type i8*. This function
    // casts the pointer to the appropriate type pointer.
    switch (TyKind) {
        case Type::TypeKind::T_Bool:
        case Type::TypeKind::T_Char:
            return VecPtr;
        case Type::TypeKind::T_Int:
            return IR.CreateBitCast(VecPtr, llvm::Type::getInt32PtrTy(GlobalCtx));
        case Type::TypeKind::T_Real:
            return IR.CreateBitCast(VecPtr, llvm::Type::getFloatPtrTy(GlobalCtx));
        default:
            assert(false && "Invalid vector inner type");
    }
}

llvm::Value *CodeGenPass::CreateVectorMallocPtrAccess(llvm::Value *VecPtr, VectorTy *VecTy) {
    auto MallocPtr = IR.CreateExtractValue(VecPtr, {3});
    MallocPtr = CreateVectorPointerBitCast(MallocPtr, VecTy->getInnerTy()->getKind());
    return MallocPtr;
}

llvm::Value *CodeGenPass::visitInterval(Interval *Interval) {
    llvm::Value *Lower = visit(Interval->getLowerExpr());
    llvm::Value *Upper = visit(Interval->getUpperExpr());

    // TODO bound check

    llvm::Value *Result = llvm::ConstantStruct::get(LLVMIntervalTy, {IR.getInt32(0), IR.getInt32(0)});
    Result = IR.CreateInsertValue(Result, Lower, {0});
    Result = IR.CreateInsertValue(Result, Upper, {1});
    return Result;
}

llvm::Value *CodeGenPass::visitConcat(Concat *Con) {
    llvm::Value *Left = visit(Con->getLHS());
    llvm::Value *Right = visit(Con->getRHS());

    // TODO temporary alloca
    llvm::Value *LeftPtr = IR.CreateAlloca(LLVMVectorTy);
    llvm::Value *RightPtr = IR.CreateAlloca(LLVMVectorTy);
    IR.CreateStore(Left, LeftPtr);
    IR.CreateStore(Right, RightPtr);

    llvm::Value *Result = IR.CreateCall(VectorConcat, {LeftPtr, RightPtr});
    return Result;
}

llvm::Value *CodeGenPass::visitDotProduct(DotProduct *DP) {
    llvm::Value *Left = visit(DP->getLHS());
    llvm::Value *Right = visit(DP->getRHS());

    // TODO temporary alloca
    llvm::Value *LeftPtr = IR.CreateAlloca(LLVMVectorTy);
    llvm::Value *RightPtr = IR.CreateAlloca(LLVMVectorTy);
    IR.CreateStore(Left, LeftPtr);
    IR.CreateStore(Right, RightPtr);

    // Determine type of the result
    auto LeftTy = dyn_cast<VectorTy>(PM->getAnnotation<ExprTypeAnnotatorPass>(DP->getLHS()));
    if (LeftTy->getInnerTy()->isSameTypeAs(PM->TypeReg.getRealTy()))
        return IR.CreateCall(VectorDotProductReal, {LeftPtr, RightPtr});
    else
        return IR.CreateCall(VectorDotProductInt, {LeftPtr, RightPtr});
}

llvm::Value *CodeGenPass::visitByOp(ByOp *By) {
    llvm::Value *Left = visit(By->getBaseExpr());
    llvm::Value *Right = visit(By->getByExpr());

    // TODO temporary alloca
    llvm::Value *LeftPtr = IR.CreateAlloca(LLVMVectorTy);
    IR.CreateStore(Left, LeftPtr);

    // Determine type of the result
    return IR.CreateCall(VectorBy, {LeftPtr, Right});

}
