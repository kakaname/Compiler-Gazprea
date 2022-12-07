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
bool canPromoteIntegerTo(const Type*);
bool canPromoteRealTo(const Type*);
bool canPromoteVectorTo(const Type*, const Type*);
bool doesTupleSupportEq(const Type*);
bool doesVectorSupportEq(const Type*);
bool doesMatrixSupportEq(const Type*);
bool doesVectorSupportArithOps(const Type*);
bool doesMatrixSupportArithOps(const Type*);
bool isVectorValidForComparisonOps(const Type*);
bool isMatrixValidForComparisonOps(const Type*);
bool isVectorValidForUnaryNot(const Type*);
bool isMatrixValidForUnaryNot(const Type*);

bool isVectorValidForUnaryAddSub(const Type*);
bool isMatrixValidForUnaryAddSub(const Type*);

bool isSameFuncAs(const Type*, const Type*);
bool isSameProcAs(const Type*, const Type*);
bool isSameVectorAs(const Type*, const Type*);

bool canCastVectorTo(const Type*, const Type*);
bool canCastBoolCharIntTo(const Type*);
bool canCastRealTo(const Type*);

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

    bool isCompositeTy() const {
        return T_Vector == Kind || T_Matrix == Kind;
    }

    bool isSameTypeAs( Type *T) {
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

    bool isValidForArithOps()  {
        if (Kind == T_Vector) {
            return doesVectorSupportArithOps(this);
        } else if (Kind == T_Matrix) {
            return doesMatrixSupportArithOps(this);
        }
        return T_Int == Kind || T_Real == Kind || T_Interval == Kind;
    }

    bool isValidForComparisonOp()  {
        if (Kind == T_Vector)
            return isVectorValidForComparisonOps(this);
        } else if (Kind == T_Matrix) {
            return isMatrixValidForComparisonOps(this);
        }
        return T_Real == Kind || T_Int == Kind || T_Interval == Kind;
    }

    bool isValidForUnaryNot()  {
        switch (Kind) {
            case T_Bool:
                return true;
            case T_Vector:
                return isVectorValidForUnaryNot(this);
            case T_Matrix:
                return isMatrixValidForUnaryNot(this);
            default:
                return false;
        }
    }

    bool isValidForUnaryAddOrSub()  {
        switch (Kind) {
            case T_Real:
            case T_Int:
            case T_Interval:
                return true;
            case T_Vector:
                return isVectorValidForUnaryAddSub(this);
            case T_Matrix:
                return isMatrixValidForUnaryAddSub(this);
            default:
                return false;
        }
    }

    bool isValidForEq()  {
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

    bool isCallable()  {
        return T_Function == Kind || T_Procedure == Kind;
    }

    bool isOutputTy()  {
        return T_Identity == Kind || T_Null == Kind ||
               T_Bool == Kind || T_Char == Kind ||
               T_Int == Kind || T_Real == Kind ||
               T_Interval == Kind || T_String == Kind ||
               T_Vector == Kind || T_Matrix == Kind;
    }

    bool canCastTo( Type *T)  {
        switch (Kind) {
            case T_Bool:
            case T_Char:
            case T_Int:
                return canCastBoolCharIntTo(T);
            case T_Real:
                return canCastRealTo(T);
            case T_Tuple:
                return isValidTupleCast(this, T);
            case T_Vector:
                return canCastVectorTo(this, T);
            default:
                return false;
        }
    }

    bool canPromoteTo( Type *T)  {
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
                return canPromoteIntegerTo(T);
            case T_Real:
                return canPromoteRealTo(T);
            case T_Tuple:
                return canPromoteTupleTo(this, T);
            case T_Vector:
                return canPromoteVectorTo(this, T);
            default:
                return false;
        }
    }

     Type * getPromotedType( Type *T)  {
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
        return getPromotedScalarType(this, T);
    }

    bool isOpaqueTy() {
        return TypeKind::T_Null == Kind || TypeKind::T_Identity == Kind;
    }

    TypeKind getKind() const {
        return Kind;
    }

    bool isValidForBy()  {
        return T_Interval == Kind || T_Vector == Kind;
    }

    std::string getTypeName()  {
        std::string TypeName;
        if (isConst())
            TypeName += " ";
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