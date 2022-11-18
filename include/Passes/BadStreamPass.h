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


struct BadStreamPass: public VisitorPass<BadStreamPass, void>  {
    static void visitOutStream(OutStream *OutStream);
    static void visitInStream(InStream *InStream);

    void runOnAST(ASTPassManager &P, ASTNodeT *Root) {
        PM = &P;
        visit(Root);
    };

private:
    ASTPassManager *PM;
};
#endif //GAZPREABASE_BADSTREAMPASS_H 