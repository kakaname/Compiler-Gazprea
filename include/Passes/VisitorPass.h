//
// Created by Chen on 2022-11-08.
//

#ifndef GAZPREABASE_VISITORPASS_H
#define GAZPREABASE_VISITORPASS_H

#include <cassert>
#include "Pass.h"
#include "AST/ASTNodes.h"

using llvm::dyn_cast;

template<typename DerivedT, typename RetT>
class VisitorPass: public ASTPassIDMixin<DerivedT> {

    RetT visitProgram(Program *Prog) {
        for (auto *child : *Prog)
            visit(child);
        return RetT();
    }

    RetT visitNoOp(NoOp *Op) {
        return RetT();
    }

    RetT visitIdentifier(Identifier *Ident) {
        return RetT();
    }

    RetT visitAssignment(Assignment *Assign) {
        visit(Assign->getAssignedTo());
        visit(Assign->getExpr());
        return RetT();
    }

    RetT visitMemberReference(MemberReference *Ref) {
        visit(Ref->getIdentifier());
        visit(Ref->getMemberExpr());
        return RetT();
    }

    RetT visitIdentReference(IdentReference *Ref) {
        return visit(Ref->getIdentifier());
    }

    RetT visitDeclaration(Declaration *Decl) {
        visit(Decl->getIdentifier());
        visit(Decl->getInitExpr());
        return RetT();
    }

    RetT visitBlock(Block *Blk) {
        for (auto *Child : *Blk)
            visit(Child);
        return RetT();
    }

    RetT visitComparisonOp(ComparisonOp *Op) {
        visit(Op->getLeftExpr());
        visit(Op->getRightExpr());
        return RetT();
    }

    RetT visitArithmeticOp(ArithmeticOp *Op) {
        visit(Op->getLeftExpr());
        visit(Op->getRightExpr());
        return RetT();
    }

    RetT visitIndex(Index *Idx) {
        visit(Idx->getBaseExpr());
        visit(Idx->getIndexExpr());
        return RetT();
    }

    RetT visitIndexReference(IndexReference *Ref) {
        visit(Ref->getBaseExpr());
        visit(Ref->getIndexExpr());
        return RetT();
    }

    RetT visitInfiniteLoop(InfiniteLoop *Loop) {
        visit(Loop->getBlock());
        return RetT();
    }

    RetT visitConditionalLoop(ConditionalLoop *Loop) {
        visit(Loop->getConditional());
        visit(Loop->getBlock());
        return RetT();
    }

    // ignored for part1
    RetT visitDomainLoop(DomainLoop *Loop) {
        throw std::runtime_error("Unimplemented");
    }

    RetT visitIntLiteral(IntLiteral *IntLit) {
        return RetT();
    }

    RetT visitNullLiteral(NullLiteral *NullLit) {
        return RetT();
    }

    RetT visitIdentityLiteral(IdentityLiteral *IdentityLit) {
        return RetT();
    }

    RetT visitRealLiteral(RealLiteral *RealLit) {
        return RetT();
    }

    RetT visitBoolLiteral(BoolLiteral *BoolLit) {
        return RetT();
    }

    RetT visitCharLiteral(CharLiteral *CharLit) {
        return RetT();
    }

    RetT visitTupleLiteral(TupleLiteral *TupleLit) {
        for (auto *child : *TupleLit)
            visit(child);
        return RetT();
    }

    RetT visitMemberAccess(MemberAccess *MemberAcc) {
        visit(MemberAcc->getExpr());
        visit(MemberAcc->getMemberExpr());
        return RetT();
    }

    RetT visitConditional(Conditional *Cond) {
        visit(Cond->getConditional());
        visit(Cond->getBlock());
        return RetT();
    }

    RetT visitConditionalElse(ConditionalElse *Cond) {
        visit(Cond->getConditional());
        visit(Cond->getIfBlock());
        visit(Cond->getElseBlock());
        return RetT();
    }

    RetT visitTypeCast(TypeCast *Cast) {
        visit(Cast->getExpr());
        return RetT();
    }

    RetT visitLogicalOp(LogicalOp *Op) {
        visit(Op->getLeftExpr());
        visit(Op->getRightExpr());
        return RetT();
    }

    RetT visitUnaryOp(UnaryOp *Op) {
        visit(Op->getExpr());
        return RetT();
    }

    RetT visitArgsList(ArgsList *List) {
        for (auto *child : *List)
            visit(child);
        return RetT();
    }

