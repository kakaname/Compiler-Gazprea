//
// Created by dhanrajbir on 28/10/22.
//

#ifndef GAZPREABASE_TREENODE_H
#define GAZPREABASE_TREENODE_H

#include "llvm/Support/Casting.h"
#include "GazpreaLexer.h"

#include <vector>
#include <list>

#include <algorithm>

using std::list;
using std::advance;
using llvm::cast;

struct TreeNode {
public:
    using ChildrenContainerT = list<TreeNode*>;

    enum TreeNodeKind {
        N_AST_Program,
        N_AST_NoOp,
        N_AST_Identifier,
        N_AST_Assignment,
        N_AST_MemberReference,
        N_AST_IdentReference,
        N_AST_CalleeParameter,
        N_AST_Declaration,
        N_AST_Block,
        N_AST_ComparisonOp,
        N_AST_ArithmeticOp,
        N_AST_Index,
        N_AST_IndexReference,
        N_AST_ConditionalLoop,
        N_AST_DomainLoop,
        N_AST_IntLiteral,
        N_AST_NullLiteral,
        N_AST_IdentityLiteral,
        N_AST_RealLiteral,
        N_AST_BoolLiteral,
        N_AST_CharLiteral,
        N_AST_TupleLiteral,
        N_AST_MemberAccess,
        N_AST_InfiniteLoop,
        N_AST_Conditional,
        N_AST_ConditionalElse,
        N_AST_TypeCast,
        N_AST_ResolvedType,
        N_AST_LogicalOp,
        N_AST_UnaryOp,
        N_AST_ArgsList,
        N_AST_ParasList,
        N_AST_FunctionDecl,
        N_AST_FunctionDef,
        N_AST_FunctionCall,
        N_AST_ProcedureDecl,
        N_AST_ProcedureDef,
        N_AST_ProcedureCall,
        N_AST_Return,
        N_AST_Break,
        N_AST_Continue,
        N_AST_OutStream,
        N_AST_InStream,
        N_AST_Concat,
        N_AST_DotProduct,
        N_AST_ByOp,
        N_AST_ExplicitCast,
        N_AST_VectorLiteral,
        N_AST_StringLiteral,
        N_AST_Interval,
        N_AST_FreeNode,
        N_AST_TupleDestruct,
        // Split AST nodes from tree node.
        N_ScopeTreeNode,
        N_AST_Generator,
        N_AST_MatrixGenerator,
        N_AST_PredicatedList,
        N_AST_Filter,
        N_AST_AppendNode,
        N_AST_Length,
        N_AST_Row,
        N_AST_Col,
        N_AST_Reverse,
        N_AST_StreamState
    };

    TreeNodeKind getKind() const {
        return Kind;
    }

    TreeNode() = delete;

    explicit TreeNode(TreeNodeKind Kind) : Kind(Kind), Parent(nullptr), Ctx(nullptr) {}

    void setParent(TreeNode *P) {
        Parent = P;
    }

    TreeNode *getParent() {
        return Parent;
    }

    void setCtx(antlr4::ParserRuleContext *ctx) {
        Ctx = ctx;
    }

    void copyCtx(TreeNode *node) {
        Ctx = node->Ctx;
    }


    size_t getLine() {
        return Ctx->getStart()->getLine();
    }

    void addChild(TreeNode *Child) {
        if (Child)
            Child->setParent(this);
        Children.push_back(Child);
    }

    // Insert child node
    void insertChildBefore(const TreeNode *Target, TreeNode *New) {
        auto Loc = std::find(Children.begin(), Children.end(), Target);

        if(Loc == Children.end())
            throw std::runtime_error("Tried to replace a non existent child");

        if (New)
            New->setParent(this);

        Children.insert(Loc, New);
    }


    ChildrenContainerT::iterator begin() {
        return Children.begin();
    }

    ChildrenContainerT::iterator end() {
        return Children.end();
    }

    bool isLoopNode() {
        return TreeNodeKind::N_AST_DomainLoop == Kind ||
        TreeNodeKind::N_AST_ConditionalLoop == Kind ||
        TreeNodeKind::N_AST_InfiniteLoop == Kind;
    }

    TreeNode *getChildAt(size_t Pos) {
        if(Pos >= Children.size())
            throw std::runtime_error("Tried to access a child that does not exist");
        auto I = Children.begin();
        advance(I, Pos);
        return *I;
    }


    [[gnu::noinline]] size_t numOfChildren() {
        return Children.size();
    }

    void replaceChildWith(const TreeNode *Old, TreeNode *New) {
        auto Loc = std::find(Children.begin(), Children.end(), Old);

        if(Loc == Children.end())
            throw std::runtime_error("Tried to replace a non existent child");

        if (New)
            New->setParent(this);
        *Loc = New;
    }

    virtual ~TreeNode() {};
protected:

    ChildrenContainerT Children;

    template<typename T>
    T *getChildAtAs(size_t Pos) {
        return cast<T>(getChildAt(Pos));
    }

    void setChildAt(unsigned Pos, TreeNode *Child) {
        if (Child)
            Child->setParent(this);
        while (Children.size() < Pos + 1)
            Children.push_back(nullptr);
        auto I = Children.begin();
        advance(I, Pos);
        *I = Child;
    }

private:
    TreeNodeKind Kind;
    TreeNode *Parent;
    antlr4::ParserRuleContext *Ctx;
};



#endif //GAZPREABASE_TREENODE_H