//
// Created by dhanrajbir on 11/11/22.
//

#ifndef GAZPREABASE_SYMBOLTABLE_H
#define GAZPREABASE_SYMBOLTABLE_H

#include <map>
#include <memory>
#include <string>
#include <utility>
#include "Symbol.h"
#include "Passes/PassManagerResource.h"

using std::unique_ptr;
using std::make_unique;
using std::string;

class SymbolTable: public ResourceIdMixin<SymbolTable>{

    unsigned Id;

    template<typename T>
    using SymbolContainer = map<unsigned, unique_ptr<T>>;

    SymbolContainer<ObjectSymbol> Objects;
    SymbolContainer<ScopedSymbol> ScopedSyms;
    SymbolContainer<TypeSymbol> Types;

public:
    ObjectSymbol *defineObject(string Name, const Type *Ty) {
        auto NewObj = make_unique<ObjectSymbol>(ObjectSymbol(Ty, std::move(Name)));
        auto Inserted = Objects.insert({++Id, std::move(NewObj)});
        return Inserted.first->second.get();
    }

    TypeSymbol *defineTypeSymbol(string Name, const Type *Ty) {
        auto NewTySym = make_unique<TypeSymbol>(TypeSymbol(std::move(Name), Ty));
        auto Inserted = Types.insert({++Id, std::move(NewTySym)});
        return Inserted.first->second.get();
    }

    ScopedSymbol *defineScopedSymbol(string Name, const Type *Ty, map<string, int> Members) {
        auto NewObj = make_unique<ScopedSymbol>(ScopedSymbol(std::move(Name), Ty, std::move(Members)));
        auto Inserted = ScopedSyms.insert({++Id, std::move(NewObj)});
        return Inserted.first->second.get();
    }

    SymbolTable(): Id(0) {}
};

#endif //GAZPREABASE_SYMBOLTABLE_H
