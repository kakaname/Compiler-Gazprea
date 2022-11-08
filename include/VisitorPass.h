//
// Created by dhanrajbir on 08/10/22.
//

#ifndef VCALCBASE_VISITORPASS_H
#define VCALCBASE_VISITORPASS_H

#include <cassert>
#include "Passes/Pass.h"
#include "AST/ASTNodes.h"


template<typename DerivedT, typename RetT>
class VisitorPass : public ASTPassIDMixin<DerivedT> {

    RetT visitAssignment(Assignment *Assign) {
        visit(Assign->getIdentifier());
        visit(Assign->getExpr());
        return RetT();
    }

    RetT visitDeclaration(Declaration *Declr) {
        visit(Declr->getIdentifier());
        visit(Declr->getInitExpr());
        return RetT();
    }

    RetT visitIdentifier(Identifier *Ident) {
        return RetT();
    }

    RetT visitProgram(Program *Prog) {
        size_t NumOfChildren = Prog->numOfChildren();
        for (size_t I = 0; I < NumOfChildren; I++) {
            visit(Prog->getChildAt(I));
        }
        return RetT();
    }

    RetT visitBlock(Block *Block) {
        for (const auto& Stmt : *Block)
            visit(Stmt.get());
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

    RetT visitLoop(ConditionalLoop *L) {
        visit(L->getCondition());
        visit(L->getStatement());
        return RetT();
    }

    RetT visitConditional(Conditional *Cond) {
        visit(Cond->getCondition());
        visit(Cond->getBlock());
        return RetT();
    }

    RetT visitCastTo(CastTo *Cast) {
        visit(Cast->getExpr());
        return RetT();
    }

    RetT visitIntLiteral(IntLiteral *Lit) {
        return RetT();
    }

    RetT callVisitAssignmentImpl(Assignment *Assign) {
        return static_cast<DerivedT*>(this)->visitAssignment(Assign);
    }

    RetT callVisitDeclarationImpl(Declaration *Declaration) {
        return static_cast<DerivedT*>(this)->visitDeclaration(Declaration);
    }

    RetT callVisitIdentifierImpl(Identifier *Ident) {
        return static_cast<DerivedT*>(this)->visitIdentifier(Ident);
    }

    RetT callVisitProgramImpl(Program *Prog) {
        return static_cast<DerivedT*>(this)->visitProgram(Prog);
    }

    RetT callVisitBlockImpl(Block *Block) {
        return static_cast<DerivedT*>(this)->visitBlock(Block);
    }

    RetT callVisitLogicalOpImpl(LogicalOp *LogicalOp) {
        return static_cast<DerivedT*>(this)->visitLogicalOp(LogicalOp);
    }

    RetT callVisitArthmeticOpImpl(ArithmeticOp *ArithOp) {
        return static_cast<DerivedT*>(this)->visitArithmeticOp(ArithOp);
    }

    RetT callVisitIndexImpl(Index *Idx) {
        return static_cast<DerivedT*>(this)->visitIndex(Idx);
    }

    RetT callVisitRangeImpl(Range *Range) {
        return static_cast<DerivedT*>(this)->visitRange(Range);
    }

    RetT callVisitGeneratorImpl(Generator *Gen) {
        return static_cast<DerivedT*>(this)->visitGenerator(Gen);
    }

    RetT callVisitFilterImpl(Filter *Filter) {
        return static_cast<DerivedT*>(this)->visitFilter(Filter);
    }

    RetT callVisitLoopImpl(Loop *L) {
        return static_cast<DerivedT*>(this)->visitLoop(L);
    }

    RetT callVisitConditionalImpl(Conditional *Cond) {
        return static_cast<DerivedT*>(this)->visitConditional(Cond);
    }

    RetT callVisitPrintStmtImpl(PrintStmt *PStmt) {
        return static_cast<DerivedT*>(this)->visitPrintStmt(PStmt);
    }

    RetT callVisitCastToImpl(CastTo *Cast) {
        return static_cast<DerivedT*>(this)->visitCastTo(Cast);
    }

    RetT callVisitIntLiteralImpl(IntLiteral *Literal) {
        return static_cast<DerivedT*>(this)->visitIntLiteral(Literal);
    }

public:
    RetT visit(ASTNodeT *Node) {
        if (auto *Assign = dynamic_cast<Assignment*>(Node))
            return callVisitAssignmentImpl(Assign);

        if (auto *Decl = dynamic_cast<Declaration*>(Node))
            return callVisitDeclarationImpl(Decl);

        if (auto *Ident = dynamic_cast<Identifier*>(Node))
            return callVisitIdentifierImpl(Ident);

        if (auto *Prog = dynamic_cast<Program*>(Node))
            return callVisitProgramImpl(Prog);

        if (auto *Blk = dynamic_cast<Block*>(Node))
            return callVisitBlockImpl(Blk);

        if (auto *LOp = dynamic_cast<LogicalOp*>(Node))
            return callVisitLogicalOpImpl(LOp);

        if (auto *AOp = dynamic_cast<ArithmeticOp*>(Node))
            return callVisitArthmeticOpImpl(AOp);

        if (auto *Idx = dynamic_cast<Index*>(Node))
            return callVisitIndexImpl(Idx);

        if (auto *R = dynamic_cast<Range*>(Node))
            return callVisitRangeImpl(R);

        if (auto *Gen = dynamic_cast<Generator*>(Node))
            return callVisitGeneratorImpl(Gen);

        if (auto *F = dynamic_cast<Filter*>(Node))
            return callVisitFilterImpl(F);

        if (auto *L = dynamic_cast<Loop*>(Node))
            return callVisitLoopImpl(L);

        if (auto *Cond = dynamic_cast<Conditional*>(Node))
            return callVisitConditionalImpl(Cond);

        if (auto *Print = dynamic_cast<PrintStmt*>(Node))
            return callVisitPrintStmtImpl(Print);

        if (auto *Cast = dynamic_cast<CastTo*>(Node))
            return callVisitCastToImpl(Cast);

        if (auto *Literal = dynamic_cast<IntLiteral*>(Node))
            return callVisitIntLiteralImpl(Literal);

        assert(false && "Unknown AST node found in the VisitorPass visit method.");
    }

    void runOnAST(ASTPassManager &PManager, ASTNodeT &Root) {
        visit(&Root);
    };
};

#endif //VCALCBASE_VISITORPASS_H
