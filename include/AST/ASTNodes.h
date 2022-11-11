//
// Created by dhanrajbir on 28/10/22.
//

#ifndef GAZPREABASE_ASTNODES_H
#define GAZPREABASE_ASTNODES_H

#include "string"

#include "Common/TreeNode.h"
#include "Types/Type.h"

using std::string;

using ASTNodeT = TreeNode;

struct Program: public TreeNode {

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Program;
    }

    Program() : TreeNode(TreeNodeKind::N_AST_Program) {}
};


struct Identifier: public TreeNode {
    string IdentName;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Identifier;
    }

    Identifier() : TreeNode(TreeNodeKind::N_AST_Identifier) {}

    void setName(const string &N) {
        IdentName = N;
    }

    const string &getName() {
        return IdentName;
    }
};


struct Assignment: public TreeNode {

    static constexpr size_t IdentChildIdx = 0;
    static constexpr size_t ExprChildIdx = 1;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Assignment;
    }

    void setIdentifier(Identifier *Ident) {
        setChildAt(IdentChildIdx, Ident);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentChildIdx);
    }

    void setExpr(ASTNodeT *Expr) {
        setChildAt(ExprChildIdx, Expr);
    }

    ASTNodeT *getExpr() {
        return getChildAt(ExprChildIdx);
    }

    Assignment() : TreeNode(TreeNodeKind::N_AST_Assignment) {};
};


struct Declaration: public TreeNode {
    static constexpr size_t IdentTypeIdx = 0;
    static constexpr size_t IdentIdx = 1;
    static constexpr size_t InitExprIdx = 2;

    bool IsConst{false};

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Declaration;
    }

    void setIdentTypeNode(ASTNodeT *N) {
        setChildAt(IdentTypeIdx, N);
    }

    void setIdent(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    void setInitExpr(ASTNodeT *Expr) {
        setChildAt(InitExprIdx, Expr);
    }

    ASTNodeT *getIdentTypeNode() {
        return getChildAt(IdentTypeIdx);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    ASTNodeT *getInitExpr() {
        return getChildAt(InitExprIdx);
    }

    void setConst() {
        IsConst = true;
    }

    Declaration() : TreeNode(TreeNodeKind::N_AST_Declaration) {}
};


struct Block: public TreeNode {
    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Block;
    }

    Block() : TreeNode(TreeNodeKind::N_AST_Block) {}
};


struct LogicalOp: public TreeNode {
    static constexpr int LeftExprIdx = 0;
    static constexpr int RightExprIdx = 1;

    enum OpKind {
        LT = 0, GT, LTEQ, GTEQ, EQEQ, NEQ
    };

    OpKind Op;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_LogicalOp;
    }

    void setOp(OpKind OpKind) {
        Op = OpKind;
    }

    OpKind getOpKind() {
        return Op;
    }

    void setLeftExpr(ASTNodeT *Expr) {
        setChildAt(LeftExprIdx, Expr);
    }

    void setRightExpr(ASTNodeT *Expr) {
        setChildAt(RightExprIdx, Expr);
    }

    ASTNodeT *getLeftExpr() {
        return getChildAt(LeftExprIdx);
    }

    ASTNodeT *getRightExpr() {
        return getChildAt(RightExprIdx);
    }

    LogicalOp() : TreeNode(TreeNodeKind::N_AST_LogicalOp) {}
};


struct ArithmeticOp: public TreeNode {
    static constexpr int LeftExprIdx = 0;
    static constexpr int RightExprIdx = 1;

    enum OpKind {
        EXP = 0, MUL, DIV, MOD, SS, ADD, SUB
    };

    OpKind Op;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ArithmeticOp;
    }

    void setOp(OpKind OpKind) {
        Op = OpKind;
    }

    OpKind getOpKind() {
        return Op;
    }

    void setLeftExpr(ASTNodeT *Expr) {
        setChildAt(LeftExprIdx, Expr);
    }

    void setRightExpr(ASTNodeT *Expr) {
        setChildAt(RightExprIdx, Expr);
    }

    ASTNodeT *getLeftExpr() {
        return getChildAt(LeftExprIdx);
    }

    ASTNodeT *getRightExpr() {
        return getChildAt(RightExprIdx);
    }

    ArithmeticOp() : TreeNode(TreeNodeKind::N_AST_ArithmeticOp) {}
};


