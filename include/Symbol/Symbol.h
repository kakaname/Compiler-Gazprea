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

    explicit Symbol(SymbolKind Kind, string Name, const Type* Ty):
        Kind(Kind), Name(std::move(Name)), Ty(Ty) {}

    const string &getName() const{
        return Name;
    }

    const Type *getSymbolType() const {
        return Ty;
    }

private:
    SymbolKind Kind;
    string Name;
    const Type* Ty;
};



class ObjectSymbol: public Symbol {
public:
    static bool classof(const Symbol *S) {
        return S->getKind() == Symbol::S_ObjectSymbol;
    }

    explicit ObjectSymbol(const Type* Ty, string Name):
        Symbol(SymbolKind::S_ObjectSymbol, std::move(Name), Ty) {}
};


class TypeSymbol: public Symbol {
    const Type *Ty;
public:
    static bool classof(const Symbol *S) {
        return S->getKind() == SymbolKind::S_TypeSymbol;
    }

    TypeSymbol(string Name, const Type *Ty):
    Symbol(SymbolKind::S_TypeSymbol, std::move(Name), nullptr), Ty(Ty) {}

    const Type *getType() const {
        return Ty;
    }
};

class ScopedSymbol: public Symbol {
    map<string, int> Members;
public:
    static bool classof(const Symbol *S) {
        return S->getKind() == SymbolKind::S_ScopedSymbol;
    }

    ScopedSymbol(string Name, const Type *Ty, map<string, int> Members):
        Symbol(SymbolKind::S_ScopedSymbol, std::move(Name), Ty), Members(std::move(Members)) {}

    int getMemberIndex(const string& Member) {
        auto Res = Members.find(Member);
        if (Res == Members.end())
            throw std::runtime_error("Tried to get a member that does not exist.");
        return Res->second;
    }
};

#endif //GAZPREABASE_SYMBOL_H
