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
                    LLVMCharTy, {}, false));
    Malloc = Mod.getOrInsertFunction(
            "malloc", llvm::FunctionType::get(
                    LLVMPtrTy, {LLVMIntTy}, false));
    VectorConcat = Mod.getOrInsertFunction(
            "rt_vector_concat", llvm::FunctionType::get(
                    LLVMVectorPtrTy, {LLVMVectorPtrTy, LLVMVectorPtrTy}, false));
    VectorDotProductInt = Mod.getOrInsertFunction(
            "rt_vector_dotproduct_int", llvm::FunctionType::get(
                    LLVMIntTy, {LLVMVectorPtrTy, LLVMVectorPtrTy}, false));
    VectorDotProductReal = Mod.getOrInsertFunction(
            "rt_vector_dotproduct_real", llvm::FunctionType::get(
                    LLVMRealTy, {LLVMVectorPtrTy, LLVMVectorPtrTy}, false));
    VectorAccessInt = Mod.getOrInsertFunction(
            "rt_vector_access_int64_t", llvm::FunctionType::get(
                    LLVMIntTy, {LLVMVectorPtrTy, LLVMIntTy, LLVMIntTy}, false));
    VectorAccessFloat = Mod.getOrInsertFunction(
            "rt_vector_access_float", llvm::FunctionType::get(
                    LLVMRealTy, {LLVMVectorPtrTy, LLVMIntTy, LLVMIntTy}, false));
    VectorAccessChar = Mod.getOrInsertFunction(
            "rt_vector_access_char", llvm::FunctionType::get(
                    LLVMCharTy, {LLVMVectorPtrTy, LLVMIntTy, LLVMIntTy}, false));
    VectorSetInt = Mod.getOrInsertFunction(
            "rt_vector_set_int64_t", llvm::FunctionType::get(
                    LLVMVoidTy, {LLVMVectorPtrTy, LLVMIntTy, LLVMIntTy, LLVMIntTy}, false));
    VectorSetFloat = Mod.getOrInsertFunction(
            "rt_vector_set_float", llvm::FunctionType::get(
                    LLVMVoidTy, {LLVMVectorPtrTy, LLVMIntTy, LLVMRealTy, LLVMIntTy}, false));
    VectorSetChar = Mod.getOrInsertFunction(
            "rt_vector_set_char", llvm::FunctionType::get(
                    LLVMVoidTy, {LLVMVectorPtrTy, LLVMIntTy, LLVMCharTy, LLVMIntTy}, false));
    VectorViewScalar = Mod.getOrInsertFunction(
            "rt_vector_view_scalar", llvm::FunctionType::get(
                    LLVMVectorPtrTy, {LLVMVectorPtrTy, LLVMIntTy}, false));
    VectorViewVector = Mod.getOrInsertFunction(
            "rt_vector_view_vector", llvm::FunctionType::get(
                    LLVMVectorPtrTy, {LLVMVectorPtrTy, LLVMVectorPtrTy}, false));
    VectorBy = Mod.getOrInsertFunction(
            "rt_vector_by", llvm::FunctionType::get(
                    LLVMVectorPtrTy, {LLVMVectorTy, LLVMIntTy}, false));
    VectorNot = Mod.getOrInsertFunction(
            "rt_vector_not", llvm::FunctionType::get(
                    LLVMVectorPtrTy, {LLVMVectorPtrTy}, false));
    VectorSub = Mod.getOrInsertFunction(
            "rt_vector_sub", llvm::FunctionType::get(
                    LLVMVectorPtrTy, {LLVMVectorPtrTy}, false));
    PrintVector = Mod.getOrInsertFunction(
            "rt_print_vector", llvm::FunctionType::get(
                    LLVMVectorPtrTy, {LLVMVectorPtrTy, LLVMVectorPtrTy, LLVMIntTy}, false));
    VectorEq = Mod.getOrInsertFunction(
            "rt_vector_eq", llvm::FunctionType::get(
                    LLVMCharTy, {LLVMVectorPtrTy, LLVMVectorPtrTy, LLVMIntTy}, false));
    VectorLogical = Mod.getOrInsertFunction(
            "rt_vector_logical", llvm::FunctionType::get(
                    LLVMVectorPtrTy, {LLVMVectorPtrTy, LLVMVectorPtrTy, LLVMIntTy}, false));
    VectorArith = Mod.getOrInsertFunction(
            "rt_vector_arith", llvm::FunctionType::get(
                    LLVMVectorPtrTy, {LLVMVectorPtrTy, LLVMVectorPtrTy, LLVMIntTy}, false));
    VectorComp = Mod.getOrInsertFunction(
            "rt_vector_comp", llvm::FunctionType::get(
                    LLVMVectorTy, {LLVMVectorTy->getPointerTo(), LLVMVectorTy->getPointerTo(), LLVMIntTy}, false));
    PrintString = Mod.getOrInsertFunction(
            "rt_print_string", llvm::FunctionType::get(
                    LLVMVectorPtrTy, {LLVMVectorPtrTy, LLVMVectorPtrTy, LLVMIntTy}, false));
    PrintMatrix = Mod.getOrInsertFunction(
            "rt_print_matrix", llvm::FunctionType::get(
                    LLVMVoidTy, {LLVMMatrixPtrTy}, false));
    MatrixNew = Mod.getOrInsertFunction(
            "rt_matrix_new", llvm::FunctionType::get(
                    LLVMMatrixPtrTy, {LLVMIntTy, LLVMIntTy, LLVMIntTy}, false));
    MatrixPopulateRow = Mod.getOrInsertFunction(
            "rt_matrix_populate_row", llvm::FunctionType::get(
                    LLVMVoidTy, {LLVMMatrixPtrTy, LLVMVectorPtrTy, LLVMIntTy}, false));
    MatrixAccessChar = Mod.getOrInsertFunction(
            "rt_matrix_access_char", llvm::FunctionType::get(
                    LLVMCharTy, {LLVMMatrixPtrTy, LLVMIntTy, LLVMIntTy, LLVMIntTy}, false));
    MatrixAccessInt = Mod.getOrInsertFunction(
            "rt_matrix_access_int64_t", llvm::FunctionType::get(
                    LLVMIntTy, {LLVMMatrixPtrTy, LLVMIntTy, LLVMIntTy, LLVMIntTy}, false));
    MatrixAccessFloat = Mod.getOrInsertFunction(
            "rt_matrix_access_float", llvm::FunctionType::get(
                    LLVMRealTy, {LLVMMatrixPtrTy, LLVMIntTy, LLVMIntTy, LLVMIntTy}, false));
    MatrixSetChar = Mod.getOrInsertFunction(
            "rt_matrix_set_char", llvm::FunctionType::get(
                    LLVMVoidTy, {LLVMMatrixPtrTy, LLVMIntTy, LLVMIntTy, LLVMCharTy, LLVMIntTy}, false));
    MatrixSetInt = Mod.getOrInsertFunction(
            "rt_matrix_set_int64_t", llvm::FunctionType::get(
                    LLVMVoidTy, {LLVMMatrixPtrTy, LLVMIntTy, LLVMIntTy, LLVMIntTy, LLVMIntTy}, false));
    MatrixSetFloat = Mod.getOrInsertFunction(
            "rt_matrix_set_float", llvm::FunctionType::get(
                    LLVMVoidTy, {LLVMMatrixPtrTy, LLVMIntTy, LLVMIntTy, LLVMRealTy, LLVMIntTy}, false));
    MatrixNot = Mod.getOrInsertFunction(
            "rt_matrix_not", llvm::FunctionType::get(
                    LLVMMatrixPtrTy, {LLVMMatrixPtrTy}, false));
    MatrixSub = Mod.getOrInsertFunction(
            "rt_matrix_sub", llvm::FunctionType::get(
                    LLVMMatrixPtrTy, {LLVMMatrixPtrTy}, false));
    MatrixArith = Mod.getOrInsertFunction(
            "rt_matrix_arith", llvm::FunctionType::get(
                    LLVMMatrixPtrTy, {LLVMMatrixPtrTy, LLVMMatrixPtrTy, LLVMIntTy}, false));
    MatrixComp = Mod.getOrInsertFunction(
            "rt_matrix_comp", llvm::FunctionType::get(
                    LLVMMatrixPtrTy, {LLVMMatrixPtrTy, LLVMMatrixPtrTy, LLVMIntTy}, false));
    MatrixLogical = Mod.getOrInsertFunction(
            "rt_matrix_logical", llvm::FunctionType::get(
                    LLVMMatrixPtrTy, {LLVMMatrixPtrTy, LLVMMatrixPtrTy, LLVMIntTy}, false));
    MatrixEq = Mod.getOrInsertFunction(
            "rt_matrix_eq", llvm::FunctionType::get(
                    LLVMCharTy, {LLVMMatrixPtrTy, LLVMMatrixPtrTy, LLVMIntTy}, false));
    MatrixViewScalar = Mod.getOrInsertFunction(
            "rt_matrix_view_scalar", llvm::FunctionType::get(
                    LLVMMatrixPtrTy, {LLVMMatrixPtrTy, LLVMIntTy, LLVMIntTy}, false));
    MatrixViewVector = Mod.getOrInsertFunction(
            "rt_matrix_view_vector", llvm::FunctionType::get(
                    LLVMMatrixPtrTy, {LLVMMatrixPtrTy, LLVMVectorPtrTy, LLVMIntTy, LLVMIntTy}, false));
    MatrixViewMatrix = Mod.getOrInsertFunction(
            "rt_matrix_view_matrix", llvm::FunctionType::get(
                    LLVMMatrixPtrTy, {LLVMMatrixPtrTy, LLVMVectorPtrTy, LLVMVectorPtrTy}, false));
    MatrixCreateDeepCopy = Mod.getOrInsertFunction(
            "rt_matrix_create_deep_copy", llvm::FunctionType::get(
                    LLVMMatrixPtrTy, {LLVMMatrixPtrTy}, false));
    VectorCreateDeepCopy = Mod.getOrInsertFunction(
            "rt_vector_create_deep_copy", llvm::FunctionType::get(
                    LLVMVectorPtrTy, {LLVMVectorPtrTy}, false));
    MatrixCopy = Mod.getOrInsertFunction(
            "rt_matrix_copy", llvm::FunctionType::get(
                    LLVMVoidTy, {LLVMMatrixPtrTy, LLVMMatrixPtrTy}, false));
    VectorCopy = Mod.getOrInsertFunction(
            "rt_vector_copy", llvm::FunctionType::get(
                    LLVMVoidTy, {LLVMVectorPtrTy, LLVMVectorPtrTy}, false));
    MatrixSetVector = Mod.getOrInsertFunction(
            "rt_matrix_set_vector", llvm::FunctionType::get(
                    LLVMVoidTy, {LLVMMatrixPtrTy, LLVMVectorPtrTy}, false));
    MatrixMul = Mod.getOrInsertFunction(
            "rt_matrix_mul", llvm::FunctionType::get(
                    LLVMMatrixPtrTy, {LLVMMatrixPtrTy, LLVMMatrixPtrTy}, false));

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
            return ConstConv(LLVMVectorPtrTy, Ty->isConst());
        case Type::TypeKind::T_Matrix:
            return ConstConv(LLVMMatrixPtrTy, Ty->isConst());
        default:
            throw std::runtime_error("Unknown type in backend");
    }
}

