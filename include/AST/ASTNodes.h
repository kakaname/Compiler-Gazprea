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

    ChildrenContainerT &getChildren() {
        return Children;
    }

    Program() : TreeNode(TreeNodeKind::N_AST_Program) {}
};

struct NoOp: public TreeNode {
    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_NoOp;
    }

    NoOp(): TreeNode(TreeNodeKind::N_AST_NoOp) {}
};

struct Identifier: public TreeNode {
    string IdentName;
    Type *IdentType;

    const Symbol *Referred;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Identifier;
    }

    Identifier() : TreeNode(TreeNodeKind::N_AST_Identifier),
        IdentType(nullptr), Referred(nullptr) {}

    void setName(const string &N) {
        IdentName = N;
    }

    void setIdentType(Type *T) {
        IdentType = T;
    }
    
    Type *getIdentType() const {
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

    static constexpr size_t AssignedToChildIdx = 0;
    static constexpr size_t ExprChildIdx = 1;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Assignment;
    }

    void setAssignedTo(ASTNodeT *AssignedTo) {
        setChildAt(AssignedToChildIdx, AssignedTo);
    }

    ASTNodeT *getAssignedTo() {
        return getChildAt(AssignedToChildIdx);
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

    Type *IdentType{nullptr};

    bool IsConst{false};

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Declaration;
    }

    void setIdentType(Type *T) {
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

    Type *getIdentType() const {
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

struct ComparisonOp: public TreeNode {
    static constexpr int LeftExprIdx = 0;
    static constexpr int RightExprIdx = 1;

    enum OpKind {
        LT = 0, GT, LTEQ, GTEQ,
    };

    OpKind Op;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ComparisonOp;
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

    ComparisonOp() : TreeNode(TreeNodeKind::N_AST_ComparisonOp) {}
};

struct ArithmeticOp: public TreeNode {
    static constexpr int LeftExprIdx = 0;
    static constexpr int RightExprIdx = 1;

    enum OpKind {
        EXP = 0, MUL, DIV, MOD, ADD, SUB
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
    static constexpr int Index2ExprIdx = 2;

    void setBaseExpr(ASTNodeT *Expr) {
        setChildAt(BaseExprIdx, Expr);
    }

    void setIndexExpr(ASTNodeT *Expr) {
        setChildAt(IndexExprIdx, Expr);
    }

    void setIndex2Expr(ASTNodeT *Expr) {
        setChildAt(Index2ExprIdx, Expr);
    }

    ASTNodeT *getBaseExpr() {
        return getChildAt(BaseExprIdx);
    }

    ASTNodeT *getIndexExpr() {
        return getChildAt(IndexExprIdx);
    }

    ASTNodeT *getIndex2Expr() {
        return getChildAt(Index2ExprIdx);
    }

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Index;
    }

    Index() : TreeNode(TreeNodeKind::N_AST_Index) {};
};

struct IndexReference: public TreeNode {
    static constexpr int BaseExprIdx = 0;
    static constexpr int IndexExprIdx = 1;
    static constexpr int Index2ExprIdx = 2;

    void setBaseExpr(ASTNodeT *Expr) {
        setChildAt(BaseExprIdx, Expr);
    }

    void setIndexExpr(ASTNodeT *Expr) {
        setChildAt(IndexExprIdx, Expr);
    }

    void setIndex2Expr(ASTNodeT *Expr) {
        setChildAt(Index2ExprIdx, Expr);
    }

    ASTNodeT *getBaseExpr() {
        return getChildAt(BaseExprIdx);
    }

    ASTNodeT *getIndexExpr() {
        return getChildAt(IndexExprIdx);
    }

    ASTNodeT *getIndex2Expr() {
        return getChildAt(Index2ExprIdx);
    }

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_IndexReference;
    }

    IndexReference() : TreeNode(TreeNodeKind::N_AST_IndexReference) {};
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

struct DomainLoop: public TreeNode {

    static constexpr int IDIdx = 0;
    static constexpr int DomainIdx = 1;
    static constexpr int BodyIdx = 2;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_DomainLoop;
    }

    void setID(Identifier *ID) {
        setChildAt(IDIdx, ID);
    }

    void setBody(Block *Body) {
        setChildAt(BodyIdx, Body);
    }

    void setDomain(ASTNodeT *Domain) {
        setChildAt(DomainIdx, Domain);
    }

    Block *getBody() {
        return dyn_cast<Block>(getChildAt(BodyIdx));
    }

    Identifier *getID() {
        return dyn_cast<Identifier>(getChildAt(IDIdx));
    }

    ASTNodeT *getDomain() {
        return getChildAt(DomainIdx);
    }

    bool isBreakable() const {
        return Breakable;
    }

    void setBreakable(bool b) {
        Breakable = b;
    }

    DomainLoop(): TreeNode(TreeNodeKind::N_AST_DomainLoop) {}

private:
    bool Breakable = false;

};

struct IntLiteral: public TreeNode {
    long Val;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_IntLiteral;
    }

    void setVal(const std::basic_string<char> &Literal) {
        Val = std::stol(Literal);
    }

    void setIntVal(long V) {
        Val = V;
    }

    long getVal() const {
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

    void setVal(const string &FloatString) {
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

    void setCharacter(char Literal) {
        Character = Literal;
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

struct TupleDestruct: public TreeNode {
    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_TupleDestruct;
    }

    TupleDestruct(): TreeNode(TreeNodeKind::N_AST_TupleDestruct) {}
};

struct MemberAccess: public TreeNode {
    static constexpr size_t ExprIdx = 0;
    static constexpr size_t MemberExprIdx = 1;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_MemberAccess;
    }

    void setExpr(ASTNodeT *Expr) {
        setChildAt(ExprIdx, Expr);
    }

    void setMemberExpr(ASTNodeT *Expr) {
        setChildAt(MemberExprIdx, Expr);
    }

    ASTNodeT *getExpr() {
        return getChildAt(ExprIdx);
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

    Type *TargetType{nullptr};

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_TypeCast;
    }

    void setExpr(ASTNodeT *N) {
        setChildAt(ExprIdx, N);
    }

    ASTNodeT *getExpr() {
        return getChildAt(ExprIdx);
    }

    Type *getTargetType() const {
        return TargetType;
    }

    void setTargetType(Type *T) {
        TargetType = T;
    }

    TypeCast(): TreeNode(TreeNodeKind::N_AST_TypeCast) {}
};

struct LogicalOp: public TreeNode {
    static constexpr int LeftExprIdx = 0;
    static constexpr int RightExprIdx = 1;

    enum OpKind {
        AND = 0, OR, XOR, EQ, NEQ
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

    LogicalOp(): TreeNode(TreeNodeKind::N_AST_LogicalOp) {}
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

    ASTNodeT *getExprAtPos(size_t Pos) {
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

    Identifier *getParamAt(size_t Pos) {
        return getChildAtAs<Identifier>(Pos);
    }

    ParameterList(): TreeNode(TreeNodeKind::N_AST_ParasList) {}
};

struct FunctionDecl: public TreeNode {
    static constexpr size_t IdentIdx = 0;

    vector<Type*> ParamTypes;
    Type *RetTy{nullptr};

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_FunctionDecl;
    }

    void setIdent(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    void addParam(Type *T) {
        ParamTypes.emplace_back(T);
    }

    Type *getParamTypeAt(long Pos) {
        return ParamTypes.at(Pos);
    }

    void setRetTy(Type *T) {
        RetTy = T;
    }

    Type *getRetType() const {
        return RetTy;
    }

    vector<Type*> &getParamTypes() {
        return ParamTypes;
    }

    FunctionDecl(): TreeNode(TreeNodeKind::N_AST_FunctionDecl) {};
};

struct FunctionDef: public TreeNode {
    static constexpr size_t IdentIdx = 0;
    static constexpr size_t ParamListIdx = 1;
    static constexpr size_t BlockIdx = 2;

    Type *RetTy{nullptr};

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

    void setRetTy(Type *T) {
        RetTy = T;
    }

    Type *getRetTy() const {
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

    Type *RetTy{nullptr};
    vector<Type*> ParamTypes;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ProcedureDecl;
    }

    void setIdent(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    void setRetTy(Type *T) {
        RetTy = T;
    }

    Type *getRetTy() const {
        return RetTy;
    }

    void addParamTy(Type *T) {
        ParamTypes.emplace_back(T);
    }

    vector<Type*> &getParamTypes() {
        return ParamTypes;
    }

    ProcedureDecl(): TreeNode(TreeNodeKind::N_AST_ProcedureDecl) {}
};

struct ProcedureDef: public TreeNode {
    static constexpr size_t IdentIdx = 0;
    static constexpr size_t ParamListIdx = 1;
    static constexpr size_t BlockIdx = 2;

    Type *RetTy{nullptr};

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ProcedureDef;
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

    void setRetTy(Type *T) {
        RetTy = T;
    }

    Type *getRetTy() const {
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

struct Interval: public TreeNode {
    static constexpr size_t LowerExprIdx = 0;
    static constexpr size_t UpperExprIdx = 1;

    // This represents the number of range bound checks we have to do.
    // These checks are emitted into the codegen
    uint32_t CheckCount;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Interval;
    }

    void setLowerExpr(ASTNodeT *Expr) {
        setChildAt(LowerExprIdx, Expr);
    }

    void setUpperExpr(ASTNodeT *Expr) {
        setChildAt(UpperExprIdx, Expr);
    }

    void addCheck(ASTNodeT *Expr) {
        addChild(Expr);
        CheckCount++;
    }

    void copyChecksFrom(Interval *Other) {
        for (size_t i = 0; i < Other->CheckCount; i++) {
            addCheck(Other->getChildAt(i+2));
        }
    }

    ASTNodeT *getLowerExpr() {
        return getChildAt(LowerExprIdx);
    }

    ASTNodeT *getUpperExpr() {
        return getChildAt(UpperExprIdx);
    }

    Interval(): TreeNode(TreeNodeKind::N_AST_Interval) {}
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


struct StreamState: public TreeNode {
    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_StreamState;
    }

    StreamState(): TreeNode(TreeNodeKind::N_AST_StreamState) {}
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
    static constexpr size_t LValueIdx = 0;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_InStream;
    }

    void setTarget(ASTNodeT *Target) {
        setChildAt(LValueIdx, Target);
    }

    ASTNodeT *getTarget() {
        return getChildAt(LValueIdx);
    }

    InStream(): TreeNode(TreeNodeKind::N_AST_InStream) {}
};

struct ExplicitCast: public TreeNode {
    static constexpr size_t ExprIdx = 0;

    Type *TargetType{nullptr};

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ExplicitCast;
    }

    void setTargetType(Type *T) {
        TargetType = T;
    }

    void setExpr(ASTNodeT *Expr) {
        setChildAt(ExprIdx, Expr);
    }

    Type *getTargetType() const {
        return TargetType;
    }

    ASTNodeT *getExpr() {
        return getChildAt(ExprIdx);
    }

    ExplicitCast(): TreeNode(TreeNodeKind::N_AST_ExplicitCast) {}
};

struct IdentReference : public TreeNode {
    static constexpr size_t IdentIdx = 0;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_IdentReference;
    }

    void setIdentifier(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    IdentReference() : TreeNode(TreeNodeKind::N_AST_IdentReference) {}
};


struct MemberReference : public TreeNode {
    static constexpr size_t IdentIdx = 0;
    static constexpr size_t MemberIdx = 1;


    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_MemberReference;
    }

    void setIdentifier(Identifier *Ident) {
        setChildAt(IdentIdx, Ident);
    }

    Identifier *getIdentifier() {
        return getChildAtAs<Identifier>(IdentIdx);
    }

    void setMemberExpr(ASTNodeT *Expr) {
        setChildAt(MemberIdx, Expr);
    }

    ASTNodeT *getMemberExpr() {
        return getChildAt(MemberIdx);
    }

    MemberReference() : TreeNode(TreeNodeKind::N_AST_MemberReference) {}
};


struct FreeNode : public TreeNode {
    vector<ASTNodeT *> FreedIdentifiers;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_FreeNode;
    }

    FreeNode() : TreeNode(TreeNodeKind::N_AST_FreeNode) {}
};

struct VectorLiteral: public TreeNode {
    Type *MemType;
    int vecSize;    

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_VectorLiteral;
    }


    void setExprAtPos(ASTNodeT *Member, size_t location) {
        setChildAt(location, Member);
    } 

    ASTNodeT *getExprAtPos(size_t location) {
        return getChildAt(location);
    }

    VectorLiteral() : TreeNode(TreeNodeKind::N_AST_VectorLiteral) {};
    
};

struct StringLiteral: public TreeNode {
    // vector but changed for different -> std_output;
    const Type *MemType;
    int vecSize;    

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_StringLiteral;
    }


    void setExprAtPos(ASTNodeT *Member, size_t location) {
        setChildAt(location, Member);
    } 

    ASTNodeT *getExprAtPos(size_t location) {
        return getChildAt(location);
    }

    StringLiteral() : TreeNode(TreeNodeKind::N_AST_StringLiteral) {};
    
};
struct DotProduct: public TreeNode {
    static constexpr size_t LHSIdx = 0;
    static constexpr size_t RHSIdx = 1;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_DotProduct;
    }

    void setLHS(ASTNodeT *LHS) {
        setChildAt(LHSIdx, LHS);
    }

    void setRHS(ASTNodeT *RHS) {
        setChildAt(RHSIdx, RHS);
    }

    ASTNodeT *getLHS() {
        return getChildAt(LHSIdx);
    }

    ASTNodeT *getRHS() {
        return getChildAt(RHSIdx);
    }

    DotProduct() : TreeNode(TreeNodeKind::N_AST_DotProduct) {}
};

