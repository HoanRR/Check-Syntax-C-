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

static inline bool isSpace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

void Lexer::skipSpacesAndComment()
{
    while (true)
    {
        // dấu cách
        if (isSpace((unsigned char)peek()))
        {
            get();
            continue;
        }
        // Comment //
        if (peek() == '/' && peek(1) == '/')
        {
            get();
            get();
            while (peek() != '\n' && peek() != '\0')
                get();
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
            else
                break;
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
    static const string ops = R"(~!@#$%^&*()-+=|{}[]:;'",.<>/?\)";
    return ops.find(c) != string::npos;
}

bool Lexer::isDigit(char c)
{
    return std::isdigit((unsigned char)c);
}
bool Lexer::isHexDigit(char c)
{
    return std::isxdigit((unsigned char)c);
}
bool Lexer::isOctDigit(char c)
{
    return c >= '0' && c <= '7';
}
bool Lexer::isBinDigit(char c)
{
    return c == '1' || c == '0';
}

Token Lexer::makeIdentifier()
{
    int startLine = line, startCol = col;
    string ident;

    while (isIdentCont((peek())))
        ident.push_back(get());

    static const unordered_set<string> keywords = {
        "int",
        "float",
        "double",
        "char",
        "long",
        "void",
        "return",
        "if",
        "else",
        "while",
        "for",
    };
    TokenType Type = keywords.find(ident) != keywords.end() ? Keyword : Identifier;

    int len = ident.length();
    return Token(ident, Type, startLine, startCol, len);
}

Token Lexer::makeNumber()
{
    int startLine = line, startCol = col;
    string num;
    bool isFloat = false;

    auto consumeDigits = [&](auto pred)
    {
        int cnt = 0;
        while (pred(peek()))
        {
            num.push_back(get());
            cnt++;
        }
        return cnt;
    };

    if (peek() == '0' && (peek(1) == 'x' || peek(1) == 'X'))
    {
        num.push_back(get());
        num.push_back(get());
        int n = consumeDigits([&](char c)
                              { return isHexDigit(c); });
        if (n == 0)
            return Token(num, TokenType::Error, startLine, startCol, num.length());
    }
    else if (peek() == '0' && isdigit(peek(1)))
    {
        num.push_back(get());
        consumeDigits([&](char c)
                      { return isOctDigit(c); });
    }
    else
    {
        consumeDigits([&](char c)
                      { return isDigit(c); });

        if (peek() == '.' && isDigit(peek(1)))
        {
            isFloat = true;
            num.push_back(get());
            consumeDigits([&](char c)
                          { return isDigit(c); });
        }

        if (peek() == 'e' || peek() == 'E')
        {
            isFloat = true;
            num.push_back(get());
            int n = consumeDigits([&](char c)
                                  { return isDigit(c); });
            if (n == 0)
                return Token(num, TokenType::Error, startLine, startCol, num.length());
        }
    }

    return Token(num, TokenType::Number, startLine, startCol, num.length());
}
Token Lexer::makeString()
{
    int startLine = line, startCol = col;
    string s;
    char quote = get();
    s.push_back(quote);

    while (true)
    {
        char c = get();
        if (c == '\0' || c == '\n')
        {
            return Token(s, TokenType::Error, startLine, startCol, (int)s.size());
        }
        s.push_back(c);
        if (c == '\\')
        {
            char n = get();
            if (n == '\0')
                return Token(s, TokenType::Error, startLine, startCol, (int)s.size());
            s.push_back(n);
            continue;
        }
        if (c == '"')
            break;
    }
    return Token(s, TokenType::String, startLine, startCol, (int)s.size());
}

Token Lexer::makeChar()
{
    int startLine = line, startCol = col;
    string s;
    char quote = get();
    s.push_back(quote);

    char c = get();
    if (c == '\0' || c == '\n')
        return Token(s, TokenType::Error, startLine, startCol, (int)s.size());
    s.push_back(c);
    if (c == '\\')
    {
        char n = get();
        if (n == '\0' || n == '\n')
            return Token(s, TokenType::Error, startLine, startCol, (int)s.size());
        s.push_back(n);
    }
    if (peek() != '\'')
    {
        return Token(s, TokenType::Error, startLine, startCol, (int)s.size());
    }
    s.push_back(get());
    return Token(s, TokenType::Char, startLine, startCol, (int)s.size());
}

Token Lexer::makeOperatorOrSymbol()
{
    int startLine = line, startCol = col;

    static const unordered_set<string> ops2 = {
        "==", "!=", ">=", "<=", "++", "--", "->", "::", "+=", "-=", "*=", "/=", "%=", "&&", "||", ">>", "<<", "&=", "|=", "^="};

    auto matchN = [&](int n) -> string
    {
        string cand;
        cand.reserve(n);
        for (int k = 0; k < n; k++)
        {
            char c = peek(k);
            if (c == '\0')
                return "";
            cand.push_back(c);
        }
        return cand;
    };
    string s2 = matchN(2);
    if (!s2.empty() && ops2.count(s2))
    {
        get();
        get();
        return Token(s2, TokenType::Operator, startLine, startCol, 2);
    }
    char c = get();
    string s(1, c);

    static const string symbols = "(){}[];,?.";
    if (symbols.find(c) != string::npos)
    {
        return Token(s, TokenType::Symbol, startLine, startCol, 1);
    }

    // Dấu chấm '.' có thể là phần số thực; nhưng tới đây nghĩa là trước đó không phải số => coi là Symbol
    if (c == '.')
    {
        return Token(s, TokenType::Symbol, startLine, startCol, 1);
    }

    // Còn lại: coi là Operator (=' + - * / % & | ^ ! ~ < > : \')
    return Token(s, TokenType::Operator, startLine, startCol, 1);
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
            Token eof("", End, line, col, 1);
            tokens.push_back(eof);
            break;
        }

        if (isIdentStart(c))
        {
            tokens.push_back(makeIdentifier());
        }
        else if (std::isdigit((unsigned char)c))
        {
            tokens.push_back(makeNumber());
        }
        else if (c == '"')
        {
            tokens.push_back(makeString());
        }
        else if (c == '\'')
        {
            tokens.push_back(makeChar());
        }
        else if (isOperatorChar(c))
        {
            tokens.push_back(makeOperatorOrSymbol());
        }
        else
        {
            int startLine = line, startCol = col;
            string s(1, get());
            tokens.emplace_back(s, TokenType::Unknown, startLine, startCol, 1);
        }
    }
    return tokens;
}