    RetT visitCalleeParameter(CalleeParameter *CalleeParameter) {
        visit(CalleeParameter->getTypeNode());
        visit(CalleeParameter->getIdentifier());
        return RetT();
    }

    RetT visitParameterList(ParameterList *List) {
        for (auto *child : *List) {
            visit(child);
        }
        return RetT();
    }

    RetT visitFunctionDecl(FunctionDecl *FuncDecl) {
        visit(FuncDecl->getIdentifier());
        return RetT();
    }

    RetT visitFunctionDef(FunctionDef *FuncDef) {
        visit(FuncDef->getIdentifier());
        visit(FuncDef->getParamList());
        visit(FuncDef->getBlock());
        return RetT();
    }

    RetT visitResolvedType(ResolvedType *ResolvedType) {
        return RetT();
    }

    RetT visitFunctionCall(FunctionCall *FuncCall) {
        visit(FuncCall->getIdentifier());
        visit(FuncCall->getArgsList());
        return RetT();
    }

    RetT visitProcedureDecl(ProcedureDecl *ProcedureDecl) {
        visit(ProcedureDecl->getIdentifier());
        return RetT();
    }

    RetT visitProcedureDef(ProcedureDef *ProcedureDef) {
        visit(ProcedureDef->getIdentifier());
        visit(ProcedureDef->getParamList());
        visit(ProcedureDef->getBlock());
        return RetT();
    }

    RetT visitProcedureCall(ProcedureCall *ProcedureCall) {
        visit(ProcedureCall->getIdentifier());
        visit(ProcedureCall->getArgsList());
        return RetT();
    }

    RetT visitReturn(Return *Return) {
        return visit(Return->getReturnExpr());
    }

    RetT visitBreak(Break *Break) {
        return RetT();
    }

    RetT visitContinue(Continue *Continue) {
        return RetT();
    }

    RetT visitOutStream(OutStream *OutStream) {
        return visit(OutStream->getOutStreamExpr());
    }

    RetT visitInStream(InStream *InStream) {
        return visit(InStream->getTarget());
    }

    RetT visitExplicitCast(ExplicitCast *ExplicitCast) {
        return visit(ExplicitCast->getExpr());
    }

    RetT visitVectorLiteral(VectorLiteral *Vector) {
        for (auto *child : *Vector)
            visit(child);
    }

    RetT visitInterval(Interval *Interval) {
        visit(Interval->getUpperExpr());
        visit(Interval->getLowerExpr());
        return RetT();
    }

    RetT visitFreeNode(FreeNode *FreeNode) {
        for (auto *Child : *FreeNode)
            visit(Child);
        return RetT();
    }

    RetT visitGenerator(Generator *Gen) {
        visit(Gen->getDomainVar());
        visit(Gen->getDomain());
        visit(Gen->getExpr());
        return RetT();
    }

    RetT visitAppendNode(AppendNode *Append) {
        visit(Append->getLeftExpr());
        visit(Append->getRightExpr());
        return RetT();
    }

    RetT callVisitProgramImpl(Program *Prog) {
        return static_cast<DerivedT*>(this)->visitProgram(Prog);
    }

    RetT callVisitNoOpImpl(NoOp *NOp) {
        return static_cast<DerivedT*>(this)->visitNoOp(NOp);
    }

    RetT callVisitIdentifierImpl(Identifier *Ident) {
        return static_cast<DerivedT*>(this)->visitIdentifier(Ident);
    }

    RetT callVisitAssignmentImpl(Assignment *Assign) {
        return static_cast<DerivedT*>(this)->visitAssignment(Assign);
    }

    RetT callVisitMemberReferenceImpl(MemberReference *Node) {
        return static_cast<DerivedT*>(this)->visitMemberReference(Node);
    }

    RetT callVisitDeclarationImpl(Declaration *Decl) {
        return static_cast<DerivedT*>(this)->visitDeclaration(Decl);
    }

    RetT callVisitBlockImpl(Block *Block) {
        return static_cast<DerivedT*>(this)->visitBlock(Block);
    }

    RetT callVisitComparisonOpImpl(ComparisonOp *CompareOp) {
        return static_cast<DerivedT*>(this)->visitComparisonOp(CompareOp);
    }

    RetT callVisitArithmeticOpImpl(ArithmeticOp *ArithOp) {
        return static_cast<DerivedT*>(this)->visitArithmeticOp(ArithOp);
    }