struct Concat: public TreeNode {
    static constexpr size_t LHSIdx = 0;
    static constexpr size_t RHSIdx = 1;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Concat;
    }

    void setLHS(ASTNodeT *LHS) {
        setChildAt(LHSIdx, LHS);
    }

    void setRHS(ASTNodeT *RHS) {
        setChildAt(RHSIdx, RHS);
    }

    ASTNodeT *getLHS() {
        return getChildAt(LHSIdx);
    }

    ASTNodeT *getRHS() {
        return getChildAt(RHSIdx);
    }

    Concat() : TreeNode(TreeNodeKind::N_AST_Concat) {}
};

struct ByOp: public TreeNode {
    static constexpr size_t LHSIdx = 0;
    static constexpr size_t RHSIdx = 1;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_ByOp;
    }

    void setBaseExpr(ASTNodeT *LHS) {
        setChildAt(LHSIdx, LHS);
    }

    void setByExpr(ASTNodeT *RHS) {
        setChildAt(RHSIdx, RHS);
    }

    ASTNodeT *getBaseExpr() {
        return getChildAt(LHSIdx);
    }

    ASTNodeT *getByExpr() {
        return getChildAt(RHSIdx);
    }

    ByOp() : TreeNode(TreeNodeKind::N_AST_ByOp) {}
};