llvm::Type *CodeGenPass::getLLVMTupleType(const TupleTy *Tuple) {
    vector<llvm::Type*> TupleTypes;
    for (const Type *SubTy : Tuple->getMemberTypes())
        TupleTypes.push_back(getLLVMType(PM->TypeReg.getConstTypeOf(SubTy)));
    return llvm::StructType::get(GlobalCtx, TupleTypes);
}

llvm::Type *CodeGenPass::getLLVMVectorType(const VectorTy *Vector) {
    // Deprecated, uses array type
    return llvm::ArrayType::get(getLLVMType(
            PM->TypeReg.getConstTypeOf(Vector->getInnerTy())),
            Vector->getSize());
}

llvm::Value *CodeGenPass::createAlloca(const Type *Ty) {
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

    if (isa<IndexReference>(Assign->getAssignedTo())) {
        auto Expr = visit(Assign->getExpr());
        auto AssignedTo = visit(Assign->getAssignedTo());

        // These outer types are not representative of the main base type, but rather the type of what is being
        // assigned. We essentially visit the IndexReference on our own, and then assign the correct value.
        auto ExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Assign->getExpr());
        auto AssignedToTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Assign->getAssignedTo());
        assert(ExprTy->isSameTypeAs(AssignedToTy) && "Types are not the same");

        auto VarExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(dyn_cast<IndexReference>(Assign->getAssignedTo())->getBaseExpr());
        if (isa<VectorTy>(VarExprTy)) {
            // TODO fix assigning boolean to function with bad function call signature

            if (ExprTy->isScalarTy()) {
                llvm::Value *Res;
                switch (ExprTy->getKind()) {
                    case Type::TypeKind::T_Int:
                        return IR.CreateCall(VectorSetInt, {AssignedTo, IR.getInt64(0), Expr, IR.getInt64(0)});
                    case Type::TypeKind::T_Real:
                        return IR.CreateCall(VectorSetFloat, {AssignedTo, IR.getInt64(0), Expr, IR.getInt64(0)});
                    case Type::TypeKind::T_Char:
                        return IR.CreateCall(VectorSetChar, {AssignedTo, IR.getInt64(0), Expr, IR.getInt64(0)});
                    case Type::TypeKind::T_Bool:
                        return IR.CreateCall(VectorSetChar, {AssignedTo, IR.getInt64(0), IR.CreateZExt(Expr, LLVMCharTy), IR.getInt64(0)});
                    default:
                        assert(false && "Unknown type");
                }
            } else if (isa<VectorTy>(ExprTy)) {
                return IR.CreateCall(VectorCopy, {AssignedTo, Expr});
            }
        } else if (isa<MatrixTy>(VarExprTy)) {
            if (ExprTy->isScalarTy()) {
                llvm::Value *Res;
                switch (ExprTy->getKind()) {
                    case Type::TypeKind::T_Int:
                        return IR.CreateCall(MatrixSetInt, {AssignedTo, IR.getInt64(0), IR.getInt64(0), Expr, IR.getInt64(0)});
                    case Type::TypeKind::T_Real:
                        return IR.CreateCall(MatrixSetFloat, {AssignedTo, IR.getInt64(0), IR.getInt64(0), Expr, IR.getInt64(0)});
                    case Type::TypeKind::T_Char:
                        return IR.CreateCall(MatrixSetChar, {AssignedTo, IR.getInt64(0), IR.getInt64(0), Expr, IR.getInt64(0)});
                    case Type::TypeKind::T_Bool:
                        return IR.CreateCall(MatrixSetChar, {AssignedTo, IR.getInt64(0), IR.getInt64(0), IR.CreateZExt(Expr, LLVMCharTy), IR.getInt64(0)});
                    default:
                        assert(false && "Unknown type");
                }
            } else if (isa<MatrixTy>(ExprTy)) {
                return IR.CreateCall(MatrixCopy, {Expr, AssignedTo});
            } else if (isa<VectorTy>(ExprTy)) {
                return IR.CreateCall(MatrixSetVector, {AssignedTo, Expr});
            }

        }
        assert(false && "Should not reach here");
    }


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

