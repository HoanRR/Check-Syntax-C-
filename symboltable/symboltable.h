#pragma once
#include "type.h"
#include "..\lexer\Token.h"
#include "..\Trie\trie.h" 
#include <string>
#include <vector>
#include <unordered_map>
#include <memory> 
using namespace std;

struct str_Symbol
{
    string name;
    bool isFunction = false;
    TypeKind type = TypeKind::Unknown;
    Token tkn;
};

struct ScopeLayer 
{
    unordered_map<string, str_Symbol> symbols;
    unique_ptr<Trie> trie; 

    ScopeLayer();
};

class SymbolTable
{
public:
    vector<ScopeLayer> scopes;
    
    void enterScope();
    void leaveScope();
    bool declareSymbol(str_Symbol );
    str_Symbol *lookupSymbol(const string &);
    vector<string> getSuggestions(const string &);
};