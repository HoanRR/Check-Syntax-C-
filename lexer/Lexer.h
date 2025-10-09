#include <string>
#include "Token.h"
#include <vector>

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
    Token makeIdentifier();
    Token makeNumber();
    Token makeOperatorOrSymbol();

public:
    Lexer(const string &src);
    vector<Token> tokenize();
};