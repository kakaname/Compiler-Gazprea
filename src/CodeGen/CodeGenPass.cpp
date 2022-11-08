//
// Created by rajan on 11/07/22.
//

llvm::Value *CodeGenPass::visitOutStream(OutStream *Stream) {
    Value *ValToOut = visit(Stream->getOutStreamExpr());
    Type ValType = PM->getAnnotation<ExprTypeAnnotatorPass>(*Stream->getOutStreamExpr());
    // TODO depends on the TypeRegistry Implementation
    if (ValType == CharType) {
        // TODO enforce ValToOut = 0/1 for null and identity
        IR.CreateCall(PrintCharFunc, {ValToOut});
    } else if (ValType == IntegerType) {
        IR.CreateCall(PrintIntFunc, {ValToOut});
    } else if (ValType == RealType) {
        IR.CreateCall(PrintRealFunc, {ValToOut});
    } else if (ValType == BoolType) {
        IR.CreateCall(PrintBoolFunc, {ValToOut});
    } else {
        // TODO error handling
        assert(false && "Cannot output non-output type");
    }

}

llvm::Value *CodeGenPass::visitInStream(InStream *Stream) {
    Type IdentTy = PM->getAnnotation<ExprTypeAnnotatorPass>(*Stream->getInStreamIdent());
    Value *StoreLoc = SymbolMap[Stream->getInStreamIdent()->getSymbolId()];

    if (IdentTy == CharType) {
        IR.CreateCall(ReadCharFunc, {StoreLoc, StreamStateLoc, Buffer});
    } else if (IdentTy == IntegerType) {
        IR.CreateCall(ReadIntFunc, {StoreLoc, StreamStateLoc, Buffer});
    } else if (IdentTy == RealType) {
        IR.CreateCall(ReadRealFunc, {StoreLoc, StreamStateLoc, Buffer});
    } else if (IdentTy == BoolType) {
        IR.CreateCall(ReadBoolFunc, {StoreLoc, StreamStateLoc, Buffer});
    } else {
        assert(false && "Cannot input non-input type");
    }


}

llvm::Value *CodeGenPass::visitProcedureCall(Identifier *Ident, ArgsList *ArgsList) {

    // The stream state function is a special case
    // so it is handled here
    if (Ident->getName() == "stream_state")  {
        // load the stream state
        Value *StreamState = IR.CreateLoad(StreamStateLoc);
        // TODO setup return value
    }
}