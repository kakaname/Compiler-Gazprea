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

    int isLengthKnown() const {
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

    VectorTy(const Type *InnerTy, int Size, bool IsConst):
        Type(TypeKind::T_Vector, IsConst), Size(Size), InnerTy(InnerTy) {}

    bool isSizeKnown() const {
        return Size != -1;
    }

    int getSize() const {
        return Size;
    }

    const Type *getInnerTy() const {
        return InnerTy;
    }

private:
    int Size;
    const Type *InnerTy;
};

struct StringTy : public Type {
    static bool classof(const Type *T) {
        return T->getKind() == TypeKind::T_String;
    }

    StringTy() = delete;

    StringTy(const Type *InnerTy, int Size, bool IsConst):
        Type(TypeKind::T_String, IsConst), Size(Size), InnerTy(InnerTy) {}

    bool isSizeKnown() const {
        return Size != -1;
    }

    int getSize() const {
        return Size;
    }

    const Type *getInnerTy() const {
        return InnerTy;
    }

private:
    int Size;
    const Type *InnerTy;
};

struct MatrixTy : public Type {
    static bool classof(const Type *T) {
        return T->getKind() == TypeKind::T_Matrix;
    }

    MatrixTy() = delete;
    
    MatrixTy(const Type *InnerTy, pair<int, int> Dimensions,  bool IsConst):
        Type(TypeKind::T_Matrix, IsConst),
        InnerTy(InnerTy),
        Dimensions(std::move(Dimensions)) {}

    int getNumOfRows() const {
        return Dimensions.first;
    };

    int getNumOfColumns() const {
        return Dimensions.second;
    };

    bool isNumOfRowsIsKnown() const {
        return Dimensions.first != -1;
    };

    bool isNumOfColumnsIsKnown() const {
        return Dimensions.second != -1;
    };

    bool isSizeKnown() const {
        return isNumOfColumnsIsKnown() && isNumOfRowsIsKnown();
    }

    bool isSameSizeAs(const MatrixTy *Mat) const {
        return Mat->getNumOfColumns() == getNumOfColumns() &&
        Mat->getNumOfRows() == getNumOfRows();
    }

    const Type *getInnerTy() const {
        return InnerTy;
    }

private:
    const Type *InnerTy;
    std::pair<int, int> Dimensions;
};

struct TupleTy : public Type {

    using MemberTyContainer = vector<const Type*>;
    using MappingsContainer = map<string, int>;

    static bool classof(const Type *T) {
        return T->getKind() == TypeKind::T_Tuple;
    }

    TupleTy() = delete;

    const Type *getMemberTypeAt(size_t Pos) const {
        if (Pos >= ContainedTypes.size())
            return nullptr;
        return ContainedTypes.at(Pos);
    }

    size_t getNumOfMembers() const {
        return ContainedTypes.size();
    }

    const vector<const Type *> &getMemberTypes() const {
        return ContainedTypes;
    }

    int getMemberIdx(const string &Name) const {
        auto Res = Mappings.find(Name);
        if (Res == Mappings.end())
            return 0;
        return Res->second;
    }

    const MappingsContainer &getMappings() const {
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

    using ParamTypeContainer = vector<const Type*>;

    const Type *getParamTypeAt(size_t Pos) const {
        if (Pos >= Params.size())
            return nullptr;
        return Params.at(Pos);
    }

    const ParamTypeContainer &getParamTypes() const {
        return Params;
    }

    size_t getNumOfArgs() const {
        return Params.size();
    }

    const Type *getRetType() const {
        return RetTy;
    }

    FunctionTy() = delete;

    explicit FunctionTy(ParamTypeContainer Args, const Type *RetTy) :
            Type(TypeKind::T_Function, true), Params(std::move(Args)), RetTy(RetTy) {}

private:
    ParamTypeContainer Params;
    const Type *RetTy;
};

struct ProcedureTy : public Type {
    static bool classof(const Type *T) {
        return T->getKind() == TypeKind::T_Procedure;
    }

    using ArgsTypeContainer = vector<const Type*>;

    const Type *getParamTypeAt(size_t Pos) const {
        if (Pos >= Args.size())
            return nullptr;
        return Args.at(Pos);
    }

    const ArgsTypeContainer &getParamTypes() const {
        return Args;
    }

    const Type *getRetTy() const {
        return RetTy;
    };

    size_t getNumOfArgs() const {
        return Args.size();
    }

    ProcedureTy() = delete;

    explicit ProcedureTy(ArgsTypeContainer Args, const Type* RetTy):
            Type(TypeKind::T_Procedure, true), Args(std::move(Args)), RetTy(RetTy) {}

private:
    ArgsTypeContainer Args;
    const Type *RetTy;
};

#endif //GAZPREABASE_COMPOSITETYPES_H
