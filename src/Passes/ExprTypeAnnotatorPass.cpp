//
// Created by rajan on 11/07/22.
//

#include "Passes/ExprTypeAnnotatorPass.h"

Type* ExprTypeAnnotatorPass::visitOutStream(OutStream *OutStream) {
    // TODO update to new type registry system
    // I am guessing what the interface looks like
    Type* ExprTy = visit(OutStream->getOutStreamExpr());
    // FIXME error handling
    assert(ExprTy->isOutputTy() && "Cannot output non-output type");

    if (ExprTy == NullType || ExprTy == IdentityType) {
        PM->setAnnotation<ExprTypeAnnotatorPass>(*OutStream->getOutStreamExpr(), CharType);
    } else {
        // FIXME redundant?
        PM->setAnnotation<ExprTypeAnnotatorPass>(*OutStream->getOutStreamExpr(), ExprTy);
    }

    return ExprTy;

}

Type* ExprTypeAnnotatorPass::visitInStream(InStream *InStream) {
    Type* IdentTy = visit(InStream->getInStreamIdent());

    // We shouldn't need to set the type annotation here, as it should already
    // be set for IDs
    assert(IdentTy->isInputTy() && "Cannot input non-input type");
    return IdentTy;

}
