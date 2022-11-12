//
// Created by dhanrajbir on 01/11/22.
//

#ifndef GAZPREABASE_TYPEREGISTERY_H
#define GAZPREABASE_TYPEREGISTERY_H

#include <map>
#include <memory>
#include <vector>
#include <array>
#include <cassert>

#include "Types/Type.h"
#include "Types/ScalarTypes.h"
#include "Types/CompositeTypes.h"

using std::unique_ptr;
using std::make_unique;
using std::pair;
using std::vector;
using std::array;

class TypeRegistry {


    template<typename T>
    using ConstTypeIdPair = pair<bool, T>;

    using VectorTyId = ConstTypeIdPair<pair<const Type*, int>>;
    using MatrixTypeId = ConstTypeIdPair<pair<const Type*, pair<int, int>>>;
    using TupleTypeId = ConstTypeIdPair<vector<const Type*>>;
    using FunctionTypeId = vector<Type*>;

    // A size of -1 for sized types implies that the size is not known at
    // compile time.
    using VectorTyContainer = map<VectorTyId, unique_ptr<VectorTy>>;
    using MatrixTyContainer =map<MatrixTypeId, unique_ptr<MatrixTy>>;
    using TupleTyContainer = map<TupleTypeId, unique_ptr<TupleTy>>;
    using FunctionTypeContainer = map<vector<Type*>, unique_ptr<FunctionTy>>;

    NullTy NullType;
    IdentityTy IdentityType;
    array<BoolTy, 2> BooleanTypes;
    array<CharTy, 2> CharacterTypes;
    array<IntegerTy, 2> IntegerTypes;
    array<RealTy, 2> RealTypes;

    VectorTyContainer VectorTypes;
    MatrixTyContainer MatrixTypes;
    TupleTyContainer TupleTypes;
    FunctionTypeContainer FunctionTypes;


public:
    explicit TypeRegistry(): NullType(), IdentityType(),
                             BooleanTypes{BoolTy(false), BoolTy(true)},
                             CharacterTypes{CharTy(false), CharTy(true)},
                             IntegerTypes{IntegerTy(false), IntegerTy(true)},
                             RealTypes{RealTy(false), RealTy(true)} {};

    const Type *getNullTy() {
        return &NullType;
    }

    const Type *getIdentityTy() {
        return &IdentityType;
    }

    const Type *getBooleanTy(bool Const = true) {
        return &BooleanTypes[Const];
    }

    const Type *getCharTy(bool Const = true) {
        return &CharacterTypes[Const];
    }

    const Type *getIntegerTy(bool Const = true) {
        return &IntegerTypes[Const];
    }

    const Type *getRealTy(bool Const = true) {
        return &RealTypes[Const];
    }

    const Type *getVectorType(const Type *InnerTy, int Size = -1, bool IsConst = true) {
        auto Res = VectorTypes.find({IsConst, {InnerTy, Size}});
        if (Res != VectorTypes.end())
            return Res->second.get();

        auto NewVecTy = make_unique<VectorTy>(VectorTy(InnerTy, Size, IsConst));
        VectorTyId Key{IsConst, {InnerTy, Size}};
        auto Inserted = VectorTypes.insert({Key, std::move(NewVecTy)});
        assert(Inserted.second && "We just checked that type wasn't in the map");
        return Inserted.first->second.get();
    }

    const Type *getMatrixType(const Type *InnerTy, int Rows = -1, int Cols = -1 ,
                              bool IsConst = true) {
        MatrixTypeId Key{IsConst, {InnerTy, pair{Rows, Cols}}};
        auto Res = MatrixTypes.find(Key);
        if (Res != MatrixTypes.end()) {
            return Res->second.get();
        }
        auto NewMatrixTy = make_unique<MatrixTy>(
                MatrixTy(InnerTy,{Rows, Cols}, IsConst));
        auto Inserted = MatrixTypes.insert({Key, std::move(NewMatrixTy)});
        assert(Inserted.second && "We just check that type wasn't in the map");
        return Inserted.first->second.get();
    }

    const Type *getTupleType(const TupleTy::MemberTyContainer &ContainedTypes,
                             bool IsConst = true) {
        TupleTypeId Key{IsConst,  ContainedTypes};
        auto Res = TupleTypes.find(Key);
        if (Res != TupleTypes.end())
            return Res->second.get();

        auto NewTupleTy = make_unique<TupleTy>(TupleTy(
                IsConst, ContainedTypes));
        auto Inserted = TupleTypes.insert({Key, std::move(NewTupleTy)});
        assert(Inserted.second && "We just checked that the type wasn't in the map");
        return Inserted.first->second.get();
    }
};

#endif //GAZPREABASE_TYPEREGISTERY_H
