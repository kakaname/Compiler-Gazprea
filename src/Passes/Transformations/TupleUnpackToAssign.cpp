
#include "Passes/Transformations/TupleUnpackToAssign.h"

void TupleUnpackToAssignTransformationPass::visitAssignment(Assignment *Assign) {

    // Only run on tuple unpacking assignments
    if (!isa<TupleDestruct>(Assign->getAssignedTo()))
        return;

    auto TD = dyn_cast<TupleDestruct>(Assign->getAssignedTo());
    auto Ty = PM->getAnnotation<ExprTypeAnnotatorPass>(Assign->getExpr());
    auto TupleType = dyn_cast<TupleTy>(Ty);

    if (!TupleType)
        throw std::runtime_error("Tuple unpacking assignment must have a tuple on the right hand side");
    
    if (TD->numOfChildren() != TupleType->getNumOfMembers())
        throw std::runtime_error("Tuple unpacking assignment must have the same number of members on the left and right hand side");

    // Create a new block for the tuple unpacking
    auto Blk = PM->Builder.build<Block>();
    Blk->copyCtx(Assign);
    Blk->setParent(Assign->getParent());

    // Assign the tuple to a temporary variable
    auto TempVar = PM->Builder.build<Identifier>();
    TempVar->copyCtx(Assign);
    TempVar->setName("__temp_tuple_unpack");
    TempVar->setIdentType(TupleType);
    auto Sym = PM->SymTable.defineObject("", TupleType);
    TempVar->setReferred(Sym);

    auto TempVarAssign = PM->Builder.build<Declaration>();
    TempVarAssign->copyCtx(Assign);
    TempVarAssign->setIdent(TempVar);
    TempVarAssign->setIdentType(TupleType);
    TempVarAssign->setInitExpr(Assign->getExpr());
    TempVarAssign->setParent(Blk);
    TempVar->setParent(TempVarAssign);
    Blk->addChild(TempVarAssign);

    // Create a new assignment for each tuple member
    for (int I = 0; I < TD->numOfChildren(); I++) {
        auto MemberAssign = PM->Builder.build<Assignment>();
        MemberAssign->copyCtx(Assign);
        MemberAssign->setParent(Blk);
        MemberAssign->setAssignedTo(TD->getChildAt(I));
        MemberAssign->setExpr(buildMemberAccess(TempVar, I+1));
        Blk->addChild(MemberAssign);
    }

    // Replace the original assignment with the new block
    Assign->getParent()->replaceChildWith(Assign, Blk);

    // Visit the new block, there should be no need to though
    visit(Blk);

}

MemberAccess *TupleUnpackToAssignTransformationPass::buildMemberAccess(ASTNodeT *BaseExpr, int Idx) const {
    auto IntLit = PM->Builder.build<IntLiteral>();
    IntLit->copyCtx(BaseExpr);
    IntLit->setIntVal(Idx);

    auto Access = PM->Builder.build<MemberAccess>();
    Access->copyCtx(BaseExpr);
    Access->setExpr(BaseExpr);
    Access->setMemberExpr(IntLit);

    return Access;
}

