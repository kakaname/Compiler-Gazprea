//
// Created by Dhanraj on 15/10/22.
//
#include "AST/ExprTypeAnnotatorPass.h"
#include "AST/ScopeResolutionPass.h"
#include "AST/SymbolTableDummyPass.h"

TypeId ExprTypeAnnotatorPass::visitLogicalOp(LogicalOp *Op) {
    TypeId LeftType = visit(Op->getLeftExpr());
    TypeId RightType = visit(Op->getRightExpr());

    // Annotate the operands.
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Op->getLeftExpr(), LeftType);
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Op->getRightExpr(), RightType);

    // If either of the children are of boolean types, we cast them to
    // integers.
    if (LeftType == BoolType) {
        CastTo *Cast = insertCastToNode(IntType, Op, LogicalOp::LeftExprIdx);
        PM->setAnnotation<ExprTypeAnnotatorPass>(*Cast, IntType);
    }

    if (RightType == BoolType) {
        CastTo *Cast = insertCastToNode(IntType, Op, LogicalOp::RightExprIdx);
        PM->setAnnotation<ExprTypeAnnotatorPass>(*Cast, IntType);
    }

    // If either of the children were vectors, the result is a vector.
    if (RightType == VectorType || LeftType == VectorType) {
        PM->setAnnotation<ExprTypeAnnotatorPass>(*Op, VectorType);
        return VectorType;
    }

    // At this point, both the children have been cast to integers if they were
    // booleans.
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Op, BoolType);
    return BoolType;
}

TypeId ExprTypeAnnotatorPass::visitArithmeticOp(ArithmeticOp *Op) {
    TypeId LeftType = visit(Op->getLeftExpr());
    TypeId RightType = visit(Op->getRightExpr());

    // Annotate the operands.

    PM->setAnnotation<ExprTypeAnnotatorPass>(*Op->getLeftExpr(), LeftType);
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Op->getRightExpr(), RightType);

    // If either of the children are boolean we cast them to ints.
    if (LeftType == BoolType) {
        CastTo *Cast = insertCastToNode(IntType, Op, ArithmeticOp::LeftExprIdx);
        PM->setAnnotation<ExprTypeAnnotatorPass>(*Cast, IntType);
    }

    if (RightType == BoolType) {
        auto *Cast = insertCastToNode(IntType, Op, ArithmeticOp::RightExprIdx);
        PM->setAnnotation<ExprTypeAnnotatorPass>(*Cast, IntType);
    }

    // If either the children are vectors, the result is a vector.
    if (LeftType == VectorType || RightType == VectorType) {
        PM->setAnnotation<ExprTypeAnnotatorPass>(*Op, VectorType);
        return VectorType;
    }

    // At this point, both the operands are guaranteed to be integers and hence
    // the type of the node is int as well.
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Op, IntType);
    return IntType;
}

void ExprTypeAnnotatorPass::runOnAST(ASTPassManager &Manager, ASTNodeT &Root) {
    PM = &Manager;
    ScopeTreeNode &GlobalScope = Manager.getResult<ScopeResolutionPass>();
    IntType = GlobalScope.resolveType("int");
    VectorType = GlobalScope.resolveType("vector");
    BoolType = GlobalScope.resolveType("%bool");
    visit(&Root);
}

TypeId ExprTypeAnnotatorPass::visitRange(Range *R) {
    // A range is always a vector.
    TypeId StartTy = visit(R->getStartExpr());
    TypeId EndTy = visit(R->getEndExpr());

    PM->setAnnotation<ExprTypeAnnotatorPass>(*R->getStartExpr(), StartTy);
    PM->setAnnotation<ExprTypeAnnotatorPass>(*R->getEndExpr(), EndTy);

    if (StartTy != IntType) {
        insertCastToNode(IntType, R, Range::StartExprIdx);
        PM->setAnnotation<ExprTypeAnnotatorPass>(*R->getStartExpr(), IntType);
    }

    if (EndTy != IntType) {
        insertCastToNode(IntType, R, Range::EndExprIdx);
        PM->setAnnotation<ExprTypeAnnotatorPass>(*R->getEndExpr(), IntType);
    }

    PM->setAnnotation<ExprTypeAnnotatorPass>(*R, VectorType);
    return VectorType;
}

