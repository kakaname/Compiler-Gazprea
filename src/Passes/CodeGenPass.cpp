////
//// Created by Chen on 2022-11-09.
////
//
//#include "Passes/CodeGenPass.h"
//#include "Passes/LoopCheckPass.h"
//
//using llvm::Value;
//
//void CodeGenPass::runOnAST(ASTPassManager &Manager, ASTNodeT &Root) {
////    PM = &Manager;
////    ScopeTreeNode &GlobalScope = Manager.getResult<ScopeResolutionPass>();
////
////    llvm::BasicBlock *Entry = llvm::BasicBlock::Create(GlobalCtx, "entry", MainFunc);
////    llvm::BasicBlock *Prog = llvm::BasicBlock::Create(GlobalCtx, "prog", MainFunc);
////
//
//}
//
//llvm::Value *CodeGenPass::visitProgram(Program *Prog) {
//
//}
//
//llvm::Value *CodeGenPass::visitIdentifier(Identifier *Ident) {
////    Type *IdentType = PM->getAnnotation<ExprTypeAnnotatorPass>(*Ident);
////    // If the identifier is an integer, load it and return the value.
////    return IR.CreateLoad(SymbolMap[Ident->getSymbolId()]);
//}
//
//llvm::Value *CodeGenPass::visitAssignment(Assignment *Assign) {
////    Value *StoreVal = visit(Assign->getExpr());
////    Type *AssignType = PM->getAnnotation<ExprTypeAnnotatorPass>(*Assign->getExpr());
////    Value *StoreLoc = SymbolMap[Assign->getExpr()->getSymbolId()];
////
////    // We are storing an integer. So we know the value of the expression is
////    // in register itself.
////    IR.CreateStore(StoreVal, StoreLoc);
//}
//
//llvm::Value *CodeGenPass::visitDeclaration(Declaration *Decl) {
////    Value *InitValue = visit(Decl->getInitExpr());
////    Type DeclType = PM->getAnnotation<ExprTypeAnnotatorPass>(*Decl->getInitExpr());
////    // We are declaring an integer.
////    Value *DeclValue = createAlloca(LLVMIntType);
////    IR.CreateStore(InitValue, DeclValue);
////    SymbolMap[Decl->getExpr()->getSymbolId()] = DeclValue;
////    return nullptr;
//
//}
//
//llvm::Value *CodeGenPass::visitLogicalOp(ComparisonOp *Op) {
//    Value *LeftOperand = visit(Op->getLeftExpr());
//    Value *RightOperand = visit(Op->getRightExpr());
//
//    // Just an assertion, not needed for code gen.
//    Type* LeftType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getLeftExpr());
//    Type* RightType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getRightExpr());
//    assert( RightType == LeftType && "Operation between different types should not"
//                                     " have reached the code gen");
//
//    llvm::CmpInst::Predicate Pred;
//    switch (Op->getOpKind()) {
//        case ComparisonOp::EQEQ:
//            Pred = llvm::CmpInst::Predicate::ICMP_EQ;
//            break;
//        case ComparisonOp::NEQ:
//            Pred = llvm::CmpInst::Predicate::ICMP_NE;
//            break;
//        case ComparisonOp::GT:
//            Pred = llvm::CmpInst::Predicate::ICMP_SGT;
//            break;
//        case ComparisonOp::LT:
//            Pred = llvm::CmpInst::Predicate::ICMP_SLT;
//            break;
//        case ComparisonOp::LTEQ:
//            Pred = llvm::CmpInst::Predicate::ICMP_SLE;
//            break;
//        case ComparisonOp::GTEQ:
//            Pred = llvm::CmpInst::Predicate::ICMP_SGE;
//            break;
//    }
//
//    return IR.CreateICmp(Pred, LeftOperand, RightOperand);
//
//}
//
//llvm::Value *CodeGenPass::visitArithmeticOp(ArithmeticOp *Op) {
////    Value *LeftOperand = visit(Op->getLeftExpr());
////    Value *RightOperand = visit(Op->getRightExpr());
////    Type *OpType = PM->getAnnotation<ExprTypeAnnotatorPass>(*Op);
////
////    // Just an assertion, not needed for code gen.
////    Type *LeftType = PM->getAnnotation<ExprTypeAnnotatorPass>(*Op->getLeftExpr());
////    Type *RightType = PM->getAnnotation<ExprTypeAnnotatorPass>(*Op->getRightExpr());
////    assert( RightType == LeftType && "Operation between different types should not"
////                                     " have reached the code gen");
////
////    // Easy integer case where we just return the result of the operation.
////        switch (Op->getOpKind()) {
////            case ArithmeticOp::ADD:
////                return IR.CreateAdd(LeftOperand, RightOperand);
////            case ArithmeticOp::SUB:
////                return IR.CreateSub(LeftOperand, RightOperand);
////            case ArithmeticOp::MUL:
////                return IR.CreateMul(LeftOperand, RightOperand);
////            case ArithmeticOp::DIV:
////                return IR.CreateSDiv(LeftOperand, RightOperand);
////            case ArithmeticOp::EXP:
////                // get llvm exponentiation function
////                return IR.CreateCall(llvm::Intrinsic::getDeclaration(&Mod, llvm::Intrinsic::powi), {LeftOperand, RightOperand});
////            case ArithmeticOp::MOD:
////                return IR.CreateSRem(LeftOperand, RightOperand);
////            default:
////                assert(false && "Unknown arithmetic operation");
////        }
//}
//
//// Ignored for part1
//llvm::Value *CodeGenPass::visitIndex(Index *Idx) {
//
//}
//
//llvm::Value *CodeGenPass::visitInfiniteLoop(InfiniteLoop *Loop) {
////    llvm::BasicBlock *LoopBody = llvm::BasicBlock::Create(GlobalCtx, "LoopBody");
////    llvm::BasicBlock *LoopEnd = llvm::BasicBlock::Create(GlobalCtx, "LoopEnd");
////    // FIXME this is a hack to annotate the LoopBody and LoopEnd
////    PM->setAnnotation<CodeGenPass>(Loop, LoopEnd);
////    PM->setAnnotation<CodeGenPass>(Loop->getStatement(), LoopBody);
////    IR.CreateBr(LoopBody);
////
////    MainFunc->getBasicBlockList().push_back(LoopBody);
////    IR.SetInsertPoint(LoopBody);
////    visit(Loop->getStatement());
////    IR.CreateBr(LoopBody);
////    MainFunc->getBasicBlockList().push_back(LoopEnd);
////    IR.SetInsertPoint(LoopEnd);
////    return nullptr
//}
//
//llvm::Value *CodeGenPass::visitConditionalLoop(ConditionalLoop *Loop) {
////    Type* CondType = PM->getAnnotation<ExprTypeAnnotatorPass>(Loop->getConditional())
////    assert(CondType == BoolType);
////    llvm::BasicBlock *Header = llvm::BasicBlock::Create(GlobalCtx, "LoopHeader", MainFunc);
////    llvm::BasicBlock *LoopBody = llvm::BasicBlock::Create(GlobalCtx, "LoopBody");
////    llvm::BasicBlock *LoopEnd = llvm::BasicBlock::Create(GlobalCtx, "LoopEnd");
////    PM->setAnnotation<CodeGenPass>(Loop, LoopEnd);
////    if (Loop->ConditionalBefore) {
////        IR.CreateBr(Header);
////    } else {
////        IR.CreateBr(LoopBody);
////    }
////    IR.SetInsertPoint(Header);
////    Value *Res = visit(Loop->getConditional());
////    IR.CreateCondBr(Res, LoopBody, LoopEnd);
////
////    MainFunc->getBasicBlockList().push_back(LoopBody);
////    IR.SetInsertPoint(LoopBody);
////    visit(Loop->getStatement());
////    IR.CreateBr(Header);
////    MainFunc->getBasicBlockList().push_back(LoopEnd);
////    IR.SetInsertPoint(LoopEnd);
////    return nullptr;
//}
//
//// ignored for part1
//llvm::Value *CodeGenPass::visitDomainLoop(DomainLoop *Loop) {
//
//}
//
//llvm::Value *CodeGenPass::visitIntLiteral(IntLiteral *IntLit) {
//    return IR.getInt32(IntLit->getVal());
//}
//
//llvm::Value *CodeGenPass::visitNullLiteral(NullLiteral *NullLit) {
//    Type *ExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(NullLit);
//    switch (ExprTy->getKind()) {
//        case Type::TypeKind::T_Int:
//            return IR.getInt32(0);
//        case Type::TypeKind::T_Char:
//            return IR.getInt8(0);
//        case Type::TypeKind::T_Bool:
//            return IR.getInt1(false);
//        case Type::TypeKind::T_Real:
//            return llvm::ConstantFP::get(llvm::Type::getDoubleTy(GlobalCtx), llvm::APFloat(0.0));
//        default:
//            assert(false && "Invalid type for null literal");
//            return nullptr;
//    }
//
//}
//
//llvm::Value *CodeGenPass::visitIdentityLiteral(IdentityLiteral *IdentityLit) {
//    Type *ExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(IdentityLit);
//    switch (ExprTy->getKind()) {
//        case Type::TypeKind::T_Int:
//            return IR.getInt32(1);
//        case Type::TypeKind::T_Char:
//            return IR.getInt8(1);
//        case Type::TypeKind::T_Bool:
//            return IR.getInt1(true);
//        case Type::TypeKind::T_Real:
//            return llvm::ConstantFP::get(llvm::Type::getDoubleTy(GlobalCtx), llvm::APFloat(1.0));
//        default:
//            assert(false && "Invalid type for identity literal");
//            return nullptr;
//    }
//}
//
//llvm::Value *CodeGenPass::visitRealLiteral(RealLiteral *RealLit) {
//    // get float value
//    float val = RealLit->getVal();
//    // convert to llvm IR
//    // TODO verify precision of float
//    // TODO exponential notation
//
//    llvm::APFloat apf(val);
//    return llvm::ConstantFP::get(GlobalCtx, apf);
//}
//
//llvm::Value *CodeGenPass::visitBoolLiteral(BoolLiteral *BoolLit) {
//    return IR.getInt1(BoolLit->getVal());
//}
//
//llvm::Value *CodeGenPass::visitCharLiteral(CharLiteral *CharLit) {
//    // TODO verify escape sequences are handled correctly in ASTBuilder
//    return IR.getInt8(CharLit->getCharacter());
//}
//
//llvm::Value *CodeGenPass::visitTupleLiteral(TupleLiteral *TupleLit) {
//
//}
//
//llvm::Value *CodeGenPass::visitMemberAccess(MemberAccess *MemberAcc) {
//
//}
//
//llvm::Value *CodeGenPass::visitTupleTypeDecl(TupleTypeDecl *TupleTypeDecl) {
//
//}
//
//llvm::Value *CodeGenPass::visitConditional(Conditional *Cond) {
//
//}
//
//llvm::Value *CodeGenPass::visitConditionalElse(ConditionalElse *Cond) {
//
//}
//
//llvm::Value *CodeGenPass::visitTypeCast(TypeCast *Cast) {
//
//}
//
//llvm::Value *CodeGenPass::visitBitwiseOp(LogicalOp *Op) {
//    Value *LeftOperand = visit(Op->getLeftExpr());
//    Value *RightOperand = visit(Op->getRightExpr());
//
//    // Just an assertion, not needed for code gen.
//    Type* LeftType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getLeftExpr());
//    Type* RightType = PM->getAnnotation<ExprTypeAnnotatorPass>(Op->getRightExpr());
//    assert( RightType == LeftType && "Operation between different types should not"
//                                     " have reached the code gen");
//
//    llvm::CmpInst::Predicate Pred;
//    switch (Op->getOpKind()) {
//        case LogicalOp::AND:
//            return IR.CreateAnd(LeftOperand, RightOperand);
//        case LogicalOp::OR:
//            return IR.CreateOr(LeftOperand, RightOperand);
//        case LogicalOp::XOR:
//            return IR.CreateXor(LeftOperand, RightOperand);
//        default:
//            assert(false && "Unknown bitwise operation");
//    }
//}
//
//llvm::Value *CodeGenPass::visitUnaryOp(UnaryOp *Op) {
//    Value *Operand = visit(Op->getExpr());
//
//    switch (Op->getOpKind()) {
//        case UnaryOp::NOT:
//            return IR.CreateNot(Operand);
//        case UnaryOp::ADD:
//            return Operand;
//        case UnaryOp::SUB:
//            return IR.CreateNeg(Operand);
//    }
//
//}
//
//llvm::Value *CodeGenPass::visitArgsList(ArgsList *List) {
//
//}
////
////llvm::Value *CodeGenPass::visitParasList(ParasList *List) {
////
////}
//
//llvm::Value *CodeGenPass::visitFunctionDecl(FunctionDecl *FuncDecl) {
//
//}
//
//llvm::Value *CodeGenPass::visitFunctionDef(FunctionDef *FuncDef) {
//
//}
//
//llvm::Value *CodeGenPass::visitFunctionCall(FunctionCall *FuncCall) {
//
//}
//
//llvm::Value *CodeGenPass::visitProcedureDecl(ProcedureDecl *ProcedureDecl) {
//
//}
//
//llvm::Value *CodeGenPass::visitProcedureDef(ProcedureDef *ProcedureDef) {
//
//}
//
//llvm::Value *CodeGenPass::visitProcedureCall(ProcedureCall *ProcedureCall) {
////    // The stream state function is a special case
////    // so it is handled here
////    if (Ident->getName() == "stream_state")  {
////        // load the stream state
////        Value *StreamState = IR.CreateLoad(StreamStateLoc);
////        // TODO setup return value
////    }
//}
//
//llvm::Value *CodeGenPass::visitReturn(Return *Return) {
//
//}
//
//llvm::Value *CodeGenPass::visitBreak(Break *Break) {
////    llvm::BasicBlock *AfterBreak = llvm::BasicBlock::Create(GlobalCtx, "AfterBreak");
////    TreeNode *Loop = PM->getAnnotation<LoopCheckPass>(Break);
////    // FIXME check if this works
////    auto *LoopEnd = static_cast<llvm::BasicBlock *>(PM->getAnnotation<CodeGenPass>(Loop));
////    IR.CreateBr(LoopEnd);
////    MainFunc->getBasicBlockList().push_back(AfterBreak);
////    IR.SetInsertPoint(AfterBreak);
////    return nullptr;
//
//}
//
//llvm::Value *CodeGenPass::visitContinue(Continue *Continue) {
//
////    llvm::BasicBlock *AfterBreak = llvm::BasicBlock::Create(GlobalCtx, "AfterContinue");
////    TreeNode *Loop = PM->getAnnotation<LoopCheckPass>(Continue);
////    // FIXME check if this works
////    if (Loop->getKind() == TreeNode::N_AST_InfiniteLoop) {
////        auto *ILoop = dynamic_cast<InfiniteLoop *>(Loop);
////        auto *LoopBody = static_cast<llvm::BasicBlock *>(PM->getAnnotation<CodeGenPass>(ILoop->getStatement()));
////        IR.CreateBr(LoopBody);
////    } else if (Loop->getKind() == TreeNode::N_AST_ConditionalLoop) {
////        auto *CLoop = dynamic_cast<ConditionalLoop *>(Loop);
////        auto *LoopBody = static_cast<llvm::BasicBlock *>(PM->getAnnotation<CodeGenPass>(CLoop->getStatement()));
////        IR.CreateBr(LoopBody);
////    } else {
////        assert(false && "Unknown loop type");
////    }
////    MainFunc->getBasicBlockList().push_back(AfterBreak);
////    IR.SetInsertPoint(AfterBreak);
////    return nullptr;
//
//}
//
//llvm::Value *CodeGenPass::visitOutStream(OutStream *OutStream) {
////    Value *ValToOut = visit(Stream->getOutStreamExpr());
////    Type ValType = PM->getAnnotation<ExprTypeAnnotatorPass>(*Stream->getOutStreamExpr());
////    // TODO depends on the TypeRegistry Implementation
////    if (ValType == CharType) {
////        // TODO enforce ValToOut = 0/1 for null and identity
////        IR.CreateCall(PrintCharFunc, {ValToOut});
////    } else if (ValType == IntegerType) {
////        IR.CreateCall(PrintIntFunc, {ValToOut});
////    } else if (ValType == RealType) {
////        IR.CreateCall(PrintRealFunc, {ValToOut});
////    } else if (ValType == BoolType) {
////        IR.CreateCall(PrintBoolFunc, {ValToOut});
////    } else {
////        // should not reach here ever
////        assert(false && "Cannot output non-output type");
////    }
////    return nullptr;
//}
//
//llvm::Value *CodeGenPass::visitInStream(InStream *InStream) {
////    Type IdentTy = PM->getAnnotation<ExprTypeAnnotatorPass>(*Stream->getInStreamIdent());
////    Value *StoreLoc = SymbolMap[Stream->getInStreamIdent()->getSymbolId()];
////
////    if (IdentTy == CharType) {
////        IR.CreateCall(ReadCharFunc, {StoreLoc, StreamStateLoc, Buffer});
////    } else if (IdentTy == IntegerType) {
////        IR.CreateCall(ReadIntFunc, {StoreLoc, StreamStateLoc, Buffer});
////    } else if (IdentTy == RealType) {
////        IR.CreateCall(ReadRealFunc, {StoreLoc, StreamStateLoc, Buffer});
////    } else if (IdentTy == BoolType) {
////        IR.CreateCall(ReadBoolFunc, {StoreLoc, StreamStateLoc, Buffer});
////    } else {
////        // should not reach here ever
////        assert(false && "Cannot input non-input type");
////    }
////    return nullptr;
//}
//
//llvm::Value *CodeGenPass::visitExplicitCast(ExplicitCast *ExplicitCast) {
//
//}
//
//