    RetT callVisitIdentReferenceImpl(IdentReference *Ref) {
        return static_cast<DerivedT*>(this)->visitIdentReference(Ref);
    }

    RetT callVisitInfiniteLoopImpl(InfiniteLoop *Loop) {
        return static_cast<DerivedT*>(this)->visitInfiniteLoop(Loop);
    }

    RetT callVisitConditionalLoopImpl(ConditionalLoop *CondLoop) {
        return static_cast<DerivedT*>(this)->visitConditionalLoop(CondLoop);
    }

    RetT callVisitDomainLoopImpl(DomainLoop *DomainLoop) {
        return static_cast<DerivedT*>(this)->visitDomainLoop(DomainLoop);
    }

    RetT callVisitIntLiteralImpl(IntLiteral *IntLiteral) {
        return static_cast<DerivedT*>(this)->visitIntLiteral(IntLiteral);
    }

    RetT callVisitNullLiteralImpl(NullLiteral *NullLiteral) {
        return static_cast<DerivedT*>(this)->visitNullLiteral(NullLiteral);
    }

    RetT callVisitIdentityLiteralImpl(IdentityLiteral *Identity) {
        return static_cast<DerivedT*>(this)->visitIdentityLiteral(Identity);
    }

    RetT callVisitRealLiteralImpl(RealLiteral *RealLiteral) {
        return static_cast<DerivedT*>(this)->visitRealLiteral(RealLiteral);
    }

    RetT callVisitBoolLiteralImpl(BoolLiteral *BoolLiteral) {
        return static_cast<DerivedT*>(this)->visitBoolLiteral(BoolLiteral);
    }

    RetT callVisitCharLiteralImpl(CharLiteral *CharLiteral) {
        return static_cast<DerivedT*>(this)->visitCharLiteral(CharLiteral);
    }

    RetT callVisitTupleLiteralImpl(TupleLiteral *TupleLiteral) {
        return static_cast<DerivedT*>(this)->visitTupleLiteral(TupleLiteral);
    }

    RetT callVisitMemberAccessImpl(MemberAccess *MemberAccess) {
        return static_cast<DerivedT*>(this)->visitMemberAccess(MemberAccess);
    }

    RetT callVisitConditionalImpl(Conditional *Cond) {
        return static_cast<DerivedT*>(this)->visitConditional(Cond);
    }

    RetT callVisitConditionalElseImpl(ConditionalElse *CondElse) {
        return static_cast<DerivedT*>(this)->visitConditionalElse(CondElse);
    }

    RetT callVisitTypeCastImpl(TypeCast *T) {
        return static_cast<DerivedT*>(this)->visitTypeCast(T);
    }

    RetT callVisitLogicalOpImpl(LogicalOp *LogicalOp) {
        return static_cast<DerivedT*>(this)->visitLogicalOp(LogicalOp);
    }

    RetT callVisitUnaryOpImpl(UnaryOp *O) {
        return static_cast<DerivedT*>(this)->visitUnaryOp(O);
    }

    RetT callVisitArgsListImpl(ArgsList *A) {
        return static_cast<DerivedT*>(this)->visitArgsList(A);
    }

    RetT callVisitCalleeParameterImpl(CalleeParameter *CalleeParameter) {
        return static_cast<DerivedT*>(this)->visitCalleeParameter(CalleeParameter);
    }

    RetT callVisitParameterListImpl(ParameterList *List) {
        return static_cast<DerivedT*>(this)->visitParameterList(List);
    }

    RetT callVisitFunctionDeclImpl(FunctionDecl *F) {
        return static_cast<DerivedT*>(this)->visitFunctionDecl(F);
    }

    RetT callVisitFunctionDefImpl(FunctionDef *F) {
        return static_cast<DerivedT*>(this)->visitFunctionDef(F);
    }

    RetT callVisitResolvedTypeImpl(ResolvedType *ResolvedType) {
        return static_cast<DerivedT*>(this)->visitResolvedType(ResolvedType);
    }

    RetT callVisitFunctionCallImpl(FunctionCall *F) {
        return static_cast<DerivedT*>(this)->visitFunctionCall(F);
    }

    RetT callVisitProcedureDeclImpl(ProcedureDecl *P) {
        return static_cast<DerivedT*>(this)->visitProcedureDecl(P);
    }