TypeId ExprTypeAnnotatorPass::visitGenerator(Generator *Gen) {
    visit(Gen->getDomain());
    TypeId ExprTy = visit(Gen->getExpr());
    // Ensure the type of the expression is integer.
    if (ExprTy != IntType) {
        auto *Cast = insertCastToNode(IntType, Gen, Generator::ExprIdx);
        PM->setAnnotation<ExprTypeAnnotatorPass>(*Cast, IntType);
    }

    // Annotate the domain var as an integer.
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Gen->getDomainVar(), IntType);

    // A generator is always a vector.
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Gen, VectorType);
    return VectorType;
}

TypeId ExprTypeAnnotatorPass::visitFilter(Filter *Fil) {
    visit(Fil->getDomain());
    TypeId ExprTy = visit(Fil->getExpr());

    if (ExprTy != BoolType) {
        auto *Cast = insertCastToNode(BoolType, Fil, Filter::ExprIdx);
        PM->setAnnotation<ExprTypeAnnotatorPass>(*Cast, BoolType);
    }

    // Annotate the domain var as an integer.
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Fil->getDomainVar(), IntType);

    // A filter is always a vector.
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Fil, VectorType);
    return VectorType;
}

TypeId ExprTypeAnnotatorPass::visitIntLiteral(IntLiteral *Lit) const {
    // An integer literal is always an integer.
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Lit, IntType);
    return IntType;
}

TypeId ExprTypeAnnotatorPass::visitIdentifier(Identifier *Ident) const {
    // Get the symbol id of the symbol to which the identifier refers to.
    unsigned SymId = Ident->getSymbolId();
    // Get the type id of the symbol to which the identifier refers to.
    TypeId Id = PM->getResult<SymbolTableDummyPass>().getSymbolById(SymId)->getTypeId();
    TypeId CopyId = Id;
    // TODO: Insert a check here.
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Ident, Id);
    return CopyId;
}

TypeId ExprTypeAnnotatorPass::visitAssignment(Assignment *Assign) {
    unsigned SymId = Assign->getIdentifier()->getSymbolId();
    TypeId SymTy = PM->getResult<SymbolTableDummyPass>().getSymbolById(SymId)->getTypeId();
    TypeId AssignTy = visit(Assign->getExpr());

    // If the type of the expression is the same as the type of the identifier.
    // then we do nothing.
    if (SymTy == AssignTy)
        return 0;
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Assign, AssignTy);

    // Otherwise we insert a cast node to cast the expression to the symbol
    // type.
    insertCastToNode(SymTy, Assign, Assignment::ExprIdx);
    // Now we mark the result of the cast to node.
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Assign->getExpr(), SymTy);
    return 0;
}

TypeId ExprTypeAnnotatorPass::visitDeclaration(Declaration *Declr) {
    unsigned SymId = Declr->getIdentifier()->getSymbolId();
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Declr->getIdentifier(), SymId);
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Declr, SymId);
    TypeId SymTy = PM->getResult<SymbolTableDummyPass>().getSymbolById(SymId)->getTypeId();
    TypeId AssignTy = visit(Declr->getInitExpr());
    // If the type of the expression is the same as the type of the identifier.
    // then we do nothing.
    if (SymTy == AssignTy)
        return 0;

    // Otherwise we insert a cast node to cast the expression to the symbol
    // type.
    insertCastToNode(SymTy, Declr, Declaration::ExprIdx);

    // Now we mark the result of the cast to node.
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Declr->getInitExpr(), SymTy);
    return 0;
}