struct Index: public TreeNode {
    static constexpr int BaseExprIdx = 0;
    static constexpr int IndexExprIdx = 1;

    void setBaseExpr(ASTNodeT *Expr) {
        setChildAt(BaseExprIdx, Expr);
    }

    void setIndexExpr(ASTNodeT *Expr) {
        setChildAt(IndexExprIdx, Expr);
    }

    ASTNodeT *getBaseExpr() {
        return getChildAt(BaseExprIdx);
    }

    ASTNodeT *getIndexExpr() {
        return getChildAt(IndexExprIdx);
    }

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Index;
    }

    Index() : TreeNode(TreeNodeKind::N_AST_Index) {};
};


struct InfiniteLoop: public TreeNode {
    static constexpr int StatementIdx = 0;

    void setStatement(ASTNodeT *Stat) {
        setChildAt(StatementIdx, Stat);
    }

    ASTNodeT *getStatement() {
        return getChildAt(StatementIdx);
    }

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_InfiniteLoop;
    }

    InfiniteLoop() : TreeNode(TreeNodeKind::N_AST_InfiniteLoop) {};
};

// Remains to be checked
struct ConditionalLoop: public TreeNode {
    static constexpr int ConditionalIdx = 0;
    static constexpr int StatementIdx = 1;

    // Check if while loop or do-while loop
    bool ConditionalBefore{true};

    void setConditionalAfter() {
        ConditionalBefore = false;
    }

    void setConditional(ASTNodeT *Expr) {
        setChildAt(ConditionalIdx, Expr);
    }

    void setStatement(ASTNodeT *Stat) {
        setChildAt(StatementIdx, Stat);
    }

    ASTNodeT *getConditional() {
        return getChildAt(ConditionalIdx);
    }

    ASTNodeT *getStatement() {
        return getChildAt(StatementIdx);
    }

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ConditionalLoop;
    }

    ConditionalLoop() : TreeNode(TreeNodeKind::N_AST_ConditionalLoop) {}
};

// ignored for part1
struct DomainLoop: public TreeNode {
    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_DomainLoop;
    }

    DomainLoop(): TreeNode(TreeNodeKind::N_AST_DomainLoop) {}
};


struct IntLiteral: public TreeNode {
    int32_t Val;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_IntLiteral;
    }

    void setVal(const std::basic_string<char> &Literal) {
        Val = std::stoi(Literal);
    }

    int32_t getVal() {
        return Val;
    }

    IntLiteral(): TreeNode(TreeNodeKind::N_AST_IntLiteral) {}
};


struct NullLiteral: public TreeNode {
    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_NullLiteral;
    }

    NullLiteral(): TreeNode(TreeNodeKind::N_AST_NullLiteral) {}
};


struct IdentityLiteral: public TreeNode {
    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_IdentityLiteral;
    }

    IdentityLiteral(): TreeNode(TreeNodeKind::N_AST_IdentityLiteral) {}
};


// Remains to be checked!!!!!
struct RealLiteral: public TreeNode {
    float Val;

    void setVal(string &FloatString) {
        this->Val = std::stof(FloatString);
    }

    float getVal() {
        return Val;
    }

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_RealLiteral;
    }

    RealLiteral(): TreeNode(TreeNodeKind::N_AST_RealLiteral) {}
};


struct BoolLiteral: public TreeNode {
    bool Val{false};

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_BoolLiteral;
    }

    void setTrue() {
        Val = true;
    }

    void setFalse() {
        Val = false;
    }

    bool getVal() {
        return Val;
    }

    BoolLiteral(): TreeNode(TreeNodeKind::N_AST_BoolLiteral) {}
};


struct CharLiteral: public TreeNode {
    char Character;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_CharLiteral;
    }

    void setCharacter(string Literal) {
        Character = Literal[0];
    }

    char getCharacter() {
        return Character;
    }

    CharLiteral(): TreeNode(TreeNodeKind::N_AST_CharLiteral) {}
};


struct TupleLiteral: public TreeNode {
    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_TupleLiteral;
    }

    void setExprAtPos(ASTNodeT *Expr, long Pos) {
        setChildAt(Pos, Expr);
    }

    ASTNodeT *getExprAtPos(long Pos) {
        return getChildAt(Pos);
    }

    TupleLiteral(): TreeNode(TreeNodeKind::N_AST_TupleLiteral) {};
};


