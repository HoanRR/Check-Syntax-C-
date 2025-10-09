#include "parser/Parser.h"
#include <iostream>
using namespace std;
int main()
{
    const string code = R"CODE(int add(int a, int b) {
int s = a + b;
if (s >= 0) return s
else return -s
}

double x = 1 
int main() {
int y = 10;
while (y > 0) { y = y - 1; 
if (y == 0) y = y + 30;
return y;
})CODE";

    Lexer lex(code);
    vector<Token> tokens = lex.tokenize();

    // for (const Token &t : tokens)
    // {
    //     cout << t.value << " (" << t.line << ":" << t.index << ")\n";
    // }

    Parser P(tokens);
    P.parseProgram();
    if (Parser::ERROR == 0)
    {
        cout << "Parse OK\n";
    }
    

}