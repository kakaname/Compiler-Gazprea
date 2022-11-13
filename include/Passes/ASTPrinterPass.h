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

        std::cout << "Ident(Name: " << Ident->getName() << "\n";
    }

    void visitAssignment(Assignment *Assign) {
        std::cout << "Assignment" << "\n";
    }

    void visitDeclaration(Declaration *Decl) {
        std::cout << "Declaration" << "\n";
    }

    void visitBlock(Block *Blk) {
        std::cout << "Block" << "\n";
    }

    void visitLogicalOp(LogicalOp *Op) {
        std::cout << "LogicalOp" << "\n";
    }

    void visitArithmeticOp(ArithmeticOp *Op) {
        std::cout << "ArithmeticOp" << "\n";
    }

    void visitIndex(Index *Idx) {

    }

    void visitInfiniteLoop(InfiniteLoop *Loop) {

    }

    void visitConditionalLoop(ConditionalLoop *Loop) {

    }

    // ignored for part1
    void visitDomainLoop(DomainLoop *Loop) {

    }

    void visitIntLiteral(IntLiteral *IntLit) {

    }

    void visitNullLiteral(NullLiteral *NullLit) {

    }

    void visitIdentityLiteral(IdentityLiteral *IdentityLit) {

    }

    void visitRealLiteral(RealLiteral *RealLit) {

    }

    void visitBoolLiteral(BoolLiteral *BoolLit) {

    }

    void visitCharLiteral(CharLiteral *CharLit) {

    }

    void visitTupleLiteral(TupleLiteral *TupleLit) {

    }

    void visitMemberAccess(MemberAccess *MemberAcc) {

    }

    void visitTupleTypeDecl(TupleTypeDecl *TupleTypeDecl) {

    }

    void visitIntegerTypeNode(IntegerTypeNode *IntegerTypeNode) {

    }

    void visitCharTypeNode(CharTypeNode *CharTypeNode) {

    }

    void visitBoolTypeNode(BoolTypeNode *BoolTypeNode) {

    }

    void visitRealTypeNode(RealTypeNode *RealTypeNode) {

    }

    void visitConditional(Conditional *Cond) {

    }

    void visitConditionalElse(ConditionalElse *Cond) {

    }

    void visitTypeCast(TypeCast *Cast) {

    }

    void visitTypeDef(TypeDef *TypeDef) {

    }

    void visitBitwiseOp(BitwiseOp *Op) {

    }

    void visitUnaryOp(UnaryOp *Op) {

    }

    void visitArgsList(ArgsList *List) {

    }

    void visitCalleeParameter(CalleeParameter *CalleeParameter) {

    }

    void visitParameterList(ParameterList *List) {

    }

    void visitFunctionDecl(FunctionDecl *FuncDecl) {

    }

    void visitFunctionDef(FunctionDef *FuncDef) {

    }

    void visitResolvedType(ResolvedType *ResolvedType) {

    }

    void visitFunctionCall(FunctionCall *FuncCall) {

    }

    void visitProcedureDecl(ProcedureDecl *ProcedureDecl) {

    }

    void visitProcedureDef(ProcedureDef *ProcedureDef) {

    }

    void visitProcedureCall(ProcedureCall *ProcedureCall) {

    }

    void visitReturn(Return *Return) {

    }

    void visitBreak(Break *Break) {

    }

    void visitContinue(Continue *Continue) {

    }

    void visitOutStream(OutStream *OutStream) {

    }

    void visitInStream(InStream *InStream) {

    }

    void visitExplicitCast(ExplicitCast *ExplicitCast) {

    }


    void runOnAST(ASTPassManager &PManager, ASTNodeT &Root) {
        PM = &PManager;
        //SymTab = &PManager.getResult<SymbolTableDummyPass>();
        this->Root = &Root;
        printTree(this->Root);
    }
};


#endif //GAZPREABASE_ASTPRINTERPASS_H
