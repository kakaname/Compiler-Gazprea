//
// Created by dhanrajbir on 29/10/22.
//

#ifndef GAZPREABASE_TYPE_H
#define GAZPREABASE_TYPE_H

class TypeRegistry;

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

    bool isScalarTy() {
        return T_Bool == Kind || T_Char == Kind ||
        T_Int == Kind || T_Real == Kind;
    }

    bool isInputTy() {
        return T_Bool == Kind || T_Char == Kind ||
               T_Int == Kind || T_Real == Kind;
    }

    bool isOutputTy() {
        return T_Identity == Kind || T_Null == Kind ||
        T_Bool == Kind || T_Char == Kind ||
        T_Int == Kind || T_Real == Kind ||
        T_Interval == Kind || T_String == Kind ||
        T_Vector == Kind || T_Matrix == Kind;
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
