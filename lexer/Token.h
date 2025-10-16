#pragma once
#include <string>
using namespace std;

enum TokenType
{
    Keyword,
    Identifier,
    Operator,
    Number,
    String,   // NEW
    Char,
    Symbol,
    End,
    Unknown,
    Error
};
class Token
{
public:
    TokenType type;
    string value;
    int line, col;
    int length;

    Token(string value, TokenType type, int line, int col, int len) : type(type), value(value), line(line), col(col), length(len) {}
};