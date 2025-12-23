#pragma once
#include "Token.h"

#include <vector>
#include <unordered_set>
#include <unordered_set>

class Lexer
{
private:
    const string &src;
    size_t i = 0;
    int line = 1, col = 1;

    char peek(int);
    char get();
    void skipSpacesAndComment();

    bool isIdentStart(char);
    bool isIdentCont(char);
    bool isOperatorChar(char);
    static bool isDigit(char );
    static bool isHexDigit(char );
    static bool isOctDigit(char );
    static bool isBinDigit(char );

    Token makeIdentifier();
    Token makeNumber();
    Token makeString();
    Token makeChar();
    Token makeOperatorOrSymbol();

public:
    Lexer(const string &src);
    vector<Token> tokenize();
};
