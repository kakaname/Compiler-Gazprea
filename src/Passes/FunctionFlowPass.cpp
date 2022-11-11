//
// Created by rajan on 11/11/22.
//

#include "Passes/FunctionFlowPass.h"

void FunctionFlowPass::visitFunctionDef(FunctionDef *FuncDef) {
    inFunction = FuncDef;
    currFuncReturnType = nullptr;
    visit(FuncDef->getBlock());

    // Check if there is a return statement
    if (currFuncReturnType != nullptr)
        throw NoReturnError(FuncDef->getLine(), FuncDef->getIdentifier()->getName());

    // Check if the function is branch safe
    bool isFunctionBranchSafe = PM->getAnnotation<FunctionFlowPass>(FuncDef->getBlock());
    if (!isFunctionBranchSafe)
        throw InvalidControlFlowError(FuncDef->getLine(), FuncDef->getIdentifier()->getName());

    // Set type of block (note that this is not the same as the function AST type)
    PM->setAnnotation<ExprTypeAnnotatorPass>(FuncDef->getBlock(), currFuncReturnType);
    inFunction = nullptr;
    currFuncReturnType = nullptr;

}



void FunctionFlowPass::visitBlock(Block *Blk) {
    // Check if the block is safe
    bool isBlockBranchSafe = false;
    for (auto &Stmt : Blk->getStatements()) {
        visit(Stmt);

        // if there is a return statement, then the block is safe
        if (Stmt->getNodeType() == Block::N_AST_Return) {
            isBlockBranchSafe = true;
            break;
        }

        // if there is a conditional with an else block, then safety is determined by all the blocks
        if (Stmt->getNodeType() == Block::N_AST_ConditionalElse) {
            if (PM->getAnnotation<FunctionFlowPass>(Stmt)) {
                isBlockBranchSafe = true;
                break;
            }
        }

        // all other cases are not safe
    }
    PM->setAnnotation<FunctionFlowPass>(Blk, isBlockBranchSafe);

}

void FunctionFlowPass::visitConditionalElse(ConditionalElse *Cond) {

    // Check if the conditional is safe
    bool isConditionalBranchSafe = false;
    // TODO ensure AST builds both as blocks

    TreeNode *IfBlock = Cond->getStatement();
    visit(IfBlock);
    bool IfBlockSafe = PM->getAnnotation<FunctionFlowPass>(IfBlock);

    if (IfBlockSafe) {

        TreeNode *ElseBlock = Cond->getElseStatement();
        visit(ElseBlock);

        if (PM->getAnnotation<FunctionFlowPass>(ElseBlock)) {
            isConditionalBranchSafe = true;
        }

    }


    PM->setAnnotation<FunctionFlowPass>(Cond, isConditionalBranchSafe);

}

void FunctionFlowPass::visitConditional(Conditional *Cond) {
    // A plain conditional (just if) will never be safe
    visit(Cond->getStatement());
    PM->setAnnotation<FunctionFlowPass>(Cond, false);

}

void FunctionFlowPass::visitReturn(Return *Return) {
    auto *Expr = Return->getReturnExpr();
    Type *ExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Expr);

    if (currFuncReturnType == nullptr)
        currFuncReturnType = ExprTy;
    else {
        // TODO isComparableTo is like a type promotion check, but it doesn't worry about
        // direction of promotion. For example, int->real and real->int both yield real

        // Check if all return types are equivalent
        Type *NewExprTy = currFuncReturnType->isComparableTo(ExprTy);
        if (NewExprTy == nullptr)
            throw InvalidReturnTypeError(Return->getLine(), ExprTy->getName(), currFuncReturnType->getName());
    }

    // TODO move this to own pass to store two types of data
    PM->setAnnotation<FunctionFlowPass>(Return, inFunction);

}