llvm::Value *CodeGenPass::declareGlobal(const string &Name, const Type *Ty) {
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
        return IR.CreateCall(VectorComp, {LeftOperand, RightOperand, llvm::ConstantInt::get(LLVMIntTy, Op->getOpKind())});
    } else if (isa<MatrixTy>(LTy)) {
        return IR.CreateCall(MatrixComp, {LeftOperand, RightOperand, llvm::ConstantInt::get(LLVMIntTy, Op->getOpKind())});

    } else if (isa<IntegerTy>(LTy)) {
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
                        {IR.getInt64(0), IR.getInt64(0), IR.getInt64(0),
                         IR.getInt64(0)});
                MulArray = IR.CreateInsertElement(MulArray, Mul1, IR.getInt64(0));
                MulArray = IR.CreateInsertElement(MulArray, Mul2, IR.getInt64(1));
                MulArray = IR.CreateInsertElement(MulArray, Mul3, IR.getInt64(2));
                MulArray = IR.CreateInsertElement(MulArray, Mul4, IR.getInt64(3));
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
        llvm::Value *Result = llvm::ConstantStruct::get(LLVMIntervalTy, {IR.getInt64(0), IR.getInt64(0)});
        Result = IR.CreateInsertValue(Result, Result1, {0});
        Result = IR.CreateInsertValue(Result, Result2, {1});
        return Result;
    } else if (isa<VectorTy>(ResultType)) {
        return IR.CreateCall(VectorArith, {LeftOperand, RightOperand, IR.getInt64(Op->getOpKind())});
    } else if (isa<MatrixTy>(ResultType)) {
        return IR.CreateCall(MatrixArith, {LeftOperand, RightOperand, IR.getInt64(Op->getOpKind())});
    }
        assert(false && "Should not reach");
}

