//
// Created by dhanrajbir on 29/10/22.
//

#ifndef GAZPREABASE_TYPE_H
#define GAZPREABASE_TYPE_H

#include "llvm/Support/Casting.h"

using llvm::cast;
using llvm::dyn_cast;

class Type;

bool isValidTupleCast(const Type *, const Type *);

class Type {
public:
    enum TypeKind {
        T_Identity,
        T_Null,
        T_Bool,
        T_Char,
        T_Int,
        T_Real,
        T_Interval,
        T_String,
        T_Vector,
        T_Matrix,
        T_Tuple,
        T_Function,
        T_Procedure
    };

    bool isConst() const {
        return IsConstTy;
    }

    bool isScalarTy() const {
        return T_Bool == Kind || T_Char == Kind ||
               T_Int == Kind || T_Real == Kind;
    }

    bool isSameTypeAs(const Type *T) const {
        return T->getKind() == Kind;
    }

    bool isValidForArithOps() const {
        return T_Int == Kind || T_Real == Kind;
    }

    bool isValidForComparisonOp() const {
        return T_Real == Kind || T_Int == Kind;
    }

    bool isValidForUnaryNot() const {
        return T_Bool == Kind;
    }

    bool isValidForUnaryAddOrSub() const {
        return T_Real == Kind || T_Int == Kind;
    }

    bool isValidForEq() const {
        return T_Real == Kind || T_Int == Kind || T_Bool == Kind;
    }

    bool isInputTy() const {
        return T_Bool == Kind || T_Char == Kind ||
               T_Int == Kind || T_Real == Kind;
    }

    bool isOutputTy() const {
        return T_Identity == Kind || T_Null == Kind ||
               T_Bool == Kind || T_Char == Kind ||
               T_Int == Kind || T_Real == Kind ||
               T_Interval == Kind || T_String == Kind ||
               T_Vector == Kind || T_Matrix == Kind;
    }

    bool canCastTo(const Type *T) const {

        TypeKind Ty = T->getKind();
        switch (Kind) {
            case T_Bool:
            case T_Char:
            case T_Int:
                return Ty == T_Bool || Ty == T_Char || Ty == T_Int || Ty == T_Real;
            case T_Real:
                return Ty == T_Int || Ty == T_Real;
            case T_Tuple:
                return isValidTupleCast(this, T);
        }
    }

    TypeKind getKind() const {
        return Kind;
    }

    Type() = delete;

protected:
    TypeKind Kind;
    bool IsConstTy;

    Type(TypeKind Kind, bool IsConst) : Kind(Kind), IsConstTy(IsConst) {}
};




#endif //GAZPREABASE_TYPE_H