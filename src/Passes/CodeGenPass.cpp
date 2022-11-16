//
// Created by Chen on 2022-11-09.
//

#include <fstream>
#include "Passes/CodeGenPass.h"

using llvm::Value;

void CodeGenPass::runOnAST(ASTPassManager &Manager, ASTNodeT *Root) {
    assert(isa<Program>(Root) && "CodeGenPass should run on the entire program");
    PM = &Manager;

    llvm::FunctionType *ft = llvm::FunctionType::get(
            LLVMIntTy, false
    );

    GlobalFunction = llvm::Function::Create(ft, llvm::Function::ExternalLinkage, "main", Mod);

    llvm::BasicBlock *Entry = llvm::BasicBlock::Create(GlobalCtx, "entry", GlobalFunction);

    // Set the current function to the global function (for global variables)
    CurrentFunction = GlobalFunction;

    IR.SetInsertPoint(Entry);
    visit(Root);

    // TODO check for main function existing (in error handling)
    llvm::Value *RetVal = IR.CreateCall(MainFunction, {});
    IR.CreateRet(RetVal);

    // Dump the module to the output file.
    std::ofstream Out(OutputFile);
    llvm::raw_os_ostream OS(Out);
    OS << Mod;


}

llvm::Type *CodeGenPass::getLLVMType(const Type *Ty) {
    switch (Ty->getKind()) {
        case Type::TypeKind::T_Bool:
            return LLVMBoolTy;
        case Type::TypeKind::T_Int:
            return LLVMIntTy;
        case Type::TypeKind::T_Real:
            return LLVMRealTy;
        case Type::TypeKind::T_Char:
            return LLVMCharTy;
        case Type::TypeKind::T_Tuple:
            return getLLVMTupleType(dyn_cast<TupleTy>(Ty));
        default:
            return nullptr;
    }
}

llvm::Type *CodeGenPass::getLLVMTupleType(const TupleTy *Tuple) {
    vector<llvm::Type*> TupleTypes;
    for (const Type *SubTy : Tuple->getMemberTypes()) {
        TupleTypes.push_back(getLLVMType(SubTy));
    }
    return llvm::StructType::get(GlobalCtx, TupleTypes);
}

llvm::Value *CodeGenPass::createAlloca(const Type *Ty) {
    llvm::IRBuilder<> Builder(GlobalCtx);
    llvm::BasicBlock *BB = &CurrentFunction->front();
    Builder.SetInsertPoint(BB);
    return Builder.CreateAlloca(getLLVMType(Ty));
}

llvm::Value *CodeGenPass::visitIdentifier(Identifier *Ident) {
    return IR.CreateLoad(SymbolMap[Ident->getReferred()]);
}

llvm::Value *CodeGenPass::visitAssignment(Assignment *Assign) {
    Value *StoreVal = visit(Assign->getExpr());
    Value *StoreLoc = SymbolMap[Assign->getIdentifier()->getReferred()];
    // All assignments, including tuple assignments, are lowered to store assignments
    IR.CreateStore(StoreVal, StoreLoc);
    return nullptr;
}

llvm::Value *CodeGenPass::visitDeclaration(Declaration *Decl) {
    Value *InitValue = visit(Decl->getInitExpr());
    const Type *DeclType = PM->getAnnotation<ExprTypeAnnotatorPass>(Decl->getInitExpr());
    Value *DeclValue = createAlloca(DeclType);
    IR.CreateStore(InitValue, DeclValue);
    SymbolMap[Decl->getIdentifier()->getReferred()] = DeclValue;
    return nullptr;

}