struct MemberAccess: public TreeNode {
    static constexpr size_t IdentIdx = 0;
    static constexpr size_t MemberExprIdx = 1;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_MemberAccess;
    }

    void setIdent(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    void setMemberExpr(ASTNodeT *Expr) {
        setChildAt(MemberExprIdx, Expr);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    ASTNodeT *getMemberExpr() {
        return getChildAt(MemberExprIdx);
    }

    MemberAccess(): TreeNode(TreeNodeKind::N_AST_MemberAccess) {}
};

// Remains to be checked!!!!!
struct TupleTypeDecl: public TreeNode {
    void setDeclAtPos(Declaration *Decl, long Pos) {
        setChildAt(Pos, Decl);
    }

    Declaration *getDeclAtPos(long Pos) {
        return getChildAtAs<Declaration>(Pos);
    }

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_TupleTypeDecl;
    }

    TupleTypeDecl(): TreeNode(TreeNodeKind::N_AST_TupleTypeDecl) {}
};

// Remains to be checked!!!!!
struct Conditional: public TreeNode {
    static constexpr int ConditionalIdx = 0;
    static constexpr int StatementIdx = 1;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Conditional;
    }

    void setConditional(ASTNodeT *Expr) {
        setChildAt(ConditionalIdx, Expr);
    }

    void setStatement(ASTNodeT *Stat) {
        setChildAt(StatementIdx, Stat);
    }

    ASTNodeT *getConditional() {
        return getChildAt(ConditionalIdx);
    }

    ASTNodeT *getStatement() {
        return getChildAt(StatementIdx);
    }

    Conditional(): TreeNode(TreeNodeKind::N_AST_Conditional) {}
};

// Remains to be checked!!!!!
struct ConditionalElse: public TreeNode {
    static constexpr int ConditionalIdx = 0;
    static constexpr int StatementIdx = 1;
    static constexpr int ElseConditionalIdx = 2;
    static constexpr int ElseStatementIdx = 3;


    void setConditional(ASTNodeT *Expr) {
        setChildAt(ConditionalIdx, Expr);
    }

    void setStatement(ASTNodeT *Stat) {
        setChildAt(StatementIdx, Stat);
    }

    void setElseConditional(ASTNodeT *Expr) {
        setChildAt(ElseConditionalIdx, Expr);
    }

    void setElseStatement(ASTNodeT *Stat) {
        setChildAt(ElseStatementIdx, Stat);
    }

    ASTNodeT *getConditional() {
        return getChildAt(ConditionalIdx);
    }

    ASTNodeT *getStatement() {
        return getChildAt(StatementIdx);
    }

    ASTNodeT *getElseConditional() {
        return getChildAt(ElseConditionalIdx);
    }

    ASTNodeT *getElseStatement() {
        return getChildAt(ElseStatementIdx);
    }

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ConditionalElse;
    }

    ConditionalElse(): TreeNode(TreeNodeKind::N_AST_ConditionalElse) {}
};


struct TypeCast: public TreeNode {
    static constexpr int OldTypeIdx = 0;
    static constexpr int NewTypeIdx = 1;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_TypeCast;
    }

    void setOldTypeNode(ASTNodeT *N) {
        setChildAt(OldTypeIdx, N);
    }

    void setNewTypeNode(ASTNodeT *N) {
        setChildAt(NewTypeIdx, N);
    }

    ASTNodeT *getOldTypeNode() {
        return getChildAt(OldTypeIdx);
    }

    ASTNodeT *getNewTypeNode() {
        return getChildAt(NewTypeIdx);
    }

    TypeCast(): TreeNode(TreeNodeKind::N_AST_TypeCast) {}
};


struct BitwiseOp: public TreeNode {
    static constexpr int LeftExprIdx = 0;
    static constexpr int RightExprIdx = 1;

    enum OpKind {
        AND = 0, OR, XOR
    };

    OpKind Op;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_BitwiseOp;
    }

    void setOp(OpKind OpKind) {
        Op = OpKind;
    }

    OpKind getOpKind() {
        return Op;
    }

    void setLeftExpr(ASTNodeT *Expr) {
        setChildAt(LeftExprIdx, Expr);
    }

    void setRightExpr(ASTNodeT *Expr) {
        setChildAt(RightExprIdx, Expr);
    }

    ASTNodeT *getLeftExpr() {
        return getChildAt(LeftExprIdx);
    }

    ASTNodeT *getRightExpr() {
        return getChildAt(RightExprIdx);
    }

    BitwiseOp(): TreeNode(TreeNodeKind::N_AST_BitwiseOp) {}
};


