//
// Created by 陈 on 2022-11-15.
//

#include <string>
#include "Passes/NullIdentityTypeCastPass.h"

void NullIdentityTypeCastPass::visitTypeCast(TypeCast *Cast) {
    auto Expr = Cast->getExpr();
    auto ExprTy = PM->getAnnotation<ExprTypeAnnotatorPass>(Cast->getExpr());

    // Don't care if the type we are casting isn't null or identity.
    if (!isa<NullTy>(ExprTy) && !isa<IdentityTy>(ExprTy))
        return;

    bool IsNull = isa<NullTy>(ExprTy);
    auto IsTupleTy = isa<TupleTy>(Cast->getTargetType());
    if (IsTupleTy) {
        auto TupleLit = PM->Builder.build<TupleLiteral>();

        auto TupleType = cast<TupleTy>(Cast->getTargetType());
        size_t NumOfMembers = TupleType->getNumOfMembers();
        for (size_t I = 0; I < NumOfMembers; I++) {
            auto MemberType = TupleType->getMemberTypeAt(I);
            TupleLit->setExprAtPos(
                    getScalarLiteral(MemberType->getKind(), IsNull), I);
        }
        Cast->getParent()->replaceChildWith(Cast, TupleLit);
        return;
    }
    // Scalar Types
    auto NewLit = getScalarLiteral(Cast->TargetType->getKind(), IsNull);
    Cast->getParent()->replaceChildWith(Cast, NewLit);
}

ASTNodeT *NullIdentityTypeCastPass::getScalarLiteral(Type::TypeKind Kind, bool IsNull) {
    switch(Kind) {
        case Type::T_Bool:
        {
            auto BoolLit = PM->Builder.build<BoolLiteral>();
            if (!IsNull)
                BoolLit->setTrue();
            return BoolLit;
        }
        case Type::T_Char:
        {
            auto CharLit = PM->Builder.build<CharLiteral>();
            IsNull ? CharLit->setCharacter("\0") : CharLit->setCharacter("\x01");
            return CharLit;
        }
        case Type::T_Int:
        {
            auto IntLit = PM->Builder.build<IntLiteral>();
            IntLit->setIntVal(!IsNull);
            return IntLit;
        }
        case Type::T_Real: {
            auto RealLit = PM->Builder.build<RealLiteral>();
            if (IsNull)
                RealLit->setVal("0.0");
            else
                RealLit->setVal("1.0");
            return RealLit;
        }
        default:
            assert(false);
    }
}
