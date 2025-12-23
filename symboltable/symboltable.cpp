#include "symboltable.h"
#include <algorithm>

ScopeLayer::ScopeLayer() : trie(make_unique<Trie>()) {}

void SymbolTable::enterScope()
{
    scopes.emplace_back();
}

void SymbolTable::leaveScope()
{
    if (!scopes.empty())
        scopes.pop_back();
}

bool SymbolTable::declareSymbol(str_Symbol sym)
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

str_Symbol *SymbolTable::lookupSymbol(const string &name)
{
    for (int i = scopes.size() - 1; i >= 0; i--)
    {
        auto it = scopes[i].symbols.find(name);
        if (it != scopes[i].symbols.end())
            return &it->second;
    }
    return nullptr;
}

vector<string> SymbolTable::getSuggestions(const string &name)
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