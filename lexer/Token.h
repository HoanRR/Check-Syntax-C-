#include <string>
using namespace std;
enum TokenType
{
    Keyword,
    Identifier,
    Operator,
    Number,
    Symbol,
    End,
    Unknown
};
class Token
{
public:
    TokenType type;
    string value;
    int line, index;

    Token(string value, TokenType type, int line, int index) : type(type), value(value), line(line), index(index) {}
    
};