#include "Parser.h"
#include <iostream>
#include <sstream>

using namespace std;
int Parser::ERROR = 0;

Parser::Parser(const vector<Token> &tok) : t(tok) {}

const Token &Parser::LA(int k = 0)
{
    return p + k < t.size() ? t[p + k] : t.back();
}

void Parser::error(const string &msg)
{
    const Token tok = LA();
    // std::ostringstream oss;
    cout << "Syntax error at token '" << tok.value << "' (pos " << tok.index << "): " << "line :" << tok.line << " " << msg << endl;
    // throw std::runtime_error(oss.str());
    ERROR++;
}
void Parser::upP()
{
    if (!isEnd())
        p++;
}

bool Parser::isEnd()
{
    return LA().type == End;
}

bool Parser::isOp(const string &s)
{
    return LA().type == Operator && LA().value == s;
}

bool Parser::isSym(const string &s)
{
    return LA().type == Symbol && LA().value == s;
}

bool Parser::isKw(const string &s)
{
    return LA().type == Keyword && LA().value == s;
}

bool Parser::acceptOp(const string &s)
{
    if (isOp(s))
    {
        p++;
        return true;
    }
    return false;
}

bool Parser::acceptSym(const string &s)
{
    if (isSym(s))
    {
        p++;
        return true;
    }
    return false;
}
bool Parser::acceptKw(const string &s)
{
    if (isKw(s))
    {
        p++;
        return true;
    }
    return false;
}

void Parser::expectOp(const string &s)
{
    if (!acceptOp(s))
    {
        error("expected operator '" + s + "'");
        upP();
    }
}

void Parser::expectSym(const string &s)
{
    if (!acceptSym(s))
    {
        error("expected symbol '" + s + "'");
        upP();
    }
}
void Parser::expectKw(const string &s)
{
    if (!acceptKw(s))
    {
        error("expected keyword '" + s + "'");
        upP();
    }
}

string Parser::expectNumber()
{
    if (LA().type == Number)
    {
        string v = LA().value;
        p++;
        return v;
    }
    error("expected number");
    upP();
    return "";
}

string Parser::expectIdent()
{
    if (LA().type == Identifier)
    {
        string v = LA().value;
        p++;
        return v;
    }
    error("expected Identifier");
    upP();

    return "";
}

bool Parser::lookLikeType()
{
    return isKw("int") || isKw("float") || isKw("double") || isKw("long") || isKw("void") || isKw("char");
}

bool Parser::lookLikeFuction()
{
    if (!lookLikeType())
        return false;

    if (t.size() <= p + 1)
        return false;

    if (LA(1).type != Identifier)
        return false;

    if (p + 2 < t.size() && LA(2).type == Symbol && LA(2).value == "(")
        return true;

    return false;
}

void Parser::parseProgram()
{
    while (!isEnd())
    {
        if (lookLikeFuction())
            parseFunction();
        else
            parseDecl();
    }
}

void Parser::parseFunction()
{
    parseType();
    expectIdent();
    expectSym("(");
    if (!isSym(")"))
        parseParamList();
    expectSym(")");
    parseBlock();
}

void Parser::parseDecl()
{
    parseType();
    expectIdent();
    if (acceptOp("="))
        parseExpr();
    expectSym(";");
}

void Parser::parseParamList()
{
    parseType();
    expectIdent();
    while (acceptSym(","))
    {
        parseType();
        expectIdent();
    }
}

void Parser::parseBlock()
{
    expectSym("{");
    if (isEnd())
    {
        error("unexpected EOF after '{'");
        return;
    }

    while (!isEnd() && !isSym("}"))
    {
        size_t guard = p;
        parseStmt();
        if (p == guard)
        {
            error("stuck in block; recovering by skipping one token");
            upP();
            if (isEnd())
                break;
        }
    }
    if (isEnd())
    {
        error("missing '}' before end of file");
        return;
    }
    expectSym("}");
}

void Parser::parseStmt()
{
    if (lookLikeType())
    {
        parseDecl();
        return;
    }

    if (isKw("return"))
    {
        parseReturnStmt();
        return;
    }

    if (isKw("if"))
    {
        parseIfStmt();
        return;
    }

    if (isKw("while"))
    {
        parseWhileStmt();
        return;
    }

    if (isSym("{"))
    {
        parseBlock();
        return;
    }

    parseExprStmt();
}

void Parser::parseExprStmt()
{
    if (!isSym(";"))
        parseExpr();
    expectSym(";");
}

void Parser::parseReturnStmt()
{
    expectKw("return");
    if (!isSym(";"))
        parseExpr();
    expectSym(";");
}

void Parser::parseIfStmt()
{
    expectKw("if");
    expectSym("(");
    parseExpr();
    expectSym(")");
    parseStmt();
    if (acceptKw("else"))
    {
        parseStmt();
    }
}

void Parser::parseWhileStmt()
{
    expectKw("while");
    expectSym("(");
    parseExpr();
    expectSym(")");
    parseStmt();
}

void Parser::parseExpr()
{
    parseAssign();
}
void Parser::parseAssign()
{
    parseLogic();
    if (acceptOp("="))
    {
        parseAssign();
    }
}

void Parser::parseLogic()
{
    parseRel();
    while (isOp("&&") || isOp("||"))
    {
        p++;
        parseRel();
    }
}

void Parser::parseRel()
{
    parseAdd();
    while (isOp("==") || isOp("!=") || isOp("<") || isOp(">") || isOp("<=") || isOp(">="))
    {
        p++;
        parseAdd();
    }
}
void Parser::parseAdd()
{
    parseMul();
    while (isOp("+") || isOp("-"))
    {
        p++;
        parseMul();
    }
}
void Parser::parseMul()
{
    parseUnary();
    while (isOp("*") || isOp("/") || isOp("%"))
    {
        p++;
        parseUnary();
    }
}

void Parser::parseUnary()
{
    if (isOp("+") || isOp("-") || isOp("!"))
        p++;
    parsePrimary();
}

void Parser::parsePrimary()
{
    if (LA().type == Identifier)
    {
        p++;
        return;
    }
    if (LA().type == Number)
    {
        p++;
        return;
    }
    if (acceptSym("("))
    {
        parseExpr();
        expectSym(")");
        return;
    }
    error("expected primary (identifier/number/(expr))");
    upP();
}

void Parser::parseType()
{
    if (acceptKw("int"))
        return;
    if (acceptKw("float"))
        return;
    if (acceptKw("long"))
        return;
    if (acceptKw("char"))
        return;
    if (acceptKw("double"))
        return;
    if (acceptKw("void"))
        return;
    error("expected type (int|float|double|void)");
    upP();
}