    RetT callVisitProcedureDefImpl(ProcedureDef *P) {
        return static_cast<DerivedT*>(this)->visitProcedureDef(P);
    }

    RetT callVisitProcedureCallImpl(ProcedureCall *P) {
        return static_cast<DerivedT*>(this)->visitProcedureCall(P);
    }

    RetT callVisitReturnImpl(Return *R) {
        return static_cast<DerivedT*>(this)->visitReturn(R);
    }

    RetT callVisitBreakImpl(Break *B) {
        return static_cast<DerivedT*>(this)->visitBreak(B);
    }

    RetT callVisitContinueImpl(Continue *C) {
        return static_cast<DerivedT*>(this)->visitContinue(C);
    }

    RetT callVisitOutStreamImpl(OutStream *S) {
        return static_cast<DerivedT*>(this)->visitOutStream(S);
    }

    RetT callVisitInStreamImpl(InStream *S) {
        return static_cast<DerivedT*>(this)->visitInStream(S);
    }

    RetT callVisitExplicitCastImpl(ExplicitCast *C) {
        return static_cast<DerivedT*>(this)->visitExplicitCast(C);
    }

    RetT callVisitVectorLiteralImpl(VectorLiteral *V) {
        return static_cast<DerivedT*>(this)->visitVectorLiteral(V);
    }

    RetT callVisitIndexImpl(Index *I) {
        return static_cast<DerivedT*>(this)->visitIndex(I);
    }

    RetT callVisitIndexReferenceImpl(IndexReference *I) {
        return static_cast<DerivedT*>(this)->visitIndexReference(I);
    }
    RetT callVisitIntervalImpl(Interval *I) {
        return static_cast<DerivedT*>(this)->visitInterval(I);
    }

    RetT callVisitFreeNodeImpl(FreeNode *N) {
        return static_cast<DerivedT*>(this)->visitFreeNode(N);
    }

    RetT callVisitGeneratorImpl(Generator *G) {
        return static_cast<DerivedT*>(this)->visitGenerator(G);
    }

