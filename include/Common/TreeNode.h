//
// Created by dhanrajbir on 28/10/22.
//

#ifndef GAZPREABASE_TREENODE_H
#define GAZPREABASE_TREENODE_H

#include "llvm/Support/Casting.h"

#include <vector>
#include <list>

using std::list;
using std::advance;
using llvm::cast;

struct TreeNode {
public:
    using ChildrenContainerT = list<TreeNode*>;

    enum TreeNodeKind {
        N_AST_Program,
        N_AST_Identifier,
        N_AST_Assignment,
        N_AST_CalleeParameter,
        N_AST_Declaration,
        N_AST_Block,
        N_AST_LogicalOp,
        N_AST_ArithmeticOp,
        N_AST_Index,
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
        N_AST_TupleTypeDecl,
        N_AST_IntegerTypeNode,
        N_AST_BooleanTypeNode,
        N_AST_CharacterTypeNode,
        N_AST_RealTypeNode,
        N_AST_InfiniteLoop,
        N_AST_Conditional,
        N_AST_ConditionalElse,
        N_AST_TypeCast,
        N_AST_TypeDef,
        N_AST_ResolvedType,
        N_AST_BitwiseOp,
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
        N_AST_ExplicitCast,

        // Split AST nodes from tree node.
        N_ScopeTreeNode
    };

    TreeNodeKind getKind() const {
        return Kind;
    }

    TreeNode() = delete;

    explicit TreeNode(TreeNodeKind Kind) : Kind(Kind), Parent(nullptr) {}

    void setParent(TreeNode *P) {
        Parent = P;
    }

    TreeNode *getParent() {
        return Parent;
    }

    void addChild(TreeNode *Child) {
        if (Child)
            Child->setParent(this);
        Children.push_back(Child);
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
        assert(Pos < Children.size());
        auto I = Children.begin();
        advance(I, Pos);
        return *I;
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
        while (Children.size() < Pos)
            Children.push_back(nullptr);
        auto I = Children.begin();
        advance(I, Pos);
        Children.insert(I, Child);
    }

private:
    TreeNodeKind Kind;
    TreeNode *Parent;
};



#endif //GAZPREABASE_TREENODE_H