llvm::Value *CodeGenPass::visitLogicalOp(LogicalOp *Op) {
    Value *LeftOperand = visit(Op->getLeftExpr());
    Value *RightOperand = visit(Op->getRightExpr());

    const Type *LeftType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getLeftExpr());
    const Type *RightType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getRightExpr());
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
        llvm::Value *Result;

        switch (Op->getOpKind()) {
            case LogicalOp::EQ:
            case LogicalOp::NEQ:
                Result = IR.CreateCall(VectorEq, {LeftOperand, RightOperand, llvm::ConstantInt::get(LLVMIntTy, Op->getOpKind())});
                return IR.CreateICmpNE(Result, llvm::ConstantInt::get(LLVMCharTy, 0));
            case LogicalOp::AND:
            case LogicalOp::OR:
            case LogicalOp::XOR:
                return IR.CreateCall(VectorLogical, {LeftOperand, RightOperand, llvm::ConstantInt::get(LLVMIntTy, Op->getOpKind())});
        }

    } else if (isa<MatrixTy>(LeftType)) {
        llvm::Value *Result;

        switch (Op->getOpKind()) {
            case LogicalOp::EQ:
            case LogicalOp::NEQ:
                Result = IR.CreateCall(MatrixEq, {LeftOperand, RightOperand, llvm::ConstantInt::get(LLVMIntTy, Op->getOpKind())});
                return IR.CreateICmpNE(Result, llvm::ConstantInt::get(LLVMCharTy, 0));
            case LogicalOp::AND:
            case LogicalOp::OR:
            case LogicalOp::XOR:
                return IR.CreateCall(MatrixLogical, {LeftOperand, RightOperand, llvm::ConstantInt::get(LLVMIntTy, Op->getOpKind())});
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
                llvm::Value *Left, *Right;
                Left = IR.CreateExtractValue(Operand, {0});
                Right = IR.CreateExtractValue(Operand, {1});
                Result = llvm::ConstantStruct::get(LLVMIntervalTy, {IR.getInt64(0), IR.getInt64(0)});
                Result = IR.CreateInsertValue(Result, Right, {0});
                Result = IR.CreateInsertValue(Result, Left, {1});
                return Result;
            case UnaryOp::ADD:
                return Operand;
            default:
                throw std::runtime_error("Invalid unary operation for interval type");
        }
    } else if (isa<VectorTy>(ResultType)) {

        switch (Op->getOpKind()) {
            case UnaryOp::NOT:
                return IR.CreateCall(VectorNot, {Operand});
            case UnaryOp::ADD:
                return Operand;
            case UnaryOp::SUB:
                return IR.CreateCall(VectorSub, {Operand});
            default:
                assert(false && "Invalid unary operation for vector type");
        }
    } else if (isa<MatrixTy>(ResultType)) {

        switch (Op->getOpKind()) {
            case UnaryOp::NOT:
                return IR.CreateCall(MatrixNot, {Operand});
            case UnaryOp::ADD:
                return Operand;
            case UnaryOp::SUB:
                return IR.CreateCall(MatrixSub, {Operand});
            default:
                assert(false && "Invalid unary operation for matrix type");
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
    // TODO Check that the index is within the bounds of the array
    llvm::Value *Vec = visit(Idx->getBaseExpr());
    const Type *BaseType = PM->getAnnotation<ExprTypeAnnotatorPass>(Idx->getBaseExpr());
    llvm::Value *Res;

    if (isa<VectorTy>(BaseType)) {
        auto VecTy = dyn_cast<VectorTy>(BaseType);
        llvm::Value *Index = visit(Idx->getIndexExpr());
        const Type *IndexType = PM->getAnnotation<ExprTypeAnnotatorPass>(Idx->getIndexExpr());

        if (isa<IntegerTy>(IndexType)) {

            switch(VecTy->getInnerTy()->getKind()) {
                case Type::T_Int:
                    return IR.CreateCall(VectorAccessInt, {Vec, Index, llvm::ConstantInt::get(LLVMIntTy, 0)});
                case Type::T_Real:
                    return IR.CreateCall(VectorAccessFloat, {Vec, Index, llvm::ConstantInt::get(LLVMIntTy, 0)});
                    break;
                case Type::T_Bool:
                    Res = IR.CreateCall(VectorAccessChar, {Vec, Index, llvm::ConstantInt::get(LLVMIntTy, 0)});
                    return IR.CreateICmpNE(Res, llvm::ConstantInt::get(LLVMCharTy, 0));
                case Type::T_Char:
                    return IR.CreateCall(VectorAccessChar, {Vec, Index, llvm::ConstantInt::get(LLVMIntTy, 0)});
                default:
                    assert(false && "Invalid type for vector");
            }

        } else if (isa<VectorTy>(IndexType)) {

            Res = IR.CreateCall(VectorViewVector, {Vec, Index});

        }
        // This is just done to ensure there are no lingering references to other vectors
        return IR.CreateCall(VectorCreateDeepCopy, {Res});

    } else if (isa<MatrixTy>(BaseType)) {
        auto MatTy = dyn_cast<MatrixTy>(BaseType);
        llvm::Value *Row = visit(Idx->getIndexExpr());
        llvm::Value *Col = visit(Idx->getIndex2Expr());
        const Type *RowType = PM->getAnnotation<ExprTypeAnnotatorPass>(Idx->getIndexExpr());
        const Type *ColType = PM->getAnnotation<ExprTypeAnnotatorPass>(Idx->getIndex2Expr());

        if (isa<IntegerTy>(RowType) && isa<IntegerTy>(ColType)) {

            switch(MatTy->getInnerTy()->getKind()) {
                case Type::T_Int:
                    return IR.CreateCall(MatrixAccessInt, {Vec, Row, Col, llvm::ConstantInt::get(LLVMIntTy, 0)});
                case Type::T_Real:
                    return IR.CreateCall(MatrixAccessFloat, {Vec, Row, Col, llvm::ConstantInt::get(LLVMIntTy, 0)});
                case Type::T_Bool:
                    Res = IR.CreateCall(MatrixAccessChar, {Vec, Row, Col, llvm::ConstantInt::get(LLVMIntTy, 0)});
                    return IR.CreateICmpNE(Res, llvm::ConstantInt::get(LLVMCharTy, 0));
                case Type::T_Char:
                    return IR.CreateCall(MatrixAccessChar, {Vec, Row, Col, llvm::ConstantInt::get(LLVMIntTy, 0)});
                default:
                    assert(false && "Invalid type for matrix");
            }

        } else if (isa<VectorTy>(RowType) && isa<VectorTy>(ColType)) {
            Res = IR.CreateCall(MatrixViewMatrix, {Vec, Row, Col});
        } else if (isa<VectorTy>(RowType) && isa<IntegerTy>(ColType)) {
            Res = IR.CreateCall(MatrixViewVector, {Vec, Row, Col, IR.getInt64(0)});
        } else if (isa<IntegerTy>(RowType) && isa<VectorTy>(ColType)) {
            Res = IR.CreateCall(MatrixViewVector, {Vec, Col, Row, IR.getInt64(1)});
        }

        return IR.CreateCall(MatrixCreateDeepCopy, {Res});

    }
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
    return IR.getInt64(IntLit->getVal());
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
                TupLoc, {IR.getInt64(0), IR.getInt64(CurrIdx++)});
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
    return nullptr;
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
    const Type *ValType = PM->getAnnotation<ExprTypeAnnotatorPass>(Stream->getOutStreamExpr());
    assert(ValType->isOutputTy() && "Invalid output stream type");

    if (ValType->getKind() == Type::T_Vector) {
        return IR.CreateCall(PrintVector, {ValToOut});
    } else if (ValType->getKind() == Type::T_Matrix) {
        return IR.CreateCall(PrintMatrix, {ValToOut});
    }
    if (ValType->getKind() == Type::T_String) {
        // TODO temporary store
        auto Str = IR.CreateAlloca(LLVMVectorTy);
        IR.CreateStore(ValToOut, Str);
        return IR.CreateCall(PrintString, {Str});
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
            ReadVal = IR.CreateICmpNE(ReadVal, llvm::ConstantInt::get(LLVMCharTy, 0));
            break;
        case Type::TypeKind::T_Real:
            ReadVal = IR.CreateCall(ScanReal);
            break;
        default:
            throw std::runtime_error("Invalid type for in-stream");
    }
    if (isa<IndexReference>(InStream->getTarget())) {

        auto VarExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(
                dyn_cast<IndexReference>(InStream->getTarget())->getBaseExpr());
        if (isa<VectorTy>(VarExprTy)) {
            // TODO fix assigning boolean to function with bad function call signature

            switch (IdentTy->getKind()) {
                case Type::TypeKind::T_Int:
                    return IR.CreateCall(VectorSetInt, {StoreLoc, IR.getInt64(0), ReadVal, IR.getInt64(0)});
                case Type::TypeKind::T_Real:
                    return IR.CreateCall(VectorSetFloat, {StoreLoc, IR.getInt64(0), ReadVal, IR.getInt64(0)});
                case Type::TypeKind::T_Char:
                    return IR.CreateCall(VectorSetChar, {StoreLoc, IR.getInt64(0), ReadVal, IR.getInt64(0)});
                case Type::TypeKind::T_Bool:
                    return IR.CreateCall(VectorSetChar,
                                         {StoreLoc, IR.getInt64(0), IR.CreateZExt(ReadVal, LLVMCharTy), IR.getInt64(0)});
                default:
                    assert(false && "Unknown type");
            }

        } else if (isa<MatrixTy>(VarExprTy)) {
            switch (IdentTy->getKind()) {
                case Type::TypeKind::T_Int:
                    return IR.CreateCall(MatrixSetInt,
                                         {StoreLoc, IR.getInt64(0), IR.getInt64(0), ReadVal, IR.getInt64(0)});
                case Type::TypeKind::T_Real:
                    return IR.CreateCall(MatrixSetFloat,
                                         {StoreLoc, IR.getInt64(0), IR.getInt64(0), ReadVal, IR.getInt64(0)});
                case Type::TypeKind::T_Char:
                    return IR.CreateCall(MatrixSetChar,
                                         {StoreLoc, IR.getInt64(0), IR.getInt64(0), ReadVal, IR.getInt64(0)});
                case Type::TypeKind::T_Bool:
                    return IR.CreateCall(MatrixSetChar,
                                         {StoreLoc, IR.getInt64(0), IR.getInt64(0), IR.CreateZExt(ReadVal, LLVMCharTy),
                                          IR.getInt64(0)});
                default:
                    assert(false && "Unknown type");
            }
        }
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

llvm::Value *CodeGenPass::visitIndexReference(IndexReference *Ref) {

    // TODO Check that the index is within the bounds of the array

    Value *Vec = visit(Ref->getBaseExpr());
    auto VecTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Ref->getBaseExpr());

    if (isa<VectorTy>(VecTy)) {

        Value *Idx = visit(Ref->getIndexExpr());
        auto IdxTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Ref->getIndexExpr());

        if (isa<IntegerTy>(IdxTy)) {

            return IR.CreateCall(VectorViewScalar, {Vec, Idx});

        } else if (isa<VectorTy>(IdxTy)) {

            return IR.CreateCall(VectorViewVector, {Vec, Idx});

        } else {
            assert(false && "Invalid index type");
        }

        assert(isa<IntegerTy>(IdxTy) && "Invalid index type");

    } else if (isa<MatrixTy>(VecTy)) {

        Value *RowIdx = visit(Ref->getIndexExpr());
        auto RowIdxTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Ref->getIndexExpr());
        Value *ColIdx = visit(Ref->getIndex2Expr());
        auto ColIdxTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Ref->getIndex2Expr());

        if (isa<IntegerTy>(RowIdxTy) && isa<IntegerTy>(ColIdxTy)) {
            return IR.CreateCall(MatrixViewScalar, {Vec, RowIdx, ColIdx});
        } else if (isa<VectorTy>(RowIdxTy) && isa<VectorTy>(ColIdxTy)) {
            return IR.CreateCall(MatrixViewMatrix, {Vec, RowIdx, ColIdx});
        } else if (isa<VectorTy>(RowIdxTy) && isa<IntegerTy>(ColIdxTy)) {
            return IR.CreateCall(MatrixViewVector, {Vec, RowIdx, ColIdx, IR.getInt64(0)});
        } else if (isa<IntegerTy>(RowIdxTy) && isa<VectorTy>(ColIdxTy)) {
            return IR.CreateCall(MatrixViewVector, {Vec, ColIdx, RowIdx, IR.getInt64(1)});
        } else {
            assert(false && "Invalid index type");
        }

    }

    assert(false && "Invalid index reference");



}

