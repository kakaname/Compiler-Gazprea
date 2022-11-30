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
#include <string>
#include <algorithm>


#include "llvm/Support/Casting.h"
#include "Types/Type.h"
#include "Types/ScalarTypes.h"
#include "Types/CompositeTypes.h"

using std::unique_ptr;
using std::make_unique;
using std::pair;
using std::vector;
using std::array;
using std::string;

using llvm::dyn_cast;
using llvm::isa;

class TypeRegistry {


    template<typename T>
    using ConstTypeIdPair = pair<bool, T>;

    using VectorTyId = ConstTypeIdPair<pair<const Type*, int>>;
    using MatrixTypeId = ConstTypeIdPair<pair<const Type*, pair<int, int>>>;
    using TupleTypeId = ConstTypeIdPair<pair<vector<const Type*>, map<string, int>>>;
    using FunctionTypeId = pair<vector<const Type*>, const Type*>;
    using ProcedureTypeId = pair<vector<const Type*>, const Type*>;

    // A size of -1 for sized types implies that the size is not known at
    // compile time.
    using VectorTyContainer = map<VectorTyId, unique_ptr<VectorTy>>;
    using MatrixTyContainer =map<MatrixTypeId, unique_ptr<MatrixTy>>;
    using TupleTyContainer = map<TupleTypeId, unique_ptr<TupleTy>>;
    using FunctionTypeContainer = map<FunctionTypeId, unique_ptr<FunctionTy>>;
    using ProcudureTypeContainer = map<ProcedureTypeId , unique_ptr<ProcedureTy>>;

    NullTy NullType;
    IdentityTy IdentityType;
    array<BoolTy, 2> BooleanTypes;
    array<CharTy, 2> CharacterTypes;
    array<IntegerTy, 2> IntegerTypes;
    array<RealTy, 2> RealTypes;
    array<IntervalTy, 2> IntervalTypes;

    VectorTyContainer VectorTypes;
    MatrixTyContainer MatrixTypes;
    TupleTyContainer TupleTypes;
    FunctionTypeContainer FunctionTypes;
    ProcudureTypeContainer ProcedureTypes;


public:
    explicit TypeRegistry(): NullType(), IdentityType(),
                             BooleanTypes{BoolTy(false), BoolTy(true)},
                             CharacterTypes{CharTy(false), CharTy(true)},
                             IntegerTypes{IntegerTy(false), IntegerTy(true)},
                             RealTypes{RealTy(false), RealTy(true)},
                             IntervalTypes{IntervalTy(false, 1), IntervalTy(true, 1)} {};

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

    const Type *getIntervalTy(bool Const = true) {
        return &IntervalTypes[Const];
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
                             const map<string, int>& Mappings,
                             bool IsConst = true) {
        TupleTypeId Key{IsConst,  pair{ContainedTypes, Mappings}};
        auto Res = TupleTypes.find(Key);
        if (Res != TupleTypes.end())
            return Res->second.get();

        auto NewTupleTy = make_unique<TupleTy>(TupleTy(
                IsConst, ContainedTypes, Mappings));
        auto Inserted = TupleTypes.insert({Key, std::move(NewTupleTy)});
        assert(Inserted.second && "We just checked that the type wasn't in the map");
        return Inserted.first->second.get();
    }

    const Type *getFunctionType(const FunctionTy::ParamTypeContainer& Args, const Type *RetTy) {
        pair Key{Args, RetTy};
        auto Res = FunctionTypes.find(Key);
        if (Res != FunctionTypes.end())
            return Res->second.get();

        auto NewFuncType = make_unique<FunctionTy>(FunctionTy(Args, RetTy));
        auto Inserted = FunctionTypes.insert({Key, std::move(NewFuncType)});
        assert(Inserted.second && "We just checked that the type wasn't in the map");
        return Inserted.first->second.get();
    }

    const Type *getProcedureType(const ProcedureTy::ArgsTypeContainer& Args, const Type *RetTy) {
        pair Key{Args, RetTy};
        auto Res = ProcedureTypes.find(Key);
        if (Res != ProcedureTypes.end())
            return Res->second.get();

        auto NewProcType = make_unique<ProcedureTy>(ProcedureTy(Args, RetTy));
        auto Inserted = ProcedureTypes.insert({Key, std::move(NewProcType)});
        assert(Inserted.second && "We just checked that the type wasn't in the map");
        return Inserted.first->second.get();
    }


    const Type *getConstTypeOf(const Type *Ty) {
        if (isa<NullTy>(Ty))
            return getNullTy();

        if(isa<IdentityTy>(Ty))
            return getIdentityTy();

        if (isa<IntegerTy>(Ty))
            return getIntegerTy(true);

        if (isa<BoolTy>(Ty))
            return getBooleanTy(true);

        if (isa<CharTy>(Ty))
            return getCharTy(true);

        if (isa<RealTy>(Ty))
            return getRealTy(true);

        if (isa<IntervalTy>(Ty))
            return getIntervalTy(true);

        if (auto *Vec = dyn_cast<VectorTy>(Ty))
            return getVectorType(Vec->getInnerTy(), Vec->getSize(), true);

        if (auto *Mat = dyn_cast<MatrixTy>(Ty))
            return getMatrixType(Mat->getInnerTy(), Mat->getNumOfRows(),
                                 Mat->getNumOfColumns(), true);

        if (auto *Tup = dyn_cast<TupleTy>(Ty)) {
            vector<const Type*> VarMembers;
            for (auto Mem: Tup->getMemberTypes())
                VarMembers.emplace_back(getConstTypeOf(Mem));
            return getTupleType(VarMembers, Tup->getMappings(), true);
        }

        assert(false && "Should not be reachable.");
    }

    const Type *getVarTypeOf(const Type *Ty) {
        if (isa<NullTy>(Ty))
            return getNullTy();

        if (isa<IdentityTy>(Ty))
            return getIdentityTy();

        if (isa<IntegerTy>(Ty))
            return getIntegerTy(false);

        if (isa<BoolTy>(Ty))
            return getBooleanTy(false);

        if (isa<CharTy>(Ty))
            return getCharTy(false);

        if (isa<RealTy>(Ty))
            return getRealTy(false);

        if (isa<IntervalTy>(Ty))
            return getIntervalTy(false);

        if (auto *Vec = dyn_cast<VectorTy>(Ty))
            return getVectorType(Vec->getInnerTy(), Vec->getSize(), false);

        if (auto *Mat = dyn_cast<MatrixTy>(Ty))
            return getMatrixType(Mat->getInnerTy(), Mat->getNumOfRows(),
                                 Mat->getNumOfColumns(), false);

        if (auto *Tup = dyn_cast<TupleTy>(Ty)) {
            vector<const Type*> VarMembers;
            for (auto Mem: Tup->getMemberTypes())
                VarMembers.emplace_back(getVarTypeOf(Mem));
            return getTupleType(VarMembers, Tup->getMappings(), false);
        }

        assert(false && "Should not be reachable.");
    }
};

#endif //GAZPREABASE_TYPEREGISTERY_H