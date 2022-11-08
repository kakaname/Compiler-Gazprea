//
// Created by dhanrajbir on 15/10/22.
//

#ifndef VCALCBASE_EXPRTYPEANNOTATORPASS_H
#define VCALCBASE_EXPRTYPEANNOTATORPASS_H

#include "VisitorPass.h"
#include "Passes/PassManager.h"

struct ExprTypeAnnotatorPass : VisitorPass<ExprTypeAnnotatorPass, TypeId> {
    using AnnotationT = TypeId;

    TypeId visitLogicalOp(LogicalOp *Op);

    TypeId visitArithmeticOp(ArithmeticOp *Op);

    TypeId visitRange(Range *R);

    TypeId visitGenerator(Generator *Gen);

    TypeId visitFilter(Filter *Fil);

    TypeId visitIntLiteral(IntLiteral *Lit) const;

    TypeId visitIdentifier(Identifier *Ident) const;

    TypeId visitAssignment(Assignment *Assign);

    TypeId visitDeclaration(Declaration *Declr);

    TypeId visitIndex(Index *Idx);

    TypeId visitPrintStmt(PrintStmt *P);

    TypeId visitConditional(Conditional *Cond);

    TypeId visitLoop(Loop *L);

    CastTo *insertCastToNode(TypeId Target, ASTNodeT *Parent, int ChildIdx) const;

    void runOnAST(ASTPassManager &Manager, ASTNodeT &Root);

    ASTPassManager *PM;
    TypeId IntType;
    TypeId VectorType;
    TypeId BoolType;
};


#endif //VCALCBASE_EXPRTYPEANNOTATORPASS_H
