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

bool canPromoteIntervalTo(Type*);

bool isValidTupleCast(Type*, Type*);
bool isSameTupleTypeAs(Type*, Type*);
bool canPromoteTupleTo(Type*, Type*);
bool canPromoteIntegerTo(Type*);
bool canPromoteRealTo(Type*);
bool canPromoteVectorTo(Type*, Type*);
bool doesTupleSupportEq(Type*);
bool doesVectorSupportEq(Type*);
bool doesMatrixSupportEq(Type*);
bool doesVectorSupportArithOps(Type*);
bool doesMatrixSupportArithOps(Type*);
bool isVectorValidForComparisonOps(Type*);
bool isMatrixValidForComparisonOps(Type*);
bool isVectorValidForUnaryNot(Type*);
bool isMatrixValidForUnaryNot(Type*);

bool isVectorValidForUnaryAddSub(Type*);
bool isMatrixValidForUnaryAddSub(Type*);

bool isSameStringAs(Type*, Type*);
bool isSameFuncAs(Type*, Type*);
bool isSameProcAs(Type*, Type*);
bool isSameVectorAs(Type*, Type*);

bool canCastVectorTo(Type*, Type*);
bool canCastBoolCharIntTo(Type*);
bool canCastRealTo(Type*);
bool canCastIntervalTo(Type*);

Type *getPromotedScalarType(Type*, Type*);

string getStringTypeName(Type* Ty);
string getVectorTypeName(Type* Ty);
string getTupleTypeName(Type *Ty);
string getMatrixTypeName(Type *Ty);
string getFunctionTypeName(Type *Ty);
string getProcedureTypeName(Type *Ty);

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
        return T_Vector == Kind || T_Matrix == Kind || T_String == Kind;
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
        else if (Kind == T_Matrix) {
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
               T_Vector == Kind || T_Matrix == Kind ||
               T_String == Kind;
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
            case T_Interval:
                return canCastIntervalTo(T);
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
            case T_Interval:
                return canPromoteIntervalTo(T);
            default:
                return false;
        }
    }

     Type *getPromotedType( Type *T)  {
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