struct UnaryOp: public TreeNode {
    static constexpr int ExprIdx = 0;

    enum OpKind {
        ADD = 0, SUB, NOT
    };

    OpKind Op;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_UnaryOp;
    }

    void setOp(OpKind OpKind) {
        Op = OpKind;
    }

    OpKind getOpKind() {
        return Op;
    }

    void setExpr(ASTNodeT *Expr) {
        setChildAt(ExprIdx, Expr);
    }

    ASTNodeT *getExpr() {
        return getChildAt(ExprIdx);
    }

    UnaryOp(): TreeNode(TreeNodeKind::N_AST_UnaryOp) {}
};



struct ArgsList: public TreeNode {
    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ArgsList;
    }

    void setExprAtPos(ASTNodeT *Expr, long Pos) {
        setChildAt(Pos, Expr);
    }

    ASTNodeT *getExprAtPos(long Pos) {
        return getChildAt(Pos);
    }

    ArgsList(): TreeNode(TreeNodeKind::N_AST_ArgsList) {}
};


struct ParasList: public TreeNode {
    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ParasList;
    }

    void setDeclAtPos(Declaration *Decl, long Pos) {
        setChildAt(Pos, Decl);
    }

    Declaration *getDeclAtPos(long Pos) {
        return getChildAtAs<Declaration>(Pos);
    }

    ParasList(): TreeNode(TreeNodeKind::N_AST_ParasList) {}
};


