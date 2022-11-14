//
// Created by rajan on 11/11/22.
//


#pragma once

/**
* This pass verifies that a function or procedure reaches a return statement
* properly. It also determines which function is returned from any given return statement.
 * Note that this must be run after ExprTypeAnnotationPass but before the DefinitionPass.
 * The pass will also ensure all returns within a function are of the same type and
 * set the type of the whole function.
 *
 * The grammar ensures that a function or procedure can only occur at the top level, which makes
 * our job much easier to determine if we are inside a function at any time;
*/

// Conditions for returns in branches:
// A function is branch safe if the upper-most body is safe.
//
// Main block: if there is a return statement in the main function body, then it is safe.
// Any block: if there is a return in any block, then it is safe
// If / else if / else block: if all blocks are safe, then its containing block is safe
// While / do while / if / else if: these blocks are never safe as they could potentially never run
//
// This methodology also asserts that there is a return value for each block

#include "Passes/Pass.h"
#include "Passes/VisitorPass.h"
#include "ErrorHandling/exceptions.h"

struct FunctionFlowPass : public VisitorPass<FunctionFlowPass, bool> {
//    // The annotation determines if a branch is safe or not
//    // could be substituted with return type
//    using AnnotationT = bool;
//
////    using ReturnT
//    FunctionDef* inFunction = nullptr;
//    Type *currFuncReturnType;
//
//    void visitFunctionDef(FunctionDef *FuncDef);
//    void visitBlock

//    void runOnAST(ASTPassManager &PManager, ASTNodeT &Root) {
//        PM = &PManager;
//        visit(&Root);
//    }



//    ASTPassManager *PM;
//
//    FunctionFlowPass() : VisitorPass() {}
};