struct Generator : public TreeNode {
    static constexpr int DomainVarIdx = 0;
    static constexpr int DomainIdx = 1;
    static constexpr int ExprIdx = 2;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Generator;
    }

    void setDomainVariable(Identifier *Ident) {
        setChildAt(DomainVarIdx, Ident);
    }

    void setDomain(ASTNodeT *Domain) {
        setChildAt(DomainIdx, Domain);
    }

    void setExpr(ASTNodeT *Expr) {
        setChildAt(ExprIdx, Expr);
    }

    Identifier *getDomainVar() {
        return getChildAtAs<Identifier>(DomainVarIdx);
    }

    ASTNodeT *getDomain() {
        return getChildAt(DomainIdx);
    }

    ASTNodeT *getExpr() {
        return getChildAt(ExprIdx);
    }

    Generator() : TreeNode(TreeNodeKind::N_AST_Generator) {};
};

struct MatrixGenerator : public TreeNode {
    static constexpr int RowDomainVarIdx = 0;
    static constexpr int RowDomainIdx = 1;
    static constexpr int ColumnDomainVarIdx = 2;
    static constexpr int ColumnDomainIdx = 3;
    static constexpr int ExprIdx = 4;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_MatrixGenerator;
    }

    void setRowDomainVariable(Identifier *Ident) {
        setChildAt(RowDomainVarIdx, Ident);
    }

    void setRowDomain(ASTNodeT *Domain) {
        setChildAt(RowDomainIdx, Domain);
    }

    void setColumnDomainVariable(Identifier *Ident) {
        setChildAt(ColumnDomainVarIdx, Ident);
    }

    void setColumnDomain(ASTNodeT *Domain) {
        setChildAt(ColumnDomainIdx, Domain);
    }

    void setExpr(ASTNodeT *Expr) {
        setChildAt(ExprIdx, Expr);
    }

    Identifier *getRowDomainVar() {
        return getChildAtAs<Identifier>(RowDomainVarIdx);
    }

    ASTNodeT *getRowDomain() {
        return getChildAt(RowDomainIdx);
    }

    Identifier *getColumnDomainVar() {
        return getChildAtAs<Identifier>(ColumnDomainVarIdx);
    }

    ASTNodeT *getColumnDomain() {
        return getChildAt(ColumnDomainIdx);
    }

    ASTNodeT *getExpr() {
        return getChildAt(ExprIdx);
    }

    MatrixGenerator() : TreeNode(TreeNodeKind::N_AST_MatrixGenerator) {};

};