llvm::Value *CodeGenPass::visitComparisonOp(ComparisonOp *Op) {
    Value *LeftOperand = visit(Op->getLeftExpr());
    Value *RightOperand = visit(Op->getRightExpr());

    // Just an assertion, not needed for code gen.
    const Type* LeftType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getLeftExpr());
    const Type* RightType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getRightExpr());
    assert( RightType == LeftType && "Operation between different types should not"
                                     " have reached the code gen");

    llvm::CmpInst::Predicate Pred;
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
    assert( RightType == LeftType && "Operation between different types should not"
                                     " have reached the code gen");

    switch (Op->getOpKind()) {
        case ArithmeticOp::ADD:
            return IR.CreateAdd(LeftOperand, RightOperand);
        case ArithmeticOp::SUB:
            return IR.CreateSub(LeftOperand, RightOperand);
        case ArithmeticOp::MUL:
            return IR.CreateMul(LeftOperand, RightOperand);
        case ArithmeticOp::DIV:
            return IR.CreateSDiv(LeftOperand, RightOperand);
        case ArithmeticOp::EXP:
            return IR.CreateCall(llvm::Intrinsic::getDeclaration(&Mod, llvm::Intrinsic::powi), {LeftOperand, RightOperand});
        case ArithmeticOp::MOD:
            return IR.CreateSRem(LeftOperand, RightOperand);
        }
}

llvm::Value *CodeGenPass::visitLogicalOp(LogicalOp *Op) {
    Value *LeftOperand = visit(Op->getLeftExpr());
    Value *RightOperand = visit(Op->getRightExpr());

    const Type *LeftType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getLeftExpr());
    const Type *RightType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getRightExpr());
    assert( RightType == LeftType && "Operation between different types should not"
                                     " have reached the code gen");

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
    llvm::BasicBlock *LoopBody = llvm::BasicBlock::Create(GlobalCtx, "LoopBody");
    llvm::BasicBlock *LoopEnd = llvm::BasicBlock::Create(GlobalCtx, "LoopEnd");

    LoopBeginBlocks.push(LoopBody);
    LoopEndBlocks.push(LoopEnd);

    IR.CreateBr(LoopBody);

    CurrentFunction->getBasicBlockList().push_back(LoopBody);
    IR.SetInsertPoint(LoopBody);
    visit(Loop->getBlock());
    IR.CreateBr(LoopBody);
    CurrentFunction->getBasicBlockList().push_back(LoopEnd);
    IR.SetInsertPoint(LoopEnd);
    return nullptr;
}

llvm::Value *CodeGenPass::visitConditionalLoop(ConditionalLoop *Loop) {
    llvm::BasicBlock *Header = llvm::BasicBlock::Create(GlobalCtx, "LoopHeader");
    llvm::BasicBlock *LoopBody = llvm::BasicBlock::Create(GlobalCtx, "LoopBody");
    llvm::BasicBlock *LoopEnd = llvm::BasicBlock::Create(GlobalCtx, "LoopEnd");

    LoopBeginBlocks.push(Header);
    LoopEndBlocks.push(LoopEnd);

    IR.SetInsertPoint(Header);
    Value *Res = visit(Loop->getConditional());
    IR.CreateCondBr(Res, LoopBody, LoopEnd);

    CurrentFunction->getBasicBlockList().push_back(LoopBody);
    IR.SetInsertPoint(LoopBody);
    visit(Loop->getBlock());
    IR.CreateBr(Header);

    CurrentFunction->getBasicBlockList().push_back(LoopEnd);
    IR.SetInsertPoint(LoopEnd);

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
    const Type *ExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(NullLit);
    switch (ExprTy->getKind()) {
        case Type::TypeKind::T_Int:
            return IR.getInt32(0);
        case Type::TypeKind::T_Char:
            return IR.getInt8(0);
        case Type::TypeKind::T_Bool:
            return IR.getInt1(false);
        case Type::TypeKind::T_Real:
            return llvm::ConstantFP::get(LLVMRealTy, llvm::APFloat(0.0));
        default:
            assert(false && "Invalid type for null literal");
    }

}

