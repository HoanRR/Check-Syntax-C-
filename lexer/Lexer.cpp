#include "Lexer.h"
#include <unordered_set>
char Lexer::peek(int k = 0)
{
    return (i + k < src.size()) ? src[i + k] : '\0';
}

char Lexer::get()
{
    char c = peek();
    if (c == '\n')
    {
        line++;
        col = 1;
    }
    else
        col++;
    i++;
    return c;
}

void Lexer::skipSpacesAndComment()
{
    while (true)
    {

        // dấu cách
        if (isspace((unsigned char)peek()))
        {
            get();
            continue;
        }
        // Comment //
        if (peek() == '/' && peek(1) == '/')
        {
            while (peek() && get() != '\n')
                ;
            continue;
        }

        // Coment /* */
        if (peek() == '/' && peek(1) == '*')
        {
            get();
            get();

            while (peek() && !(peek() == '*' && peek(1) == '/'))
                get();
            if (peek())
            {
                get();
                get();
            }
            continue;
        }
        break;
    }
}
bool Lexer::isIdentStart(char c)
{
    return isalpha((unsigned char)c) || c == '_';
}
bool Lexer::isIdentCont(char c)
{
    return std::isalnum((unsigned char)c) || c == '_';
}
bool Lexer::isOperatorChar(char c)
{
    static const string ops = "+-*/%=!<>&|";
    return ops.find(c) != string::npos;
}

Token Lexer::makeIdentifier()
{
    int L = line, C = col;
    string s;

    while (isIdentCont((peek())))
        s.push_back(get());

    unordered_set<string> keywords = {
        "true", "false", "private", "protected", "public", "try", "catch", "dynamic_cash", "reinterpret_cast", "static_cast", "const_cast", "throw", "explicit", "new", "this", "asm", "operator", "namespace", "typeid", "typename", "class", "friend", "template", "using", "virtual", "delete", "inline", "mutable", "wchar_t", "bool", "And", "bitor", "not_eq", "xor", "and_eq", "compl", "or", "xor_eq", "bitand", "not", "or_eq", "export", "explicit", "auto", "double", "int", "struct", "break", "else", "long", "switch", "case", "enum", "register", "typedef", "char", "extern", "return", "union", "const", "float", "short", "unsigned", "continue", "for", "signed", "void", "default", "goto", "sizeof", "volatile", "do", "if", "static", "while"};

    TokenType T = Identifier;
    for (auto &kw : keywords)
        if (s == kw)
        {
            T = Keyword;
        }
    return Token(s, T, L, C);
}

Token Lexer::makeNumber()
{
    int L = line, C = col;
    string s;

    while (isdigit(static_cast<unsigned char>(peek())))
        s.push_back(get());

    return Token(s, Number, L, C);
}

Token Lexer::makeOperatorOrSymbol()
{
    int L = line, C = col;
    char c = get();
    string two;
    two += c;
    two += peek();

    if (two == "&&" || two == "||" || two == "==" || two == "!=" || two == "<=" || two == ">=")
    {
        get();
        return Token(two, Operator, L, C);
    }

    string one(1, c);
    if (isOperatorChar(c))
        return Token(one, Operator, L, C);

    if (string("(){}[];,").find(c) != string::npos)
        return Token(one, Symbol, L, C);

    return Token(one, Unknown, L, C);
}

Lexer::Lexer(const string &src) : src(src) {}

vector<Token> Lexer::tokenize()
{
    vector<Token> tokens;

    while (true)
    {
        skipSpacesAndComment();
        char c = peek();
        if (c == '\0')
        {
            Token eof("", End, line, col);
            tokens.push_back(eof);
            break;
        }

        if (isIdentStart(c))
            tokens.push_back(makeIdentifier());

        else if (isdigit(c))
            tokens.push_back(makeNumber());
        else
            tokens.push_back(makeOperatorOrSymbol());
    }
    return tokens;
}
