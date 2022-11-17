//
// Created by youngwo2 on 05/11/22.
//

#ifndef GAZPREABASE_BADSTREAMPASS_H
#define GAZPREABASE_BADSTREAMPASS_H

#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/Support/raw_os_ostream.h"

#include "Passes/PassManager.h"
#include "Passes/VisitorPass.h"
#include "Passes/ExprTypeAnnotatorPass.h"
#include "Symbol/Symbol.h"


class BadStreamPass: public VisitorPass<BadStreamPass, void>  {
    using AnnotationT = TreeNode*;
    
    void visitOutStream(OutStream *OutStream);
    void visitInStream(InStream *InStream);

    void runOnAST(ASTPassManager &P, ASTNodeT *Root) {
        PM = &P;
        visit(Root);
    };
private:

    ASTPassManager *PM;

    BadStreamPass() : VisitorPass() {}
};
#endif //GAZPREABASE_BADSTREAMPASS_H 