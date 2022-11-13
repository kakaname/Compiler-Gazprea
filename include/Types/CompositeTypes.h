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

    explicit IntervalTy(bool IsConst, int StepSize):
        Type(TypeKind::T_Interval, IsConst), StepSize(1) {
        StepSizeKnown = (StepSize > 0);
    }

    int getStepSize() const {
        return StepSize;
    }

    bool isStepSizeKnown() const {
        return StepSizeKnown;
    }

private:
    bool StepSizeKnown;
    int StepSize;
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

    const Type *getInnerTy() const {
        return InnerTy;
    }

private:
    const Type *InnerTy;
    std::pair<int, int> Dimensions;
};

struct TupleTy : public Type {

    using MemberTyContainer = vector<const Type*>;

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

    TupleTy(bool IsConst, MemberTyContainer ContainedTypes) :
        Type(TypeKind::T_Tuple, IsConst), ContainedTypes(std::move(ContainedTypes)) {}

private:
    MemberTyContainer ContainedTypes;
};


struct FunctionTy : public Type {
    static bool classof(const Type *T) {
        return T->getKind() == TypeKind::T_Function;
    }

    using ArgsTypeContainer = vector<const Type*>;

    const Type *getArgTypeAt(size_t Pos) {
        if (Pos >= Args.size())
            return nullptr;
        return Args.at(Pos);
    }

    size_t getNumOfArgs() {
        return Args.size();
    }

    const Type *getRetType() const {
        return RetTy;
    }

    FunctionTy() = delete;

    explicit FunctionTy(ArgsTypeContainer Args, const Type *RetTy) :
        Type(TypeKind::T_Function, true), Args(std::move(Args)), RetTy(RetTy) {}

private:
    ArgsTypeContainer Args;
    const Type *RetTy;
};

struct ProcedureTy : public Type {
    static bool classof(const Type *T) {
        return T->getKind() == TypeKind::T_Procedure;
    }

    using ArgsTypeContainer = vector<Type*>;

    const Type *getArgTypeAt(size_t Pos) {
        if (Pos >= Args.size())
            return nullptr;
        return Args.at(Pos);
    }

    size_t getNumOfArgs() {
        return Args.size();
    }

    ProcedureTy() = delete;

    explicit ProcedureTy(ArgsTypeContainer Args, const Type* ReturnTy):
            Type(TypeKind::T_Function, true), Args(std::move(Args)) {}

private:
    ArgsTypeContainer Args;
};

#endif //GAZPREABASE_COMPOSITETYPES_H