    RetT callVisitAppendNodeImpl(AppendNode *Append) {
        return static_cast<DerivedT*>(this)->visitAppendNode(Append);
    }

public:
    RetT visit(ASTNodeT *Node) {
        assert(Node && "Tried to visit empty node");


        if (auto *Prog = dyn_cast<Program>(Node))
            return callVisitProgramImpl(Prog);

        if (auto NOp = dyn_cast<NoOp>(Node))
            return callVisitNoOpImpl(NOp);

        if (auto *Ident = dyn_cast<Identifier>(Node))
            return callVisitIdentifierImpl(Ident);

        if (auto *Assign = dyn_cast<Assignment>(Node))
            return callVisitAssignmentImpl(Assign);

        if (auto *MemRef = dyn_cast<MemberReference>(Node))
            return callVisitMemberReferenceImpl(MemRef);

        if (auto *IndexRef = dyn_cast<IdentReference>(Node))
            return callVisitIdentReferenceImpl(IndexRef);

        if (auto *Decl = dyn_cast<Declaration>(Node))
            return callVisitDeclarationImpl(Decl);

        if (auto *Blk = dyn_cast<Block>(Node))
            return callVisitBlockImpl(Blk);

        if (auto *CompOp = dyn_cast<ComparisonOp>(Node))
            return callVisitComparisonOpImpl(CompOp);

        if (auto *ArithOp = dyn_cast<ArithmeticOp>(Node))
            return callVisitArithmeticOpImpl(ArithOp);

        if (auto *Idx = dyn_cast<Index>(Node))
            return callVisitIndexImpl(Idx);

        if (auto *IdxRef = dyn_cast<IndexReference>(Node))
            return callVisitIndexReferenceImpl(IdxRef);

        if (auto *Int = dyn_cast<Interval>(Node))
            return callVisitIntervalImpl(Int);

        if (auto *InfLoop = dyn_cast<InfiniteLoop>(Node))
            return callVisitInfiniteLoopImpl(InfLoop);

        if (auto *CondLoop = dyn_cast<ConditionalLoop>(Node))
            return callVisitConditionalLoopImpl(CondLoop);

        if (auto *DomLoop = dyn_cast<DomainLoop>(Node))
            return callVisitDomainLoopImpl(DomLoop);

        if (auto *IntLit = dyn_cast<IntLiteral>(Node))
            return callVisitIntLiteralImpl(IntLit);

        if (auto *NullLit = dyn_cast<NullLiteral>(Node))
            return callVisitNullLiteralImpl(NullLit);

        if (auto *IdentLit = dyn_cast<IdentityLiteral>(Node))
            return callVisitIdentityLiteralImpl(IdentLit);

        if (auto *RealLit = dyn_cast<RealLiteral>(Node))
            return callVisitRealLiteralImpl(RealLit);

        if (auto *BoolLit = dyn_cast<BoolLiteral>(Node))
            return callVisitBoolLiteralImpl(BoolLit);

        if (auto *CharLit = dyn_cast<CharLiteral>(Node))
            return callVisitCharLiteralImpl(CharLit);

        if (auto *TupleLit = dyn_cast<TupleLiteral>(Node))
            return callVisitTupleLiteralImpl(TupleLit);

        if (auto *MemberAcc = dyn_cast<MemberAccess>(Node))
            return callVisitMemberAccessImpl(MemberAcc);

        if (auto *Cond = dyn_cast<Conditional>(Node))
            return callVisitConditionalImpl(Cond);

        if (auto *CondElse = dyn_cast<ConditionalElse>(Node))
            return callVisitConditionalElseImpl(CondElse);

        if (auto *TypeC = dyn_cast<TypeCast>(Node))
            return callVisitTypeCastImpl(TypeC);

        if (auto *LogicOp = dyn_cast<LogicalOp>(Node))
            return callVisitLogicalOpImpl(LogicOp);

        if (auto *UOp = dyn_cast<UnaryOp>(Node))
            return callVisitUnaryOpImpl(UOp);

        if (auto *ArgsLi = dyn_cast<ArgsList>(Node))
            return callVisitArgsListImpl(ArgsLi);

        if (auto *CalleePara = dyn_cast<CalleeParameter>(Node))
            return callVisitCalleeParameterImpl(CalleePara);

        if (auto *ParaList = dyn_cast<ParameterList>(Node))
            return callVisitParameterListImpl(ParaList);

        if (auto *FunDec = dyn_cast<FunctionDecl>(Node))
            return callVisitFunctionDeclImpl(FunDec);

        if (auto *FuncDef = dyn_cast<FunctionDef>(Node))
            return callVisitFunctionDefImpl(FuncDef);

        if (auto *ResolType = dyn_cast<ResolvedType>(Node)) {
            return callVisitResolvedTypeImpl(ResolType);
        }

        if (auto *FuncCall = dyn_cast<FunctionCall>(Node))
            return callVisitFunctionCallImpl(FuncCall);

        if (auto *ProcedDecl = dyn_cast<ProcedureDecl>(Node))
            return callVisitProcedureDeclImpl(ProcedDecl);

        if (auto *ProcedDef = dyn_cast<ProcedureDef>(Node))
            return callVisitProcedureDefImpl(ProcedDef);

        if (auto *ParamList = dyn_cast<ParameterList>(Node))
            return callVisitParameterListImpl(ParamList);

        if (auto *ProcedCall = dyn_cast<ProcedureCall>(Node))
            return callVisitProcedureCallImpl(ProcedCall);

        if (auto *Ret = dyn_cast<Return>(Node))
            return callVisitReturnImpl(Ret);

        if (auto *Brk = dyn_cast<Break>(Node))
            return callVisitBreakImpl(Brk);

        if (auto *Cont = dyn_cast<Continue>(Node))
            return callVisitContinueImpl(Cont);

        if (auto *OutS = dyn_cast<OutStream>(Node))
            return callVisitOutStreamImpl(OutS);

        if (auto *InS = dyn_cast<InStream>(Node))
            return callVisitInStreamImpl(InS);

        if (auto *Cast = dyn_cast<ExplicitCast>(Node))
            return callVisitExplicitCastImpl(Cast);

        if (auto *FreeN = dyn_cast<FreeNode>(Node))
            return callVisitFreeNodeImpl(FreeN);

        if (auto *Vec = dyn_cast<VectorLiteral>(Node))
            return callVisitVectorLiteralImpl(Vec);

        if (auto *Gen = dyn_cast<Generator>(Node))
            return callVisitGeneratorImpl(Gen);

        if (auto *Append = dyn_cast<AppendNode>(Node))
            return callVisitAppendNodeImpl(Append);

        assert(false && "Should be unreachable");
    }
};


#endif //GAZPREABASE_VISITORPASS_H