struct PredicatedList : public TreeNode {
    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_PredicatedList;
    }

    PredicatedList() : TreeNode(TreeNodeKind::N_AST_PredicatedList) {};
};


struct Filter : public TreeNode {
    static constexpr int DomainVarIdx = 0;
    static constexpr int DomainIdx = 1;
    static constexpr int PredicatedListIdx = 2;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Filter;
    }

    void setDomainVariable(Identifier *Ident) {
        setChildAt(DomainVarIdx, Ident);
    }

    void setDomain(ASTNodeT *Domain) {
        setChildAt(DomainIdx, Domain);
    }

    void setPredicatedList(PredicatedList *PredList) {
        setChildAt(PredicatedListIdx, PredList);
    }

    Identifier *getDomainVar() {
        return getChildAtAs<Identifier>(DomainVarIdx);
    }

    ASTNodeT *getDomain() {
        return getChildAt(DomainIdx);
    }

    PredicatedList *getPredicatedList() {
        return getChildAtAs<PredicatedList>(PredicatedListIdx);
    }

    Filter() : TreeNode(TreeNodeKind::N_AST_Filter) {};

};


struct AppendNode : public TreeNode {
    static constexpr int LeftExprIdx = 0;
    static constexpr int RightExprIdx = 1;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_AppendNode;
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