struct FunctionDecl: public TreeNode {
    static constexpr size_t IdentIdx = 0;
    static constexpr size_t ParasListIdx = 1;
    static constexpr size_t ReturnsTypeIdx = 2;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_FunctionDecl;
    }

    void setIdent(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    void setParasList(ParasList *ParasList) {
        setChildAt(ParasListIdx, ParasList);
    }

    void setReturnsType(ASTNodeT *N) {
        setChildAt(ReturnsTypeIdx, N);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    ParasList *getParasList() {
        return getChildAtAs<ParasList>(ParasListIdx);
    }

    ASTNodeT *getReturnsType() {
        return getChildAt(ReturnsTypeIdx);
    }

    FunctionDecl(): TreeNode(TreeNodeKind::N_AST_FunctionDecl) {};
};


struct FunctionDef: public TreeNode {
    static constexpr size_t IdentIdx = 0;
    static constexpr size_t ParasListIdx = 1;
    static constexpr size_t ReturnsTypeIdx = 2;
    static constexpr size_t BlockIdx = 3;
    static constexpr size_t ExprIdx = 3; // should be 3

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_FunctionDef;
    }

    void setIdent(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    void setParasList(ParasList *ParasList) {
        setChildAt(ParasListIdx, ParasList);
    }

    void setReturnsType(ASTNodeT *N) {
        setChildAt(ReturnsTypeIdx, N);
    }

    void setBlock(ASTNodeT *Block) {
        setChildAt(BlockIdx, Block);
    }

    void setExpr(ASTNodeT *Expr) {
        setChildAt(ExprIdx, Expr);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    ParasList *getParasList() {
        return getChildAtAs<ParasList>(ParasListIdx);
    }

    ASTNodeT *getReturnsType() {
        return getChildAt(ReturnsTypeIdx);
    }

    Block *getBlock() {
        return getChildAtAs<Block>(BlockIdx);
    }

    ASTNodeT *getExpr() {
        return getChildAt(ExprIdx);
    }

    FunctionDef(): TreeNode(TreeNodeKind::N_AST_FunctionDef) {};
};


struct FunctionCall: public TreeNode {
    static constexpr size_t IdentIdx = 0;
    static constexpr size_t ArgsListIdx = 1;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_FunctionCall;
    }

    void setIdent(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    void setArgsList(ArgsList *ArgsList) {
        setChildAt(ArgsListIdx, ArgsList);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    ArgsList *getArgsList() {
        return getChildAtAs<ArgsList>(ArgsListIdx);
    }

    FunctionCall(): TreeNode(TreeNodeKind::N_AST_FunctionCall) {}
};


struct ProcedureDecl: public TreeNode {
    static constexpr size_t IdentIdx = 0;
    static constexpr size_t ParasListIdx = 1;
    static constexpr size_t ReturnsTypeIdx = 2;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ProcedureDecl;
    }

    void setIdent(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    void setParasList(ParasList *ParasList) {
        setChildAt(ParasListIdx, ParasList);
    }

    void setReturnsType(ASTNodeT *N) {
        setChildAt(ReturnsTypeIdx, N);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    ParasList *getParasList() {
        return getChildAtAs<ParasList>(ParasListIdx);
    }

    ASTNodeT *getReturnsType() {
        return getChildAt(ReturnsTypeIdx);
    }

    ProcedureDecl(): TreeNode(TreeNodeKind::N_AST_ProcedureDecl) {}
};


struct ProcedureDef: public TreeNode {
    static constexpr size_t IdentIdx = 0;
    static constexpr size_t ParasListIdx = 1;
    static constexpr size_t ReturnsTypeIdx = 2;
    static constexpr size_t BlockIdx = 3;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ProcedureDef;
    }

    void setIdent(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    void setParasList(ParasList *ParasList) {
        setChildAt(ParasListIdx, ParasList);
    }

    void setReturnsType(ASTNodeT *N) {
        setChildAt(ReturnsTypeIdx, N);
    }

    void setBlock(ASTNodeT *Block) {
        setChildAt(BlockIdx, Block);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    ParasList *getParasList() {
        return getChildAtAs<ParasList>(ParasListIdx);
    }

    ASTNodeT *getReturnsType() {
        return getChildAt(ReturnsTypeIdx);
    }

    Block *getBlock() {
        return getChildAtAs<Block>(BlockIdx);
    }

    ProcedureDef(): TreeNode(TreeNodeKind::N_AST_ProcedureDef) {}
};


struct ProcedureCall: public TreeNode {
    static constexpr size_t IdentIdx = 0;
    static constexpr size_t ArgsListIdx = 1;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ProcedureCall;
    }

    void setIdent(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    void setArgsList(ArgsList *ArgsList) {
        setChildAt(ArgsListIdx, ArgsList);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    ArgsList *getArgsList() {
        return getChildAtAs<ArgsList>(ArgsListIdx);
    }

    ProcedureCall(): TreeNode(TreeNodeKind::N_AST_ProcedureCall) {}
};


struct Return: public TreeNode {
    static constexpr size_t ReturnExprIdx = 0;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Return;
    }

    void setReturnExpr(ASTNodeT *Expr) {
        setChildAt(ReturnExprIdx, Expr);
    }

    ASTNodeT *getReturnExpr() {
        return getChildAt(ReturnExprIdx);
    }

    Return(): TreeNode(TreeNodeKind::N_AST_Return) {}
};


struct Break: public TreeNode {
    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Break;
    }

    Break(): TreeNode(TreeNodeKind::N_AST_Break) {}
};


struct Continue: public TreeNode {
    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Continue;
    }

    Continue(): TreeNode(TreeNodeKind::N_AST_Continue) {}
};

struct OutStream: public TreeNode {
    static constexpr size_t OutStreamExprIdx = 0;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_OutStream;
    }

    void setOutStreamExpr(ASTNodeT *Expr) {
        setChildAt(OutStreamExprIdx, Expr);
    }

    ASTNodeT *getOutStreamExpr() {
        return getChildAt(OutStreamExprIdx);
    }

    OutStream(): TreeNode(TreeNodeKind::N_AST_OutStream) {}
};

struct InStream: public TreeNode {
    static constexpr size_t IdentIdx = 0;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_InStream;
    }

    void setIdentifier(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    InStream(): TreeNode(TreeNodeKind::N_AST_InStream) {}
};

struct ExplicitCast: public TreeNode {
    static constexpr size_t TypeIdxIdx = 0;
    static constexpr size_t ExprIdx = 1;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ExplicitCast;
    }

    void setType(ASTNodeT *N) {
        setChildAt(TypeIdxIdx, N);
    }

    void setExpr(ASTNodeT *Expr) {
        setChildAt(ExprIdx, Expr);
    }

    ASTNodeT *getType() {
        return getChildAt(TypeIdxIdx);
    }

    ASTNodeT *getExpr() {
        return getChildAt(ExprIdx);
    }

    ExplicitCast(): TreeNode(TreeNodeKind::N_AST_ExplicitCast) {}
};

#endif //GAZPREABASE_ASTNODES_H
