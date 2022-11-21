//
// Created by dhanrajbir on 19/11/22.
//

#ifndef GAZPREABASE_CASTOPERATION_H
#define GAZPREABASE_CASTOPERATION_H

#include "llvm/IR/Value.h"
#include "llvm/IR/IRBuilder.h"

#include "Types/Type.h"


template<Type::TypeKind DestT>
struct CastOperation {
    static llvm::Value *doCast(llvm::IRBuilder<> &IR, llvm::Value *Val, const Type *SrcTy) {
        throw std::runtime_error("Invalid cast");        
    }
};

template<> struct CastOperation<Type::TypeKind::T_Char> {
    static llvm::Value *doCast(llvm::IRBuilder<> &IR, llvm::Value *Val, const Type *SrcTy) {
        switch (SrcTy->getKind()) {
            case Type::T_Int:
                return IR.CreateTrunc(Val, IR.getInt8Ty());
            case Type::T_Bool:
                return IR.CreateZExt(Val, IR.getInt8Ty());
            default:
                assert("Tried to cast some type to char that can't be"
                       " casted to char" && false);
        }
    }
};

template<> struct CastOperation<Type::TypeKind::T_Int> {
    static llvm::Value *doCast(llvm::IRBuilder<> &IR, llvm::Value *Val, const Type *SrcTy) {
        switch (SrcTy->getKind()) {
            case Type::TypeKind::T_Char:
            case Type::TypeKind::T_Bool:
                return IR.CreateZExt(Val, IR.getInt32Ty());
            case Type::TypeKind::T_Real:
                return IR.CreateFPToSI(Val, IR.getInt32Ty());
            default:
                assert("Tried to cast some type to integer that can't be"
                       " casted to integer");
        }
    }
};

template<> struct CastOperation<Type::TypeKind::T_Real> {
    static llvm::Value *doCast(llvm::IRBuilder<> &IR, llvm::Value *Val, const Type *SrcTy) {
        switch (SrcTy->getKind()) {
            case Type::TypeKind::T_Int:
                return IR.CreateSIToFP(Val, IR.getFloatTy());
            case Type::TypeKind::T_Char:
            case Type::TypeKind::T_Bool:
                return IR.CreateUIToFP(Val, IR.getFloatTy());
            default:
                assert("Tried to cast some type to real that can't be"
                       " casted to real");
        }
    }
};

#endif //GAZPREABASE_CASTOPERATION_H
