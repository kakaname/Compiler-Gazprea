//
// Created by rajan on 11/07/22.
//

Type ExprTypeAnnotatorPass::visitOutStream(OutStreamExpr *Expr) {
    // TODO update to new type registry system
    // I am guessing what the interface looks like
    Type ExprTy = visit(Expr->getOutStreamExpr());
    // FIXME error handling
    assert(ExprTy.isOutputTy() && "Cannot output non-output type");

    if (ExprTy == NullType || ExprTy == IdentityType) {
        PM->setAnnotation<ExprTypeAnnotatorPass>(*Expr->getOutStreamExpr(), CharType);
    } else {
        // FIXME redundant?
        PM->setAnnotation<ExprTypeAnnotatorPass>(*Expr->getOutStreamExpr(), ExprTy);
    }

    return ExprTy;

}

Type ExprTypeAnnotatorPass::visitInStream(Identifier *Ident) {
    Type IdentTy = visit(Ident->getInStreamIdent());

    // We shouldn't need to set the type annotation here, as it should already
    // be set for IDs
    assert(IdentTy.isInputTy() && "Cannot input non-input type");
    return IdentTy;

}
