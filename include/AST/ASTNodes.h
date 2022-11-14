//
// Created by dhanrajbir on 28/10/22.
//

#ifndef GAZPREABASE_ASTNODES_H
#define GAZPREABASE_ASTNODES_H

#include <string>
#include <vector>

#include "Common/TreeNode.h"
#include "Types/Type.h"
#include "Symbol/Symbol.h"

using std::string;
using std::vector;

using ASTNodeT = TreeNode;

struct Program: public TreeNode {

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Program;
    }

    Program() : TreeNode(TreeNodeKind::N_AST_Program) {}
};

struct Identifier: public TreeNode {
    string IdentName;
    const Type *IdentType;

    const Symbol *Referred;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Identifier;
    }

    Identifier() : TreeNode(TreeNodeKind::N_AST_Identifier),
        IdentType(nullptr), Referred(nullptr) {}

    void setName(const string &N) {
        IdentName = N;
    }

    void setIdentType(const Type *T) {
        IdentType = T;
    }
    
    const Type *getIdentType() const {
        return IdentType;
    }

    const string &getName() const {
        return IdentName;
    }

    void setReferred(const Symbol *S) {
        Referred = S;
    }

    const Symbol *getReferred() const {
        return Referred;
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
    static constexpr size_t IdentIdx = 0;
    static constexpr size_t InitExprIdx = 1;

    const Type *IdentType{nullptr};

    bool IsConst{false};

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Declaration;
    }

    void setIdentType(const Type *T) {
        IdentType = T;
    }

    void setIdent(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    void setInitExpr(ASTNodeT *Expr) {
        setChildAt(InitExprIdx, Expr);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    ASTNodeT *getInitExpr() {
        return getChildAt(InitExprIdx);
    }

    const Type *getIdentType() const {
        return IdentType;
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

    OpKind getOpKind() const {
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
        EXP = 0, MUL, DIV, MOD, DOTPROD, ADD, SUB
    };

    OpKind Op;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ArithmeticOp;
    }

    void setOp(OpKind OpKind) {
        Op = OpKind;
    }

    OpKind getOpKind() const {
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
    static constexpr int BlockIdx = 0;

    void setBlock(Block *Block) {
        setChildAt(BlockIdx, Block);
    }

    ASTNodeT *getBlock() {
        return getChildAtAs<Block>(BlockIdx);
    }

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_InfiniteLoop;
    }

    InfiniteLoop() : TreeNode(TreeNodeKind::N_AST_InfiniteLoop) {};
};

struct ConditionalLoop: public TreeNode {
    static constexpr int ConditionalIdx = 0;
    static constexpr int BlockIdx = 1;

    // Check if while loop or do-while loop
    bool ConditionalBefore{true};

    void setConditionalAfter() {
        ConditionalBefore = false;
    }

    void setConditional(ASTNodeT *Expr) {
        setChildAt(ConditionalIdx, Expr);
    }

    void setBlock(Block *Stat) {
        setChildAt(BlockIdx, Stat);
    }

    ASTNodeT *getConditional() {
        return getChildAt(ConditionalIdx);
    }

    ASTNodeT *getBlock() {
        return getChildAtAs<Block>(BlockIdx);
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

    IntLiteral(): TreeNode(TreeNodeKind::N_AST_IntLiteral), Val(0) {}
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

    bool getVal() const {
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

struct Conditional: public TreeNode {
    static constexpr int ConditionalIdx = 0;
    static constexpr int BlockIdx = 1;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Conditional;
    }

    void setConditional(ASTNodeT *Expr) {
        setChildAt(ConditionalIdx, Expr);
    }

    void setBlock(Block *Stat) {
        setChildAt(BlockIdx, Stat);
    }

    ASTNodeT *getConditional() {
        return getChildAt(ConditionalIdx);
    }

    Block *getBlock() {
        return getChildAtAs<Block>(BlockIdx);
    }

    Conditional(): TreeNode(TreeNodeKind::N_AST_Conditional) {}
};

// Remains to be checked!!!!!
struct ConditionalElse: public TreeNode {
    static constexpr int ConditionalIdx = 0;
    static constexpr int IfBlockIdx = 1;
    static constexpr int ElseBlockIdx = 2;


    void setConditional(ASTNodeT *Expr) {
        setChildAt(ConditionalIdx, Expr);
    }

    void setIfBlock(Block *Stat) {
        setChildAt(IfBlockIdx, Stat);
    }

    void setElseBlock(Block *Expr) {
        setChildAt(ElseBlockIdx, Expr);
    }

    ASTNodeT *getConditional() {
        return getChildAt(ConditionalIdx);
    }

    Block *getIfBlock() {
        return getChildAtAs<Block>(IfBlockIdx);
    }

    Block *getElseBlock() {
        return getChildAtAs<Block>(ElseBlockIdx);
    }

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ConditionalElse;
    }

    ConditionalElse(): TreeNode(TreeNodeKind::N_AST_ConditionalElse) {}
};

struct TypeCast: public TreeNode {
    static constexpr int ExprIdx = 0;

    const Type* TargetType{nullptr};

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_TypeCast;
    }

    void setExpr(ASTNodeT *N) {
        setChildAt(ExprIdx, N);
    }

    ASTNodeT *getExpr() {
        return getChildAt(ExprIdx);
    }

    const Type *getTargetType() const {
        return TargetType;
    }

    void setTargetType(const Type *T) {
        TargetType = T;
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

    OpKind getOpKind() const {
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

struct CalleeParameter: public TreeNode {
    static constexpr size_t TypeNodeIdx = 0;
    static constexpr size_t IdentifierIdx = 1;

    bool IsConst{false};

    void setTypeNode(ASTNodeT *N) {
        setChildAt(TypeNodeIdx, N);
    }

    void setIdentifier(Identifier *I) {
        setChildAt(IdentifierIdx, I);
    }

    ASTNodeT *getTypeNode() {
        return getChildAt(TypeNodeIdx);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentifierIdx);
    }

    static bool classof(const TreeNode* N) {
        return N->getKind() == TreeNode::N_AST_CalleeParameter;
    }

    void setConst() {
        IsConst = true;
    }

    bool isConst() const {
        return IsConst;
    }

    CalleeParameter(): TreeNode(TreeNode::N_AST_CalleeParameter) {}
};

// Represent a list of ASTNodes representing the type of each parameter of
// a function or a procedure.
struct ParameterList: public TreeNode {
    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ParasList;
    }

    void addParam(Identifier *I) {
        addChild(I);
    }

    Identifier *getParamAt(long Pos) {
        return getChildAtAs<Identifier>(Pos);
    }

    ParameterList(): TreeNode(TreeNodeKind::N_AST_ParasList) {}
};

struct FunctionDecl: public TreeNode {
    static constexpr size_t IdentIdx = 0;

    vector<const Type*> ParamTypes;
    const Type *RetTy{nullptr};

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_FunctionDecl;
    }

    void setIdent(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    void addParam(const Type* T) {
        ParamTypes.emplace_back(T);
    }

    const Type *getParamTypeAt(long Pos) {
        return ParamTypes.at(Pos);
    }

    void setRetTy(const Type *T) {
        RetTy = T;
    }

    const Type *getRetType() const {
        return RetTy;
    }

    const vector<const Type*> &getParamTypes() const {
        return ParamTypes;
    }

    FunctionDecl(): TreeNode(TreeNodeKind::N_AST_FunctionDecl) {};
};

struct FunctionDef: public TreeNode {
    static constexpr size_t IdentIdx = 0;
    static constexpr size_t ParamListIdx = 1;
    static constexpr size_t BlockIdx = 2;

    const Type *RetTy{nullptr};

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_FunctionDef;
    }

    void setIdent(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    void setBlock(ASTNodeT *Block) {
        setChildAt(BlockIdx, Block);
    }

    Block *getBlock() {
        return getChildAtAs<Block>(BlockIdx);
    }

    void setRetTy(const Type *T) {
        RetTy = T;
    }

    const Type *getRetTy() const {
        return RetTy;
    }

    void setParamList(ParameterList *P) {
        setChildAt(ParamListIdx, P);
    }

    ParameterList *getParamList() {
        return getChildAtAs<ParameterList>(ParamListIdx);
    }

    FunctionDef(): TreeNode(TreeNodeKind::N_AST_FunctionDef) {};
};

struct ResolvedType: public TreeNode {
    string Name;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ResolvedType;
    }

    ResolvedType(): TreeNode(TreeNode::N_AST_ResolvedType) {}

    void setName(const string& N) {
        Name = N;
    }

    const string &getName() const {
        return Name;
    }
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

    const Type *RetTy;
    vector<const Type*> Arguments;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ProcedureDecl;
    }

    void setIdent(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    void setRetTy(const Type *T) {
        RetTy = T;
    }

    const Type *getRetTy() const {
        return RetTy;
    }

    void addArgumentTy(const Type *T) {
        Arguments.emplace_back(T);
    }

    vector<const Type*> &getArgumentList() {
        return Arguments;
    }

    ProcedureDecl(): TreeNode(TreeNodeKind::N_AST_ProcedureDecl) {}
};

struct ProcedureDef: public TreeNode {
    static constexpr size_t IdentIdx = 0;
    static constexpr size_t ParamListIdx = 1;
    static constexpr size_t BlockIdx = 2;

    const Type *RetTy{nullptr};

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_FunctionDef;
    }

    void setIdent(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    void setBlock(ASTNodeT *Block) {
        setChildAt(BlockIdx, Block);
    }

    Block *getBlock() {
        return getChildAtAs<Block>(BlockIdx);
    }

    void setRetTy(const Type *T) {
        RetTy = T;
    }

    const Type *getRetTy() const {
        return RetTy;
    }

    void setParamList(ParameterList *P) {
        setChildAt(ParamListIdx, P);
    }

    ParameterList *getParamList() {
        return getChildAtAs<ParameterList>(ParamListIdx);
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
    static constexpr size_t ExprIdx = 0;

    const Type *TargetType{nullptr};

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ExplicitCast;
    }

    void setTargetType(const Type *T) {
        TargetType = T;
    }

    void setExpr(ASTNodeT *Expr) {
        setChildAt(ExprIdx, Expr);
    }

    const Type *getTargetType() const {
        return TargetType;
    }

    ASTNodeT *getExpr() {
        return getChildAt(ExprIdx);
    }

    ExplicitCast(): TreeNode(TreeNodeKind::N_AST_ExplicitCast) {}
};

#endif //GAZPREABASE_ASTNODES_H
