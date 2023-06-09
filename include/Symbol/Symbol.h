//
// Created by dhanrajbir on 11/11/22.
//

#ifndef GAZPREABASE_SYMBOL_H
#define GAZPREABASE_SYMBOL_H

#include <string>
#include <map>
#include <utility>

#include <stdexcept>

#include "Types/Type.h"

using std::string;
using std::map;

struct Symbol {
    enum SymbolKind {
        S_ObjectSymbol,
        S_TypeSymbol,
        S_ScopedSymbol
    };


    SymbolKind getKind() const {
        return Kind;
    }

    Symbol() = delete;

    explicit Symbol(SymbolKind Kind, string Name, Type* Ty):
        Kind(Kind), Name(std::move(Name)), Ty(Ty) {}

    const string &getName() const{
        return Name;
    }

    Type *getSymbolType() const {
        return Ty;
    }

private:
    SymbolKind Kind;
    string Name;
    Type* Ty;
};



class ObjectSymbol: public Symbol {
public:
    static bool classof(const Symbol *S) {
        return S->getKind() == Symbol::S_ObjectSymbol;
    }

    explicit ObjectSymbol(Type* Ty, string Name):
        Symbol(SymbolKind::S_ObjectSymbol, std::move(Name), Ty) {}
};

// tuple t;
//
// ...
// getRefer
// t.name
// t.1;

class TypeSymbol: public Symbol {
    Type *Ty;
public:
    static bool classof(const Symbol *S) {
        return S->getKind() == SymbolKind::S_TypeSymbol;
    }

    TypeSymbol(string Name, Type *Ty):
    Symbol(SymbolKind::S_TypeSymbol, std::move(Name), nullptr), Ty(Ty) {}

    Type *getType() const {
        return Ty;
    }
};

class ScopedSymbol: public Symbol {
    map<string, int> Members;
public:
    static bool classof(const Symbol *S) {
        return S->getKind() == SymbolKind::S_ScopedSymbol;
    }

    ScopedSymbol(string Name, Type *Ty, map<string, int> Members):
        Symbol(SymbolKind::S_ScopedSymbol, std::move(Name), Ty), Members(std::move(Members)) {}

    int getMemberIndex(const string& Member) const {
        auto Res = Members.find(Member);
        if (Res == Members.end())
            throw std::runtime_error("Tried to get a member that does not exist.");
        return Res->second;
    }
};

#endif //GAZPREABASE_SYMBOL_H
