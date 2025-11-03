#pragma once
#include "type.h"
#include "..\lexer\Token.h"
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;

struct str_Symbol
{
    string name;
    bool isFunction = false;
    TypeKind type = TypeKind::Unknown;
    Token tkn;
    // vector<TypeKind> params;
};

typedef unordered_map<string, str_Symbol> scope;

class SymbolTable
{
public:
    vector<scope> scopes;
    void enterScope()
    {
        scopes.emplace_back();
    }
    void leaveScope()
    {
        if (!scopes.empty())
            scopes.pop_back();
    }

    bool declareSymbol(str_Symbol sym)
    {
        if (scopes.empty())
            enterScope();
        auto &top = scopes.back();

        if (top.find(sym.name) != top.end())
            return false;

        top.emplace(sym.name, std::move(sym));
        return true;
    }
    str_Symbol *lookupSymbol(const string &name)
    {
        for (int i = scopes.size() - 1; i >= 0; i--)
        {
            auto it = scopes[i].find(name);
            if (it != scopes[i].end())
                return &it->second;
        }
        return nullptr;
    }
};