llvm::Value *CodeGenPass::visitIdentityLiteral(IdentityLiteral *IdentityLit) {
    const Type *ExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(IdentityLit);
    switch (ExprTy->getKind()) {
        case Type::TypeKind::T_Int:
            return IR.getInt32(1);
        case Type::TypeKind::T_Char:
            return IR.getInt8(1);
        case Type::TypeKind::T_Bool:
            return IR.getInt1(true);
        case Type::TypeKind::T_Real:
            return llvm::ConstantFP::get(LLVMRealTy, llvm::APFloat(1.0));
        default:
            assert(false && "Invalid type for identity literal");
    }
}

llvm::Value *CodeGenPass::visitRealLiteral(RealLiteral *RealLit) {
    // TODO verify precision of float
    float val = RealLit->getVal();
    llvm::APFloat apf(val);
    return llvm::ConstantFP::get(GlobalCtx, apf);
}

llvm::Value *CodeGenPass::visitBoolLiteral(BoolLiteral *BoolLit) {
    return IR.getInt1(BoolLit->getVal());
}

llvm::Value *CodeGenPass::visitCharLiteral(CharLiteral *CharLit) {
    return IR.getInt8(CharLit->getCharacter());
}

llvm::Value *CodeGenPass::visitTupleLiteral(TupleLiteral *TupleLit) {
    // TODO
}

llvm::Value *CodeGenPass::visitMemberAccess(MemberAccess *MemberAcc) {
    // All member expressions should be converted to a tuple access by an index
    // at this point
    int MemberIdx = dyn_cast<IntLiteral>(MemberAcc->getMemberExpr())->getVal();
    llvm::Value *Expr = visit(MemberAcc->getExpr());
    llvm::Value *MemberPtr = IR.CreateStructGEP(Expr, MemberIdx);
    return IR.CreateLoad(MemberPtr);

}

llvm::Value *CodeGenPass::visitConditional(Conditional *Cond) {

    llvm::BasicBlock *CondHeader = llvm::BasicBlock::Create(GlobalCtx, "CondHeader");
    llvm::BasicBlock *CondIf = llvm::BasicBlock::Create(GlobalCtx, "CondIf");
    llvm::BasicBlock *CondEnd = llvm::BasicBlock::Create(GlobalCtx, "CondEnd");

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

    llvm::BasicBlock *CondHeader = llvm::BasicBlock::Create(GlobalCtx, "CondHeader");
    llvm::BasicBlock *CondIf = llvm::BasicBlock::Create(GlobalCtx, "CondIf");
    llvm::BasicBlock *CondElse = llvm::BasicBlock::Create(GlobalCtx, "CondElse");
    llvm::BasicBlock *CondEnd = llvm::BasicBlock::Create(GlobalCtx, "CondEnd");

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
                    return IR.CreateSExt(Val, LLVMIntTy);
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
                case Type::TypeKind::T_Char:
                    return IR.CreateSIToFP(Val, LLVMRealTy);
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
    llvm::Function *Func = llvm::Function::Create(
            ProcedureTy,
            llvm::Function::ExternalLinkage,
            "pd_" + ProcedureDef->getIdentifier()->getName(),
            Mod);

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

    // Set main procedure if applicable
    if (ProcedureDef->getIdentifier()->getName() == "main") {
        MainFunction = Func;
    }

    // Set current function
    CurrentFunction = Func;

    // Visit function body
    visit(ProcedureDef->getBlock());

    CurrentFunction = GlobalFunction;

    // The return is defined inside the function body
}

llvm::Value *CodeGenPass::visitProcedureCall(ProcedureCall *ProcedureCall) {
    // The stream state function is a special case,
    // so it is handled here
//    if (ProcedureCall->getIdentifier()->getName() == "stream_state")  {
//        // load the stream state
//        return IR.CreateLoad(StreamStateLoc);
//    }

    // Get the function
    llvm::Function *Func = Mod.getFunction(ProcedureCall->getIdentifier()->getName());
    assert(Func && "Function not found");

    // Get the arguments
    std::vector<llvm::Value *> Args;
    for (size_t i = 0; i < ProcedureCall->getArgsList()->numOfChildren(); i++) {
        ASTNodeT *Expr = ProcedureCall->getArgsList()->getExprAtPos(i);
        const Type *ExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Expr);

        // Constant arguments are passed by value
        if (ExprTy->isConst()) {
            Args.push_back(visit(Expr));

        // Variable arguments are passed by reference
        } else {
            assert(isa<Identifier>(Expr) && "Invalid variable argument to procedure");
            Args.push_back(SymbolMap[cast<Identifier>(Expr)->getReferred()]);
        }
    }

    // Call the function
    return IR.CreateCall(Func, Args);

}