llvm::Value *CodeGenPass::visitMemberReference(MemberReference *Ref) {
    auto MemIdx = dyn_cast<IntLiteral>(Ref->getMemberExpr());
    if (!MemIdx)
        throw std::runtime_error("Only int literals should reach here");
    auto StructLoc = SymbolMap[Ref->getIdentifier()->getReferred()];
    return IR.CreateGEP(StructLoc, {
        IR.getInt64(0), IR.getInt64(MemIdx->getVal() - 1)});
}

llvm::Function *CodeGenPass::getOrInsertFunction(const Type *Ty,
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
    size_t ChildCount = Blk->numOfChildren();
    for (size_t I = 0; I < ChildCount; I++) {
        auto Child = Blk->getChildAt(I);
        visit(Child);
    }
    return nullptr;
    // TODO free unnecessary vectors
    return nullptr;
}

llvm::Value *CodeGenPass::visitVectorLiteral(VectorLiteral *VecLit) {

    const auto *MatTy = dyn_cast<MatrixTy>(PM->getAnnotation<ExprTypeAnnotatorPass>(VecLit));
    if (MatTy) {
        auto MatStruct = IR.CreateCall(MatrixNew, {IR.getInt64(TypeKindMapToVectorTypeInRuntime(MatTy->getInnerTy()->getKind())),
                                                  IR.getInt64(MatTy->getNumOfRows()),
                                                  IR.getInt64(MatTy->getNumOfColumns())});

        for (int i = 0; i < MatTy->getNumOfRows(); i++) {
            auto Row = visit(VecLit->getChildAt(i));
            IR.CreateCall(MatrixPopulateRow, {MatStruct, Row, IR.getInt64(i)});
        }
        return MatStruct;
    }


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
        auto ElemPtr = IR.CreateInBoundsGEP(MallocPtr, {IR.getInt64(i)});
        if (VecTy->getInnerTy()->isSameTypeAs(PM->TypeReg.getBooleanTy()))
            ElemVal = IR.CreateZExt(ElemVal, IR.getInt8Ty());
        IR.CreateStore(ElemVal, ElemPtr);
    }

    // TODO temp alloc while this function is not in the runtime
    auto ResultLoc = IR.CreateAlloca(LLVMVectorTy);
    IR.CreateStore(Result, ResultLoc);

    return ResultLoc;



}

