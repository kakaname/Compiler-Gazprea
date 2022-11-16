//
// Created by 陈 on 2022-11-15.
//

#include <string>
#include "Passes/NullIdentityTypeCastPass.h"

void NullIdentityTypeCastPass::visitTypeCast(TypeCast *Cast) {
    auto Expr = Cast->getExpr();
    if (dyn_cast<NullLiteral>(Expr) || dyn_cast<IdentityLiteral>(Expr)) {
        bool IsNull = dyn_cast<NullLiteral>(Expr);
        Type::TypeKind Kind = Cast->getTargetType()->getKind();
        if (Kind == Type::T_Tuple) {
            auto TupleLit = PM->Builder.build<TupleLiteral>();

            auto TupleType = cast<TupleTy>(Cast->getTargetType());
            int NumOfMembers = TupleType->getNumOfMembers();
            for (size_t I = 0; I < NumOfMembers; I++) {
                auto MemberType = TupleType->getMemberTypeAt(I);
                TupleLit->setExprAtPos(getScalarLiteral(MemberType->getKind(), IsNull));
            }
            Cast->setExpr(TupleLit);
        }
        else {
            // Scalar Types
            auto NewLit = getScalarLiteral(Kind, IsNull);
            Cast->setExpr(NewLit);
        }
    }
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
            if (IsNull)
                CharLit->setCharacter("\0");
            else
                CharLit->setCharacter("\x01");
            return CharLit;
        }
        case Type::T_Int:
        {
            auto IntLit = PM->Builder.build<IntLiteral>();
            if (IsNull)
                IntLit->setVal("0");
            else
                IntLit->setVal("1");
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
