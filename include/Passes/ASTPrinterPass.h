//
// Created by Chen on 2022-11-05.
//

#ifndef GAZPREABASE_ASTPRINTERPASS_H
#define GAZPREABASE_ASTPRINTERPASS_H

#include "PassManager.h"
#include "VisitorPass.h"
#include "ExprTypeAnnotatorPass.h"

class ASTPrinterPass: public VisitorPass<ASTPrinterPass, void> {
    ASTPassManager *PM;
    SymbolTable *SymTab;

public:
    ASTNodeT *Root;

    void printTree(ASTNodeT *Root);

    void printSubTree(ASTNodeT *Root, const std::string &Prefix);


    void visitProgram(Program *Prog) {
        std::cout << "Program" << "\n";
    }

    void visitIdentifier(Identifier *Ident) {
        std::cout << "Ident(Name: " << Ident->getName() << ')' << "\n";
        //std::cout << ", Type: " << SymTab->getType(SymTab->getSymbolById(Ident->getSymbolId())->getTypeId())->getTag();
    }

    void visitAssignment(Assignment *Assign) {
        std::cout << "Assignment" << "\n";
    }

    void visitDeclaration(Declaration *Decl) {
        std:: cout << "Decl(Name:" << Decl->getIdentifier()->getName() << ')' << "\n";
    }

    void visitBlock(Block *Blk) {
        std::cout << "Block" << "\n";
    }

    void visitLogicalOp(LogicalOp *Op);

    void visitArithmeticOp(ArithmeticOp *Op);

    void visitIndex(Index *Idx) {
        std::cout << "Index" << "\n";
    }

    void visitInfiniteLoop(InfiniteLoop *Loop) {
        std::cout << "InfiniteLoop" << "\n";
    }

    void visitConditionalLoop(ConditionalLoop *Loop) {
        std::cout << "ConditionalLoop" << "\n";
    }

    // ignored for part1
    void visitDomainLoop(DomainLoop *Loop) {
        std::cout << "DomainLoop" << "\n";
    }

    void visitIntLiteral(IntLiteral *IntLit) {
        std::cout << "IntLit(" << IntLit->getVal() << ")" << "\n";
    }

    void visitNullLiteral(NullLiteral *NullLit) {
        std::cout << "NULL" << "\n";
    }

    void visitIdentityLiteral(IdentityLiteral *IdentityLit) {
        std::cout << "Identity" << "\n";
    }

    void visitRealLiteral(RealLiteral *RealLit) {
        std::cout << "RealLit(" << RealLit->getVal() << ")" << "\n";
    }

    void visitBoolLiteral(BoolLiteral *BoolLit) {
        std::cout << "BoolLit(" << BoolLit->getVal() << ")" << "\n";
    }

    void visitCharLiteral(CharLiteral *CharLit) {
        std::cout << "CharLit(" << CharLit->getCharacter() << ")" << "\n";
    }

    void visitTupleLiteral(TupleLiteral *TupleLit) {
        std::cout << "TupleLit" << "\n";
    }

    void visitMemberAccess(MemberAccess *MemberAcc) {
        std::cout << "MemberAccess" << "\n";
    }

    void visitConditional(Conditional *Cond) {
        std::cout << "Conditional" << "\n";
    }

    void visitConditionalElse(ConditionalElse *Cond) {
        std::cout << "ConditionalElse" << "\n";
    }

    void visitTypeCast(TypeCast *Cast) {
        std::cout << "TypeCast" << "\n";
    }

    void visitUnaryOp(UnaryOp *Op);

    void visitArgsList(ArgsList *List) {
        std::cout << "ArgsList" << "\n";
    }

    void visitCalleeParameter(CalleeParameter *CalleeParameter) {
        std::cout << "CalleeParameter" << "\n";
    }

    void visitParameterList(ParameterList *List) {
        std::cout << "ParameterList" << "\n";
    }

    void visitFunctionDecl(FunctionDecl *FuncDecl) {
        std::cout << "FunctionDecl" << "\n";
    }

    void visitFunctionDef(FunctionDef *FuncDef) {
        std::cout << "FunctionDef" << "\n";
    }

    void visitResolvedType(ResolvedType *ResolvedType) {
        std::cout << "ResolvedType" << "\n";
    }

    void visitFunctionCall(FunctionCall *FuncCall) {
        std::cout << "FunctionCall" << "\n";
    }

    void visitProcedureDecl(ProcedureDecl *ProcedureDecl) {
        std::cout << "ProcedureDecl" << "\n";
    }

    void visitProcedureDef(ProcedureDef *ProcedureDef) {
        std::cout << "ProcedureDef" << "\n";
    }

    void visitProcedureCall(ProcedureCall *ProcedureCall) {
        std::cout << "ProcedureCall" << "\n";
    }

    void visitReturn(Return *Return) {
        std::cout << "Return" << "\n";
    }

    void visitBreak(Break *Break) {
        std::cout << "Break" << "\n";
    }

    void visitContinue(Continue *Continue) {
        std::cout << "Continue" << "\n";
    }

    void visitOutStream(OutStream *OutStream) {
        std::cout << "OutStream" << "\n";
    }

    void visitInStream(InStream *InStream) {
        std::cout << "InStream" << "\n";
    }

    void visitExplicitCast(ExplicitCast *ExplicitCast) {
        std::cout << "ExplicitCast" << "\n";
    }


    void runOnAST(ASTPassManager &PManager, ASTNodeT &Root) {
        PM = &PManager;
        //SymTab = &PManager.getResult<SymbolTableDummyPass>();
        this->Root = &Root;
        printTree(this->Root);
    }
};


#endif //GAZPREABASE_ASTPRINTERPASS_H