    AppendNode() : TreeNode(TreeNodeKind::N_AST_AppendNode) {};
};

struct LengthFunc : public TreeNode {
    static constexpr int ExprIdx = 0;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Length;
    }
    
    void setVector(ASTNodeT * Expr){
        setChildAt(ExprIdx, Expr);
    }

    ASTNodeT *getVector(){
        return getChildAt(ExprIdx);
    }

    LengthFunc() : TreeNode(TreeNodeKind::N_AST_Length) {};

};

struct RowFunc : public TreeNode {
    static constexpr int ExprIdx = 0;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Row;
    }
    
    void setMatrix(ASTNodeT * Expr){
        setChildAt(ExprIdx, Expr);
    }

    ASTNodeT *getMatrix(){
        return getChildAt(ExprIdx);
    }

    RowFunc() : TreeNode(TreeNodeKind::N_AST_Row) {};

};

struct ColFunc : public TreeNode {
    static constexpr int ExprIdx = 0;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Col;
    }
    
    void setMatrix(ASTNodeT * Expr){
        setChildAt(ExprIdx, Expr);
    }

    ASTNodeT *getMatrix(){
        return getChildAt(ExprIdx);
    }

    ColFunc() : TreeNode(TreeNodeKind::N_AST_Col) {};

};

struct ReverseFunc : public TreeNode {
    static constexpr int ExprIdx = 0;

    static bool classof(const TreeNode *N) {
        return N->getKind() == TreeNodeKind::N_AST_Reverse;
    }
    
    void setVector(ASTNodeT * Expr){
        setChildAt(ExprIdx, Expr);
    }

    ASTNodeT *getVector(){
        return getChildAt(ExprIdx);
    }

    ReverseFunc() : TreeNode(TreeNodeKind::N_AST_Reverse) {};

};
#endif //GAZPREABASE_ASTNODES_H
