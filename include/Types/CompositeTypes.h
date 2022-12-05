//
// Created by dhanrajbir on 01/11/22.
//

#ifndef GAZPREABASE_COMPOSITETYPES_H
#define GAZPREABASE_COMPOSITETYPES_H

#include <utility>
#include <vector>
#include <map>
#include <string>

#include "Type.h"
#include "AST/ASTNodes.h"

using std::vector;
using std::map;
using std::string;
using std::pair;

struct IntervalTy : public Type {
    static bool classof(const Type *T) {
        return T->getKind() == TypeKind::T_Interval;
    }

    IntervalTy() = delete;

    explicit IntervalTy(int Length):
        Type(TypeKind::T_Interval, true), Length(Length) {}

    int isLengthKnown()  {
        return Length >= 0;
    }

private:
    int Length;
};

struct VectorTy : public Type {
    static bool classof(const Type *T) {
        return T->getKind() == TypeKind::T_Vector;
    }

    VectorTy() = delete;

    VectorTy( Type *InnerTy, int Size, bool IsConst):
        Type(TypeKind::T_Vector, IsConst), Size(Size), InnerTy(InnerTy) {}

    bool isSizeKnown() {
        return Size != -1;
    }

    int getSize() {
        return Size;
    }

     Type *getInnerTy() {
        return InnerTy;
    }

    ASTNodeT *getSizeExpr() {
        return SizeExpr;
    }

    void setSizeExpr(ASTNodeT *Expr) {
        SizeExpr = Expr;
    }

private:
    ASTNodeT *SizeExpr;
    int Size;
     Type *InnerTy;
};

struct MatrixTy : public Type {
    static bool classof(const Type *T) {
        return T->getKind() == TypeKind::T_Matrix;
    }

    MatrixTy() = delete;
    
    MatrixTy(Type *InnerTy, pair<int, int> Dimensions,  bool IsConst):
        Type(TypeKind::T_Matrix, IsConst),
        InnerTy(InnerTy),
        Dimensions(std::move(Dimensions)) {}

    int getNumOfRows()  {
        return Dimensions.first;
    };

    int getNumOfColumns()  {
        return Dimensions.second;
    };

    bool isNumOfRowsIsKnown()  {
        return Dimensions.first != -1;
    };

    bool isNumOfColumnsIsKnown()  {
        return Dimensions.second != -1;
    };

    bool isSizeKnown()  {
        return isNumOfColumnsIsKnown() && isNumOfRowsIsKnown();
    }

    bool isSameSizeAs(MatrixTy *Mat)  {
        return Mat->getNumOfColumns() == getNumOfColumns() &&
        Mat->getNumOfRows() == getNumOfRows();
    }

     Type *getInnerTy() {
        return InnerTy;
    }

    ASTNodeT *getRowSizeExpr() {
        return RowSizeExpr;
    }

    void setRowSizeExpr(ASTNodeT *Expr) {
        RowSizeExpr = Expr;
    }

    ASTNodeT *getColSizeExpr() {
        return ColSizeExpr;
    }

    void setColSizeExpr(ASTNodeT *Expr) {
        ColSizeExpr = Expr;
    }

private:
    ASTNodeT *RowSizeExpr;
    ASTNodeT *ColSizeExpr;
    Type *InnerTy;
    std::pair<int, int> Dimensions;
};

struct TupleTy : public Type {

    using MemberTyContainer = vector<Type*>;
    using MappingsContainer = map<string, int>;

    static bool classof(const Type *T) {
        return T->getKind() == TypeKind::T_Tuple;
    }

    TupleTy() = delete;

    Type *getMemberTypeAt(size_t Pos) {
        if (Pos >= ContainedTypes.size())
            return nullptr;
        return ContainedTypes.at(Pos);
    }

    size_t getNumOfMembers() {
        return ContainedTypes.size();
    }

    vector<Type *> &getMemberTypes() {
        return ContainedTypes;
    }

    int getMemberIdx(const string &Name)  {
        auto Res = Mappings.find(Name);
        if (Res == Mappings.end())
            return 0;
        return Res->second;
    }

     MappingsContainer &getMappings()  {
        return Mappings;
    }

    TupleTy(bool IsConst, MemberTyContainer ContainedTypes, MappingsContainer Mappings) :
        Type(TypeKind::T_Tuple, IsConst), ContainedTypes(std::move(ContainedTypes)),
        Mappings(std::move(Mappings)) {}

private:
    MemberTyContainer ContainedTypes;
    MappingsContainer Mappings;
};


struct FunctionTy : public Type {
    static bool classof(const Type *T) {
        return T->getKind() == TypeKind::T_Function;
    }

    using ParamTypeContainer = vector<Type*>;

     Type *getParamTypeAt(size_t Pos)  {
        if (Pos >= Params.size())
            return nullptr;
        return Params.at(Pos);
    }

     ParamTypeContainer &getParamTypes()  {
        return Params;
    }

    size_t getNumOfArgs()  {
        return Params.size();
    }

     Type *getRetType()  {
        return RetTy;
    }

    FunctionTy() = delete;

    explicit FunctionTy(ParamTypeContainer Args,  Type *RetTy) :
            Type(TypeKind::T_Function, true), Params(std::move(Args)), RetTy(RetTy) {}

private:
    ParamTypeContainer Params;
     Type *RetTy;
};

struct ProcedureTy : public Type {
    static bool classof(const Type *T) {
        return T->getKind() == TypeKind::T_Procedure;
    }

    using ArgsTypeContainer = vector<Type*>;

     Type *getParamTypeAt(size_t Pos)  {
        if (Pos >= Args.size())
            return nullptr;
        return Args.at(Pos);
    }

     ArgsTypeContainer &getParamTypes()  {
        return Args;
    }

     Type *getRetTy()  {
        return RetTy;
    };

    size_t getNumOfArgs()  {
        return Args.size();
    }

    ProcedureTy() = delete;

    explicit ProcedureTy(ArgsTypeContainer Args,  Type* RetTy):
            Type(TypeKind::T_Procedure, true), Args(std::move(Args)), RetTy(RetTy) {}

private:
    ArgsTypeContainer Args;
     Type *RetTy;
};

#endif //GAZPREABASE_COMPOSITETYPES_H