llvm::Value *CodeGenPass::visitStringLiteral(StringLiteral *VecLit) {
    auto VecTy = dyn_cast<VectorTy>(PM->getAnnotation<ExprTypeAnnotatorPass>(VecLit));
    assert(VecTy && "Invalid vector type");

    auto VecSize = VecTy->getSize();
    assert(VecSize >= 0 && "All vector literals should have a size");


    llvm::Value *Result = CreateVectorStruct(Type::TypeKind::T_Char, VecSize, true);
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


uint64_t CodeGenPass::TypeKindMapToVectorTypeInRuntime(Type::TypeKind Kind) {
    switch (Kind) {
        case Type::TypeKind::T_Bool:
            return 0;
        case Type::TypeKind::T_Char:
            return 1;
        case Type::TypeKind::T_Int:
            return 2;
        case Type::TypeKind::T_Real:
            return 3;
        default:
            assert(false && "Invalid type kind");
    }
}

llvm::Value *CodeGenPass::CreateVectorStruct(enum Type::TypeKind TyKind, uint64_t size, bool malloc) {
    uint64_t InnerTyEnum;
    uint64_t InnerTySize;
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
                    IR.getInt64(size),
                    llvm::ConstantPointerNull::get(LLVMPtrTy),
                    IR.getInt64(InnerTyEnum),
                    llvm::ConstantPointerNull::get(LLVMPtrTy)
            });

    if (malloc) {
        // malloc space for the vector
        auto MallocCall = IR.CreateCall(Malloc, {IR.getInt64(size * InnerTySize)});

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
            return IR.CreateBitCast(VecPtr, llvm::Type::getInt64PtrTy(GlobalCtx));
        case Type::TypeKind::T_Real:
            return IR.CreateBitCast(VecPtr, llvm::Type::getFloatPtrTy(GlobalCtx));
        default:
            assert(false && "Invalid vector inner type");
    }
}

