//
// Created by Chen on 2022-11-08.
//

#ifndef GAZPREABASE_VISITORPASS_H
#define GAZPREABASE_VISITORPASS_H

#include <cassert>
#include "Pass.h"
#include "AST/ASTNodes.h"

template<typename DerivedT, typename RetT>
class VisitorPass {

    RetT visitProgram(Program *Prog) {
        for (auto *child : *Prog) {
            visit(child);
        }
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
        visit(Decl->getIdentTypeNode());
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
        visit(Loop->getStatement());
        return RetT();
    }

    RetT visitConditionalLoop(ConditionalLoop *Loop) {
        visit(Loop->getConditional());
        visit(Loop->getStatement());
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

    RetT visitTupleTypeDecl(TupleTypeDecl *TupleTypeDecl) {
        for (auto *child : *TupleTypeDecl) {
            visit(child);
        }
        return RetT();
    }

    RetT visitConditional(Conditional *Cond) {
        visit(Cond->getConditional());
        visit(Cond->getStatement());
        return RetT();
    }

    RetT visitConditionalElse(ConditionalElse *Cond) {
        visit(Cond->getConditional());
        visit(Cond->getStatement());
        visit(Cond->getElseConditional());
        visit(Cond->getElseStatement());
        return RetT();
    }

    RetT visitTypeCast(TypeCast *Cast) {
        visit(Cast->getOldTypeNode());
        visit(Cast->getNewTypeNode());
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

    RetT visitParasList(ParasList *List) {
        for (auto *child : *List) {
            visit(child);
        }
        return RetT();
    }

    RetT visitFunctionDecl(FunctionDecl *FuncDecl) {
        visit(FuncDecl->getIdentifier());
        visit(FuncDecl->getParasList());
        visit(FuncDecl->getReturnsType());
        return RetT();
    }

    RetT visitFunctionDef(FunctionDef *FuncDef) {
        visit(FuncDef->getIdentifier());
        visit(FuncDef->getParasList());
        visit(FuncDef->getReturnsType());
        if (dynamic_cast<Block*>(FuncDef->getBlock()))
           visit(FuncDef->getBlock());
        else
            visit(FuncDef->getExpr());
        return RetT();
    }

    RetT visitFunctionCall(FunctionCall *FuncCall) {
        visit(FuncCall->getIdentifier());
        visit(FuncCall->getArgsList());
        return RetT();
    }

    RetT visitProcedureDecl(ProcedureDecl *ProcedureDecl) {
        visit(ProcedureDecl->getIdentifier());
        visit(ProcedureDecl->getParasList());
        visit(ProcedureDecl->getReturnsType());
        return RetT();
    }

    RetT visitProcedureDef(ProcedureDef *ProcedureDef) {
        visit(ProcedureDef->getIdentifier());
        visit(ProcedureDef->getParasList());
        visit(ProcedureDef->getReturnsType());
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
        visit(ExplicitCast->getType());
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

    RetT callVisitLogicalOpImpl(ASTNodeT *LogicOp) {
        return static_cast<DerivedT*>(this)->visitLogicalOp(LogicOp);
    }

    RetT callVisitArithmeticOpImpl(ASTNodeT *ArithOp) {
        return static_cast<DerivedT*>(this)->visitArithmeticOp(ArithOp);
    }

    RetT callVisitIndexImpl(ASTNodeT *Ind) {
        return static_cast<DerivedT*>(this)->visitIndex(Ind);
    }

    RetT callVisitInfiniteLoopImpl(ASTNodeT *Loop) {
        return static_cast<DerivedT*>(this)->visitInfiniteLoop(Loop);
    }

    RetT callVisitConditionalLoopImpl(ASTNodeT *CondLoop) {
        return static_cast<DerivedT*>(this)->visitConditionalLoop(CondLoop);
    }

    RetT callVisitDomainLoopImpl(ASTNodeT *DomainLoop) {
        return static_cast<DerivedT*>(this)->visitDomainLoop(DomainLoop);
    }

    RetT callVisitIntLiteralImpl(ASTNodeT *IntLiteral) {
        return static_cast<DerivedT*>(this)->visitIntLiteral(IntLiteral);
    }

    RetT callVisitNullLiteralImpl(ASTNodeT *NullLiteral) {
        return static_cast<DerivedT*>(this)->visitNullLiteral(NullLiteral);
    }

    RetT callVisitIdentityLiteralImpl(ASTNodeT *Identity) {
        return static_cast<DerivedT*>(this)->visitIdentityLiteral(Identity);
    }

    RetT callVisitRealLiteralImpl(ASTNodeT *RealLiteral) {
        return static_cast<DerivedT*>(this)->visitRealLiteral(RealLiteral);
    }

    RetT callVisitBoolLiteralImpl(ASTNodeT *BoolLiteral) {
        return static_cast<DerivedT*>(this)->visitBoolLiteral(BoolLiteral);
    }

    RetT callVisitCharLiteralImpl(ASTNodeT *CharLiteral) {
        return static_cast<DerivedT*>(this)->visitCharLiteral(CharLiteral);
    }

    RetT callVisitTupleLiteralImpl(ASTNodeT *TupleLiteral) {
        return static_cast<DerivedT*>(this)->visitTupleLiteral(TupleLiteral);
    }

    RetT callVisitMemberAccessImpl(ASTNodeT *MemberAccess) {
        return static_cast<DerivedT*>(this)->visitMemberAccess(MemberAccess);
    }

    RetT callVisitTupleTypeDeclImpl(ASTNodeT *TupleTypeDecl) {
        return static_cast<DerivedT*>(this)->visitTupleTypeDecl(TupleTypeDecl);
    }

    RetT callVisitConditionalImpl(ASTNodeT *Cond) {
        return static_cast<DerivedT*>(this)->visitConditional(Cond);
    }

    RetT callVisitConditionalElseImpl(ASTNodeT *CondElse) {
        return static_cast<DerivedT*>(this)->visitConditionalElse(CondElse);
    }

    RetT callVisitTypeCastImpl(ASTNodeT *TypeCast) {
        return static_cast<DerivedT*>(this)->visitTypeCast(TypeCast);
    }

    RetT callVisitBitwiseOpImpl(ASTNodeT *BitwiseOp) {
        return static_cast<DerivedT*>(this)->visitBitwiseOp(BitwiseOp);
    }

    RetT callVisitUnaryOpImpl(ASTNodeT *UnaryOp) {
        return static_cast<DerivedT*>(this)->visitUnaryOp(UnaryOp);
    }

    RetT callVisitArgsListImpl(ASTNodeT *ArgsList) {
        return static_cast<DerivedT*>(this)->visitArgsList(ArgsList);
    }

    RetT callVisitParasListImpl(ASTNodeT *ParasList) {
        return static_cast<DerivedT*>(this)->visitParasList(ParasList);
    }

    RetT callVisitFunctionDeclImpl(ASTNodeT *FuncDecl) {
        return static_cast<DerivedT*>(this)->visitFunctionDecl(FuncDecl);
    }

    RetT callVisitFunctionDefImpl(ASTNodeT *FuncDef) {
        return static_cast<DerivedT*>(this)->visitFunctionDef(FuncDef);
    }

    RetT callVisitFunctionCallImpl(ASTNodeT *FuncCall) {
        return static_cast<DerivedT*>(this)->visitFunctionCall(FuncCall);
    }

    RetT callVisitProcedureDeclImpl(ASTNodeT *ProcedureDecl) {
        return static_cast<DerivedT*>(this)->visitProcedureDecl(ProcedureDecl);
    }

    RetT callVisitProcedureDefImpl(ASTNodeT *ProcedureDef) {
        return static_cast<DerivedT*>(this)->visitProcedureDef(ProcedureDef);
    }

    RetT callVisitProcedureCallImpl(ASTNodeT *ProcedureCall) {
        return static_cast<DerivedT*>(this)->visitProcedureCall(ProcedureCall);
    }

    RetT callVisitReturnImpl(ASTNodeT *Return) {
        return static_cast<DerivedT*>(this)->visitReturn(Return);
    }

    RetT callVisitBreakImpl(ASTNodeT *Break) {
        return static_cast<DerivedT*>(this)->visitBreak(Break);
    }

    RetT callVisitContinueImpl(ASTNodeT *Continue) {
        return static_cast<DerivedT*>(this)->visitContinue(Continue);
    }

    RetT callVisitOutStreamImpl(ASTNodeT *OutStream) {
        return static_cast<DerivedT*>(this)->visitOutStream(OutStream);
    }

    RetT callVisitInStreamImpl(ASTNodeT *InStream) {
        return static_cast<DerivedT*>(this)->visitInStream(InStream);
    }

    RetT callVisitExplicitCastImpl(ASTNodeT *ExpliCast) {
        return static_cast<DerivedT*>(this)->visitExplicitCast(ExpliCast);
    }

public:
    RetT visit(ASTNodeT *Node) {
        if (auto *Prog = dynamic_cast<Program*>(Node)) {
            return callVisitProgramImpl(Prog);
        }

        if (auto *Ident = dynamic_cast<Identifier*>(Node)) {
            return callVisitIdentifierImpl(Ident);
        }

        if (auto *Assign = dynamic_cast<Assignment*>(Node)) {
            return callVisitAssignmentImpl(Assign);
        }

        if (auto *Decl = dynamic_cast<Declaration*>(Node)) {
            return callVisitDeclarationImpl(Decl);
        }

        if (auto *Blk = dynamic_cast<Block*>(Node)) {
            return callVisitBlockImpl(Blk);
        }

        if (auto *LogicOp = dynamic_cast<LogicalOp*>(Node)) {
            return callVisitLogicalOpImpl(LogicOp);
        }

        if (auto *ArithOp = dynamic_cast<ArithmeticOp*>(Node)) {
            return callVisitArithmeticOpImpl(ArithOp);
        }

        if (auto *Idx = dynamic_cast<Index*>(Node)) {
            return callVisitIndexImpl(Idx);
        }

        if (auto *InfLoop = dynamic_cast<InfiniteLoop*>(Node)) {
            return callVisitInfiniteLoopImpl(InfLoop);
        }

        if (auto *CondLoop = dynamic_cast<ConditionalLoop*>(Node)) {
            return callVisitConditionalLoopImpl(CondLoop);
        }

        if (auto *DomLoop = dynamic_cast<DomainLoop*>(Node)) {
            return callVisitDomainLoopImpl(DomLoop);
        }

        if (auto *IntLit = dynamic_cast<IntLiteral*>(Node)) {
            return callVisitIntLiteralImpl(IntLit);
        }

        if (auto *NullLit = dynamic_cast<NullLiteral*>(Node)) {
            return callVisitNullLiteralImpl(NullLit);
        }

        if (auto *IdentLit = dynamic_cast<IdentityLiteral*>(Node)) {
            return callVisitIdentityLiteralImpl(IdentLit);
        }

        if (auto *RealLit = dynamic_cast<RealLiteral*>(Node)) {
            return callVisitRealLiteralImpl(RealLit);
        }

        if (auto *BoolLit = dynamic_cast<BoolLiteral*>(Node)) {
            return callVisitBoolLiteralImpl(BoolLit);
        }

        if (auto *CharLit = dynamic_cast<CharLiteral*>(Node)) {
            return callVisitCharLiteralImpl(CharLit);
        }

        if (auto *TupleLit = dynamic_cast<TupleLiteral*>(Node)) {
            return callVisitTupleLiteralImpl(TupleLit);
        }

        if (auto *MemberAcc = dynamic_cast<MemberAccess*>(Node)) {
            return callVisitMemberAccessImpl(MemberAcc);
        }

        if (auto *TupleTypeDec = dynamic_cast<TupleTypeDecl*>(Node)) {
            return callVisitTupleTypeDeclImpl(TupleTypeDec);
        }

        if (auto *Cond = dynamic_cast<Conditional*>(Node)) {
            return callVisitConditionalImpl(Cond);
        }

        if (auto *CondElse = dynamic_cast<ConditionalElse*>(Node)) {
            return callVisitConditionalElseImpl(CondElse);
        }

        if (auto *TypeC = dynamic_cast<TypeCast*>(Node)) {
            return callVisitTypeCastImpl(TypeC);
        }

        if (auto *BitwsOp = dynamic_cast<BitwiseOp*>(Node)) {
            return callVisitBitwiseOpImpl(BitwsOp);
        }

        if (auto *UnaOp = dynamic_cast<UnaryOp*>(Node)) {
            return callVisitUnaryOpImpl(UnaOp);
        }

        if (auto *ArgsLi = dynamic_cast<ArgsList*>(Node)) {
            return callVisitArgsListImpl(ArgsLi);
        }

        if (auto *ParaLi = dynamic_cast<ParasList*>(Node)) {
            return callVisitParasListImpl(ParaLi);
        }

        if (auto *FunDec = dynamic_cast<FunctionDecl*>(Node)) {
            return callVisitFunctionDeclImpl(FunDec);
        }

        if (auto *FuncDef = dynamic_cast<FunctionDef*>(Node)) {
            return callVisitFunctionDefImpl(FuncDef);
        }

        if (auto *FuncCall = dynamic_cast<FunctionCall*>(Node)) {
            return callVisitFunctionCallImpl(FuncCall);
        }

        if (auto *ProcedDecl = dynamic_cast<ProcedureDecl*>(Node)) {
            return callVisitProcedureDeclImpl(ProcedDecl);
        }

        if (auto *ProcedDef = dynamic_cast<ProcedureDef*>(Node)) {
            return callVisitProcedureDefImpl(ProcedDef);
        }

        if (auto *ProcedCall = dynamic_cast<ProcedureCall*>(Node)) {
            return callVisitProcedureCallImpl(ProcedCall);
        }

        if (auto *Ret = dynamic_cast<Return*>(Node)) {
            return callVisitReturnImpl(Ret);
        }

        if (auto *Brk = dynamic_cast<Break*>(Node)) {
            return callVisitBreakImpl(Brk);
        }

        if (auto *Cont = dynamic_cast<Continue*>(Node)) {
            return callVisitContinueImpl(Cont);
        }

        if (auto *OutS = dynamic_cast<OutStream*>(Node)) {
            return callVisitOutStreamImpl(OutS);
        }

        if (auto *InS = dynamic_cast<InStream*>(Node)) {
            return callVisitInStreamImpl(InS);
        }

        if (auto *ExpliCast = dynamic_cast<ExplicitCast*>(Node)) {
            return callVisitExplicitCastImpl(ExpliCast);
        }
    }
};


#endif //GAZPREABASE_VISITORPASS_H