TypeId ExprTypeAnnotatorPass::visitIndex(Index *Idx) {
    // Visit the base expr.
    visit(Idx->getBaseExpr());
    TypeId IndexTy = visit(Idx->getIndexExpr());
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Idx->getIndexExpr(), IndexTy);

    // If index is of type int, the resultant type is int.
    if (IndexTy == IntType) {
        PM->setAnnotation<ExprTypeAnnotatorPass>(*Idx, IntType);
        return IntType;
    }

    // If the index expr has type bool, we cast it to int.
    if (IndexTy == BoolType) {
        insertCastToNode(IntType, Idx, Index::IndexExprIdx);
        PM->setAnnotation<ExprTypeAnnotatorPass>(*Idx->getIndexExpr(), IntType);
        PM->setAnnotation<ExprTypeAnnotatorPass>(*Idx, IntType);
        return IntType;
    }

    // Otherwise the Index is of type vector in which case, the type of the
    // index expression is vector.
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Idx, VectorType);
    return VectorType;
}

CastTo *ExprTypeAnnotatorPass::insertCastToNode(TypeId Target, ASTNodeT *Parent, int ChildIdx) const {
    auto Cast = PM->Builder.build<CastTo>();
    Cast->setTargetType(Target);
    Cast->setParent(Parent);
    auto Child = Parent->getChildAtStrong(ChildIdx);
    Child->setParent(Cast.get());
    Cast->setExpr(Child);
    Parent->replaceChildAt(Cast, ChildIdx);
    return Cast.get();
}

TypeId ExprTypeAnnotatorPass::visitPrintStmt(PrintStmt *P) {
    TypeId ExprType = visit(P->getExpr());
    // We know how tp print vectors and integers.
    if (ExprType == IntType || ExprType == VectorType) {
        PM->setAnnotation<ExprTypeAnnotatorPass>(*P->getExpr(), ExprType);
        return 0;
    }

    // Otherwise the type must be a bool.
    assert(ExprType == BoolType && "Trying to print an unknown type.");
    // Mark the expression as a bool
    PM->setAnnotation<ExprTypeAnnotatorPass>(*P->getExpr(), BoolType);
    // Cast the bool to an int before printing.
    insertCastToNode(IntType, P, PrintStmt::ChildExprIdx);
    // Now mark the cost to node as yielding an integer.
    PM->setAnnotation<ExprTypeAnnotatorPass>(*P->getExpr(), IntType);
    return 0;
}

TypeId ExprTypeAnnotatorPass::visitConditional(Conditional *Cond) {
    visit(Cond->getBlock());
    // Make sure the expr is of type bool, if not make sure its an int and cast
    // it to a bool.
    auto *C = Cond->getCondition();
    TypeId CondTy = visit(C);
    assert((CondTy == IntType || CondTy == BoolType) && "Invalid type used as"
                                                        " the result of a "
                                                        "conditional");

    PM->setAnnotation<ExprTypeAnnotatorPass>(*Cond->getCondition(), CondTy);
    // If the type of the expression is already bool, we just return.
    if (CondTy == BoolType) {
        return 0;
    }

    // Otherwise it must be int and hence we cast it to bool.
    insertCastToNode(BoolType, Cond, Conditional::ConditionalIdx);
    // Annotate the cast node as bool because we just inserted it.
    PM->setAnnotation<ExprTypeAnnotatorPass>(*Cond->getCondition(), BoolType);
    return 0;

}

TypeId ExprTypeAnnotatorPass::visitLoop(Loop *L) {
    visit(L->getBlock());
    // Make sure the expr is of type bool, if not make sure its an int and cast
    // it to a bool.
    auto *C = L->getCondition();
    TypeId CondTy = visit(C);
    assert((CondTy == IntType || CondTy == BoolType) && "Invalid type used as"
                                                        " the result of a "
                                                        "conditional");

    PM->setAnnotation<ExprTypeAnnotatorPass>(*L->getCondition(), CondTy);
    // If the type of the expression is already bool, we just return.
    if (CondTy == BoolType) {
        return 0;
    }

    // Otherwise it must be int and hence we cast it to bool.
    insertCastToNode(BoolType, L, Loop::ConditionalIdx);
    // Annotate the cast node as bool because we just inserted it.
    PM->setAnnotation<ExprTypeAnnotatorPass>(*L->getCondition(), BoolType);
    return 0;
}
