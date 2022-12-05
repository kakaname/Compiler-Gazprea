//
// Created by dhanrajbir on 29/10/22.
//

#ifndef GAZPREABASE_TYPE_H
#define GAZPREABASE_TYPE_H

#include <string>
#include "llvm/Support/Casting.h"

using llvm::cast;
using llvm::dyn_cast;

using std::string;

class Type;

bool isValidTupleCast(const Type*, const Type*);
bool isSameTupleTypeAs(const Type*, const Type*);
bool canPromoteTupleTo(const Type*, const Type*);
bool canPromoteVectorTo(const Type*, const Type*);
bool doesTupleSupportEq(const Type*);
bool doesVectorSupportEq(const Type*);
bool doesVectorSupportArithOps(const Type*);
bool doesVectorSupportComparisonOps(const Type*);
bool doesMatrixSupportEq(const Type*);
bool doesMatrixSupportArithOps(const Type*);
bool doesMatrixSupportComparisonOps(const Type*);
bool isSameFuncAs(const Type*, const Type*);
bool isSameProcAs(const Type*, const Type*);
bool isSameVectorAs(const Type*, const Type*);

const Type *getPromotedScalarType(const Type*, const Type*);

string getVectorTypeName(const Type* Ty);
string getTupleTypeName(const Type *Ty);
string getMatrixTypeName(const Type *Ty);
string getFunctionTypeName(const Type *Ty);
string getProcedureTypeName(const Type *Ty);

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
        switch (Kind) {
            case T_Int:
            case T_Bool:
            case T_Real:
            case T_Char:
            case T_Interval:
                return T->getKind() == Kind;
            case T_Tuple:
                return isSameTupleTypeAs(this, T);
            case T_Function:
                return isSameFuncAs(this, T);
            case T_Procedure:
                return isSameProcAs(this, T);
            case T_Vector:
                return isSameVectorAs(this, T);
            default:
                return T->getKind() == Kind;
        }
    }

    bool isValidForArithOps() const {
        if (Kind == T_Vector) {
            return doesVectorSupportArithOps(this);
        } else if (Kind == T_Matrix) {
            return doesMatrixSupportArithOps(this);
        }
        return T_Int == Kind || T_Real == Kind || T_Interval == Kind;
    }

    bool isValidForComparisonOp() const {
        if (Kind == T_Vector) {
            return doesVectorSupportComparisonOps(this);
        } else if (Kind == T_Matrix) {
            return doesMatrixSupportComparisonOps(this);
        }
        return T_Real == Kind || T_Int == Kind || T_Interval == Kind;
    }

    bool isValidForUnaryNot() const {
        return T_Bool == Kind;
    }

    bool isValidForUnaryAddOrSub() const {
        return T_Real == Kind || T_Int == Kind || T_Interval == Kind;
    }

    bool isValidForEq() const {
        switch (Kind) {
            case T_Real:
            case T_Int:
            case T_Bool:
            case T_Interval:
                return true;
            case T_Tuple:
                return doesTupleSupportEq(this);
            case T_Vector:
                return doesVectorSupportEq(this);
            case T_Matrix:
                return doesMatrixSupportEq(this);
            default:
                return T_Real == Kind || T_Int == Kind || T_Bool == Kind || T_Interval == Kind;
        }
    }

    bool isInputTy() const {
        return T_Bool == Kind || T_Char == Kind ||
               T_Int == Kind || T_Real == Kind;
    }

    bool isCallable() const {
        return T_Function == Kind || T_Procedure == Kind;
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
            default:
                return false;
        }
    }

    bool canPromoteTo(const Type *T) const {
        if (isSameTypeAs(T))
            return true;

        // To cover the case where the base is a scalar and the promoted type is a vector
        if (T->getKind() == T_Vector)
            return canPromoteVectorTo(this, T);

        switch (Kind) {
            case T_Null:
            case T_Identity:
                return true;
            case T_Int:
                return T->getKind() == T_Real;
            case T_Tuple:
                return canPromoteTupleTo(this, T);
            case T_Vector:
                return canPromoteVectorTo(this, T);
            default:
                return false;
        }
    }

    // This function returns the promoted type of values within a matrix, which
    // already assumes at least one of the two inputs are matrices. This is due
    // to the fact that the matrix type has a unique vector promotion rule that
    // does not follow normal vector promotion rules. (size mismatches are allowed)
//    const Type *getPromotedMatrixType(const Type *T) const {
//        assert(T->getKind() == T_Vector || this->getKind() == T_Vector);
//
//        if (T->getKind() != T_Vector)
//            return this;
//        if (this->getKind() != T_Vector)
//            return T;
//
//        return getPromotedVectorInMatrixType(this, T);
//
//    }

    const Type * getPromotedType(const Type *T) const {
        return getPromotedScalarType(this, T);
    }

    bool isOpaqueTy() const {
        return TypeKind::T_Null == Kind || TypeKind::T_Identity == Kind;
    }

    TypeKind getKind() const {
        return Kind;
    }

    std::string getTypeName() const {
        std::string TypeName;
        if (isConst())
            TypeName += "const ";
        switch (Kind) {
            case T_Identity:
                return TypeName + "identity";
            case T_Null:
                return TypeName + "null";
            case T_Bool:
                return TypeName + "bool";
            case T_Char:
                return TypeName + "char";
            case T_Int:
                return TypeName + "int";
            case T_Real:
                return TypeName + "real";
            case T_Tuple:
                return TypeName + getTupleTypeName(this);
            case T_Function:
                return getFunctionTypeName(this);
            case T_Procedure:
                return getProcedureTypeName(this);
            case T_Vector:
                return getVectorTypeName(this);
            case T_Matrix:
                return getMatrixTypeName(this);
            case T_Interval:
                return TypeName + "interval";
            default:
                assert(false);
        }
    }

    Type() = delete;

protected:
    TypeKind Kind;
    bool IsConstTy;

    Type(TypeKind Kind, bool IsConst) : Kind(Kind), IsConstTy(IsConst) {}
};




#endif //GAZPREABASE_TYPE_H