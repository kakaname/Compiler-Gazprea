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

    RetT visitIdentifier(Identifier *Ident) {
        return RetT();
    }

    RetT visitAssignment(Assignment *Assign) {
        visit(Assign->getIdentifier());
        visit(Assign->getExpr());
        return RetT();
    }

    RetT visitDeclaration(Declaration *Decl) {
        visit(Decl->getIdentifier());
        visit(Decl->getInitExpr());
        return RetT();
    }

    RetT visitBlock(Block *Blk) {
        for (auto *child : *Blk) {
            visit(child);
        }
        return RetT();
    }

    RetT visitLogicalOp(LogicalOp *Op) {
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
        return RetT();
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
        for (auto *child : *TupleLit) {
            visit(child);
        }
        return RetT();
    }

    RetT visitMemberAccess(MemberAccess *MemberAcc) {
        visit(MemberAcc->getIdentifier());
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
//        visit(Cond->getElseConditional());
        visit(Cond->getElseBlock());
        return RetT();
    }

    RetT visitTypeCast(TypeCast *Cast) {
        visit(Cast->getExpr());
        return RetT();
    }
    
    RetT visitBitwiseOp(BitwiseOp *Op) {
        visit(Op->getLeftExpr());
        visit(Op->getRightExpr());
        return RetT();
    }

    RetT visitUnaryOp(UnaryOp *Op) {
        visit(Op->getExpr());
        return RetT();
    }

    RetT visitArgsList(ArgsList *List) {
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
        visit(FuncDef->getBlock());
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
        visit(ProcedureDef->getBlock());
        return RetT();
    }

    RetT visitProcedureCall(ProcedureCall *ProcedureCall) {
        visit(ProcedureCall->getIdentifier());
        visit(ProcedureCall->getArgsList());
        return RetT();
    }

    RetT visitReturn(Return *Return) {
        visit(Return->getReturnExpr());
        return RetT();
    }

    RetT visitBreak(Break *Break) {
        return RetT();
    }

    RetT visitContinue(Continue *Continue) {
        return RetT();
    }

    RetT visitOutStream(OutStream *OutStream) {
        visit(OutStream->getOutStreamExpr());
        return RetT();
    }

    RetT visitInStream(InStream *InStream) {
        visit(InStream->getIdentifier());
        return RetT();
    }

    RetT visitExplicitCast(ExplicitCast *ExplicitCast) {
        visit(ExplicitCast->getExpr());
        return RetT();
    }


    RetT callVisitProgramImpl(Program *Prog) {
        return static_cast<DerivedT*>(this)->visitProgram(Prog);
    }

    RetT callVisitIdentifierImpl(Identifier *Ident) {
        return static_cast<DerivedT*>(this)->visitIdentifier(Ident);
    }

    RetT callVisitAssignmentImpl(Assignment *Assign) {
        return static_cast<DerivedT*>(this)->visitAssignment(Assign);
    }

    RetT callVisitDeclarationImpl(Declaration *Decl) {
        return static_cast<DerivedT*>(this)->visitDeclaration(Decl);
    }

    RetT callVisitBlockImpl(Block *Block) {
        return static_cast<DerivedT*>(this)->visitBlock(Block);
    }

    RetT callVisitLogicalOpImpl(LogicalOp *LogicOp) {
        return static_cast<DerivedT*>(this)->visitLogicalOp(LogicOp);
    }

    RetT callVisitArithmeticOpImpl(ArithmeticOp *ArithOp) {
        return static_cast<DerivedT*>(this)->visitArithmeticOp(ArithOp);
    }

    RetT callVisitIndexImpl(Index *Idx) {
        return static_cast<DerivedT*>(this)->visitIndex(Idx);
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

    RetT callVisitBitwiseOpImpl(BitwiseOp *O) {
        return static_cast<DerivedT*>(this)->visitBitwiseOp(O);
    }

    RetT callVisitUnaryOpImpl(UnaryOp *O) {
        return static_cast<DerivedT*>(this)->visitUnaryOp(O);
    }

    RetT callVisitArgsListImpl(ArgsList *A) {
        return static_cast<DerivedT*>(this)->visitArgsList(A);
    }

    RetT callVisitFunctionDeclImpl(FunctionDecl *F) {
        return static_cast<DerivedT*>(this)->visitFunctionDecl(F);
    }

    RetT callVisitFunctionDefImpl(FunctionDef *F) {
        return static_cast<DerivedT*>(this)->visitFunctionDef(F);
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

public:
    RetT visit(ASTNodeT *Node) {
        if (auto *Prog = dyn_cast<Program>(Node))
            return callVisitProgramImpl(Prog);

        if (auto *Ident = dyn_cast<Identifier>(Node))
            return callVisitIdentifierImpl(Ident);

        if (auto *Assign = dyn_cast<Assignment>(Node))
            return callVisitAssignmentImpl(Assign);

        if (auto *Decl = dyn_cast<Declaration>(Node))
            return callVisitDeclarationImpl(Decl);

        if (auto *Blk = dyn_cast<Block>(Node))
            return callVisitBlockImpl(Blk);

        if (auto *LogicOp = dyn_cast<LogicalOp>(Node))
            return callVisitLogicalOpImpl(LogicOp);

        if (auto *ArithOp = dyn_cast<ArithmeticOp>(Node))
            return callVisitArithmeticOpImpl(ArithOp);

        if (auto *Idx = dyn_cast<Index>(Node))
            return callVisitIndexImpl(Idx);

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

        if (auto *BitOp = dyn_cast<BitwiseOp>(Node))
            return callVisitBitwiseOpImpl(BitOp);

        if (auto *UOp = dyn_cast<UnaryOp>(Node))
            return callVisitUnaryOpImpl(UOp);

        if (auto *ArgsLi = dyn_cast<ArgsList>(Node))
            return callVisitArgsListImpl(ArgsLi);

        if (auto *FunDec = dyn_cast<FunctionDecl>(Node))
            return callVisitFunctionDeclImpl(FunDec);

        if (auto *FuncDef = dyn_cast<FunctionDef>(Node))
            return callVisitFunctionDefImpl(FuncDef);

        if (auto *FuncCall = dyn_cast<FunctionCall>(Node))
            return callVisitFunctionCallImpl(FuncCall);

        if (auto *ProcedDecl = dyn_cast<ProcedureDecl>(Node))
            return callVisitProcedureDeclImpl(ProcedDecl);

        if (auto *ProcedDef = dyn_cast<ProcedureDef>(Node))
            return callVisitProcedureDefImpl(ProcedDef);

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

        assert(false && "Should be unreachable");
    }
};


#endif //GAZPREABASE_VISITORPASS_H