llvm::Value *CodeGenPass::visitReturn(Return *Return) {
    if (Return->getReturnExpr()) {
        llvm::Value *RetVal = visit(Return->getReturnExpr());
        IR.CreateRet(RetVal);
    } else {
        IR.CreateRetVoid();
    }
    return nullptr;

}

llvm::Value *CodeGenPass::visitBreak(Break *Break) {
    llvm::BasicBlock *AfterBreak = llvm::BasicBlock::Create(GlobalCtx, "AfterBreak");
    llvm::BasicBlock *LoopEnd = LoopEndBlocks.top();
    LoopBeginBlocks.pop();
    LoopEndBlocks.pop();

    IR.CreateBr(LoopEnd);

    CurrentFunction->getBasicBlockList().push_back(AfterBreak);
    IR.SetInsertPoint(AfterBreak);
    return nullptr;

}

llvm::Value *CodeGenPass::visitContinue(Continue *Continue) {

    llvm::BasicBlock *AfterContinue = llvm::BasicBlock::Create(GlobalCtx, "AfterContinue");
    llvm::BasicBlock *LoopEnd = LoopBeginBlocks.top();
    LoopBeginBlocks.pop();
    LoopEndBlocks.pop();

    IR.CreateBr(LoopEnd);

    CurrentFunction->getBasicBlockList().push_back(AfterContinue);
    IR.SetInsertPoint(AfterContinue);
    return nullptr;

}

llvm::Value *CodeGenPass::visitOutStream(OutStream *OutStream) {
//    Value *ValToOut = visit(Stream->getOutStreamExpr());
//    Type ValType = PM->getAnnotation<ExprTypeAnnotatorPass>(*Stream->getOutStreamExpr());
//    // TODO depends on the TypeRegistry Implementation
//    if (ValType == CharType) {
//        // TODO enforce ValToOut = 0/1 for null and identity
//        IR.CreateCall(PrintCharFunc, {ValToOut});
//    } else if (ValType == IntegerType) {
//        IR.CreateCall(PrintIntFunc, {ValToOut});
//    } else if (ValType == RealType) {
//        IR.CreateCall(PrintRealFunc, {ValToOut});
//    } else if (ValType == BoolType) {
//        IR.CreateCall(PrintBoolFunc, {ValToOut});
//    } else {
//        // should not reach here ever
//        assert(false && "Cannot output non-output type");
//    }
//    return nullptr;
}

llvm::Value *CodeGenPass::visitInStream(InStream *InStream) {
    const Type *IdentTy = PM->getAnnotation<ExprTypeAnnotatorPass>(InStream->getIdentifier());
    Value *StoreLoc = SymbolMap[InStream->getIdentifier()->getReferred()];

//    if (IdentTy == CharType) {
//        IR.CreateCall(ReadCharFunc, {StoreLoc, StreamStateLoc, Buffer});
//    } else if (IdentTy == IntegerType) {
//        IR.CreateCall(ReadIntFunc, {StoreLoc, StreamStateLoc, Buffer});
//    } else if (IdentTy == RealType) {
//        IR.CreateCall(ReadRealFunc, {StoreLoc, StreamStateLoc, Buffer});
//    } else if (IdentTy == BoolType) {
//        IR.CreateCall(ReadBoolFunc, {StoreLoc, StreamStateLoc, Buffer});
//    } else {
//        // should not reach here ever
//        assert(false && "Cannot input non-input type");
//    }
    return nullptr;
}
