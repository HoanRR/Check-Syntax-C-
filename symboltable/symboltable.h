#pragma once
#include "type.h"
#include "..\lexer\Token.h"
#include "..\trie.h" 
#include <string>
#include <vector>
#include <unordered_map>
#include <memory> 
#include <algorithm>
using namespace std;

struct str_Symbol
{
    string name;
    bool isFunction = false;
    TypeKind type = TypeKind::Unknown;
    Token tkn;
    // vector<TypeKind> params;
};

struct ScopeLayer {
    unordered_map<string, str_Symbol> symbols;
    unique_ptr<Trie> trie; 

    ScopeLayer() : trie(make_unique<Trie>()) {}
};

class SymbolTable
{
public:
    vector<ScopeLayer> scopes;
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

        if (top.symbols.find(sym.name) != top.symbols.end())
            return false;

        top.symbols.emplace(sym.name, sym);
        
        top.trie->insert(sym.name);
        
        return true;
    }
    str_Symbol *lookupSymbol(const string &name)
    {
        for (int i = scopes.size() - 1; i >= 0; i--)
        {
            auto it = scopes[i].symbols.find(name);
            if (it != scopes[i].symbols.end())
                return &it->second;
        }
        return nullptr;
    }

    vector<string> getSuggestions(const string &name)
    {
        vector<string> allSuggestions;
        
        for (int i = scopes.size() - 1; i >= 0; i--)
        {
            vector<string> scopeSuggestions = scopes[i].trie->findSimilarWords(name, 2); 
            allSuggestions.insert(allSuggestions.end(), scopeSuggestions.begin(), scopeSuggestions.end());
        }

        sort(allSuggestions.begin(), allSuggestions.end());
        allSuggestions.erase(unique(allSuggestions.begin(), allSuggestions.end()), allSuggestions.end());

        return allSuggestions;
    }
};
