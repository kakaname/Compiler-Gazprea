//
// Created by dhanrajbir on 08/10/22.
//

#ifndef VCALCBASE_SYMBOLTABLE_H
#define VCALCBASE_SYMBOLTABLE_H

#include <map>
#include <memory>
#include <stdexcept>
#include <string>
#include "Types/Type.h"

using std::map;
using std::unique_ptr;
using std::make_unique;
using std::string;

class SymbolTable;


// Each symbol represents an l-value in the program. Note that these symbols
// are not the same as are found in the data section of a binary file but
// instead these are essentially variables that are defined any where in the
// program.
//
// Each symbol has an associated type and an id that uniquely identifies the
// symbol. The primary purpose of the ID is to help in scope resolution.
// Different scopes can have identifiers that have the same name, but each of
// them must be backed by a different Symbol. New symbols are created through
// the SymbolBuilder which ensures each symbol has a unique ID.
//
// The scope resolution problem is hence solved by assigning each identifier,
// the symbol ID it is backed by.
class Symbol {
    friend SymbolTable;
    const unsigned Id;
    TypeId SymbolTypeId;

    explicit Symbol(unsigned SymbolId, TypeId TyId) : Id(SymbolId), SymbolTypeId(TyId) {};

public:
    // Only the symbol table should create new Symbols.
    Symbol() = delete;

    unsigned getId() const {
        return Id;
    }

    TypeId getTypeId() {
        return SymbolTypeId;
    }

    Symbol(Symbol &&) = default;
};


// The SymbolTable constructs and holds all symbols (aka things that back
// identifiers).
class SymbolTable {
    using SymbolMapT = map<unsigned, std::unique_ptr<Symbol>>;
    SymbolMapT Symbols;

    using TypeMapT = map<unsigned, unique_ptr<Type>>;
    TypeMapT Types;

    inline static unsigned CurrentId = 0;

public:
    unsigned declareSymbol(TypeId TyId) {
        auto NewSymbol = Symbol(++CurrentId, TyId);
        Symbols[NewSymbol.Id] = make_unique<Symbol>(std::forward<Symbol>(NewSymbol));
        return CurrentId;
    }

    Symbol *getSymbolById(unsigned Id) {
        auto Res = Symbols.find(Id);
        // If the symbol table has no symbol with the given ID return null.
        if (Res == Symbols.end())
            return nullptr;
        return Res->second.get();
    }

    unsigned declareType(Type &Ty) {
        Types[++CurrentId] = make_unique<Type>(Ty);
        return CurrentId;
    }

    Type *getType(unsigned Id) {
        auto Res = Types.find(Id);
        // If the symbol table has no type with the given ID return null.
        if (Res == Types.end())
            return nullptr;
        return Res->second.get();
    }

    SymbolTable() = default;
    SymbolTable(SymbolTable &&) = default;
};

#endif //VCALCBASE_SYMBOLTABLE_H