llvm::Value *CodeGenPass::CreateVectorMallocPtrAccess(llvm::Value *VecPtr, const VectorTy *VecTy) {
    auto MallocPtr = IR.CreateExtractValue(VecPtr, {3});
    MallocPtr = CreateVectorPointerBitCast(MallocPtr, VecTy->getInnerTy()->getKind());
    return MallocPtr;
}

llvm::Value *CodeGenPass::visitInterval(Interval *Interval) {
    llvm::Value *Lower = visit(Interval->getLowerExpr());
    llvm::Value *Upper = visit(Interval->getUpperExpr());

    // TODO bound check

    llvm::Value *Result = llvm::ConstantStruct::get(LLVMIntervalTy, {IR.getInt64(0), IR.getInt64(0)});
    Result = IR.CreateInsertValue(Result, Lower, {0});
    Result = IR.CreateInsertValue(Result, Upper, {1});
    return Result;
}

llvm::Value *CodeGenPass::visitConcat(Concat *Con) {
    llvm::Value *Left = visit(Con->getLHS());
    llvm::Value *Right = visit(Con->getRHS());

    llvm::Value *Result = IR.CreateCall(VectorConcat, {Left, Right});
    return Result;
}

llvm::Value *CodeGenPass::visitDotProduct(DotProduct *DP) {
    llvm::Value *Left = visit(DP->getLHS());
    llvm::Value *Right = visit(DP->getRHS());

    auto LeftTy = PM->getAnnotation<ExprTypeAnnotatorPass>(DP->getLHS());
    auto RightTy = PM->getAnnotation<ExprTypeAnnotatorPass>(DP->getRHS());

    if (isa<VectorTy>(LeftTy)) {
        // Dot product

        // Determine type of the result
        auto LeftVTy = dyn_cast<VectorTy>(LeftTy);
        if (LeftVTy->getInnerTy()->isSameTypeAs(PM->TypeReg.getRealTy()))
            return IR.CreateCall(VectorDotProductReal, {Left, Right});
        else
            return IR.CreateCall(VectorDotProductInt, {Left, Right});

    } else if (isa<MatrixTy>(LeftTy)) {

        // Determine type of the result
        return IR.CreateCall(MatrixMul, {Left, Right});

    }

    assert(false && "Invalid dot product/matrix mul");

}

llvm::Value *CodeGenPass::visitByOp(ByOp *By) {
    llvm::Value *Left = visit(By->getBaseExpr());
    llvm::Value *Right = visit(By->getByExpr());

    return IR.CreateCall(VectorBy, {Left, Right});

}
