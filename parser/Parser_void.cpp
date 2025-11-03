#include "Parser.h"
#include <iostream>
#include <sstream>

using namespace std;
int Parser::ERROR = 0;

Parser::Parser(const vector<Token> &tok) : t(tok) {}

void Parser::setSemantics(semantics *s)
{
    sem = s;
}

void Parser::setDiagnosticReporter(DiagnosticReporter *dr)
{
    diag = dr;
    if (sem)
    {
        sem->setReporter(dr);
    }
}

const Token &Parser::LA(int k)
{
    return p + k < t.size() ? t[p + k] : t.back();
}


void Parser::reportSyntax(const string &msg, const Token &tok)
{
    ERROR++;
    if (diag)
    {
        diag->syntax(msg, tok.line, tok.col, tok.length);
    }
    else
    {
        cerr << "[Error] Line " << tok.line << ", Col " << tok.col << ": " << msg << endl;
    }
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
bool Parser::isSyncSym(const Token &tok)
{
    return tok.type == Symbol && (tok.value == ";" || tok.value == "}");
}

bool Parser::acceptOp(const string &s)
{
    if (isOp(s))
    {
        upP();
        return true;
    }
    return false;
}

bool Parser::acceptSym(const string &s)
{
    if (isSym(s))
    {
        upP();
        return true;
    }
    return false;
}
bool Parser::acceptKw(const string &s)
{
    if (isKw(s))
    {
        upP();
        return true;
    }
    return false;
}

void Parser::expectSym(const string &s)
{
    if (acceptSym(s))
        return;

    reportSyntax(string("thiếu '") + s + "'", LA());

    if (isSym(s))
    {
        upP();
        return;
    }
    while (!isEnd() && !isSym(s) && !isSyncSym(LA()))
    {
        upP();
    }
    if (isSym(s))
    {
        upP();
    }
}

void Parser::expectOp(const string &s)
{
    if (acceptOp(s))
        return;

    reportSyntax(string("thiếu toán tử '") + s + "'", LA());

    if (isOp(s))
    {
        upP();
        return;
    }
    while (!isEnd() && !isOp(s) && !isSyncSym(LA()))
        upP();
    if (isOp(s))
        upP();
}

void Parser::expectKw(const string &s)
{
    if (acceptKw(s))
        return;

    reportSyntax(string("thiếu từ khóa '") + s + "'", LA());

    if (isKw(s))
    {
        upP();
        return;
    }
    while (!isEnd() && !isKw(s) && !isSyncSym(LA()))
        upP();
    if (isKw(s))
        upP();
}

string Parser::expectNumber()
{
    if (LA().type == Number)
    {
        string v = LA().value;
        upP();
        return v;
    }
    reportSyntax("thiếu số ", LA());
    upP();
    return "";
}

Token Parser::expectIdent()
{
    if (LA().type == Identifier)
    {
        string v = LA().value;
        Token ret = LA();
        upP();
        return ret;
    }
    reportSyntax("thiếu định danh ", LA());
    upP();

    return Token("", Unknown, LA().line, LA().col, LA().length);
}
bool Parser::lookLikeType()
{
    int q = 0;
    if (isKw("const"))
    {
        q = 1;
    }
    if (q)
    {
    }

    return isKw("int") || isKw("float") || isKw("double") || isKw("long") || isKw("void") || isKw("char") ||
           (q && (LA(1 + q - 1).type == Keyword && (LA(q).value == "int" || LA(q).value == "float" || LA(q).value == "double" ||
                                                    LA(q).value == "long" || LA(q).value == "void" || LA(q).value == "char")));
}

bool Parser::lookLikeFunction()
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
        if (lookLikeFunction())
            parseFunction();
        else
            parseDecl();
    }
    if (sem)
        sem->leaveScope();
}

void Parser::parseFunction()
{
    parseType();
    Token identoken = expectIdent();
    sem->beginFunction(lastTypekind, identoken);
    expectSym("(");
    if (!isSym(")"))
        parseParamList();
    expectSym(")");
    parseBlock(true);
    sem->endFunction();
}

void Parser::parseDecl()
{
    parseType();
    parseDeclarator();
    while (acceptSym(","))
    {
        parseDeclarator();
    }
    expectSym(";");
}
void Parser::parseDeclarator()
{
    Token nameTok = expectIdent();
    if (sem)
        sem->declareVar(lastTypekind, nameTok);

    if (acceptOp("="))
    {
        parseExpr();
    }
}
void Parser::parseParamList()
{
    parseType();
    Token IdentToken = expectIdent();
    sem->declareParam(lastTypekind, IdentToken);
    while (acceptSym(","))
    {
        parseType();
        IdentToken = expectIdent();
        sem->declareParam(lastTypekind, IdentToken);
    }
}

void Parser::parseBlock(bool isFunctionBlock)
{
    if (!isFunctionBlock)
        sem->enterScope();
    expectSym("{");
    if (isEnd())
    {
        reportSyntax("thiếu '}' trước khi kết thúc file", t.back());
        if (!isFunctionBlock)
            sem->leaveScope();
        return;
    }

    while (!isEnd() && !isSym("}"))
    {
        size_t guard = p;
        parseStmt();
        if (p == guard)
        {
            reportSyntax("không thể phân tích cú pháp câu lệnh", LA());
            upP();
            if (isEnd())
                break;
        }
    }
    if (isEnd())
    {
        reportSyntax("thiếu '}' trước khi kết thúc file", t.back());
        if (!isFunctionBlock)
            sem->leaveScope();
        return;
    }
    expectSym("}");
    if (!isFunctionBlock)
        sem->leaveScope();
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
        parseBlock(false);
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
    bool hasExpr = false;
    Token retTok = LA(-1);
    if (!isSym(";"))
    {
        parseExpr();
        hasExpr = true;
    }
    sem->onReturnToken(LA(-1), hasExpr);
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
    parseLogicalOr();
    if (acceptOp("=") || acceptOp("+=") || acceptOp("-=") || acceptOp("*=") || acceptOp("/=") || acceptOp("%="))
    {
        parseAssign();
    }
}

void Parser::parseLogicalOr()
{
    parseBinaryLeftAssoc(&Parser::parseLogicalAnd, {"||"});
}
void Parser::parseLogicalAnd()
{
    parseBinaryLeftAssoc(&Parser::parseEquality, {"&&"});
}
void Parser::parseEquality()
{
    parseBinaryLeftAssoc(&Parser::parseRelational, {"==", "!="});
}
void Parser::parseRelational()
{
    parseBinaryLeftAssoc(&Parser::parseShift, {"<", "<=", ">", ">="});
}
void Parser::parseShift()
{
    parseBinaryLeftAssoc(&Parser::parseAdd, {"<<", ">>"});
}
void Parser::parseAdd()
{
    parseBinaryLeftAssoc(&Parser::parseMul, {"+", "-"});
}
void Parser::parseMul()
{
    parseBinaryLeftAssoc(&Parser::parseUnary, {"*", "/", "%"});
}

void Parser::parseUnary()
{
    if (acceptOp("+") || acceptOp("-") || acceptOp("!") || acceptOp("++") || acceptOp("--"))
    {
        parseUnary();
        return;
    }
    parsePostfix();
}
void Parser::parsePostfix()
{
    // primary trước
    parsePrimary();
    // sau đó là ++ hoặc --
    while (acceptOp("++") || acceptOp("--"))
    {
    }
}

void Parser::parseBinaryLeftAssoc(void (Parser::*sub)(), const vector<string> &ops)
{
    (this->*sub)();
    while (true)
    {
        bool matched = false;
        for (auto &op : ops)
        {
            if (acceptOp(op))
            {
                matched = true;
                break;
            }
        }
        if (!matched)
            break;
        (this->*sub)();
    }
}
void Parser::parseArgList()
{

    parseExpr();
    while (acceptSym(","))
    {
        parseExpr();
    }
}

void Parser::parsePrimary()
{
    // số
    if (LA().type == TokenType::Number)
    {
        upP();
        return;
    }

    // string hoặc char
    if (LA().type == TokenType::String || LA().type == TokenType::Char)
    {
        upP();
        return;
    }

    // (expr)
    if (isSym("("))
    {
        upP();
        parseExpr();
        expectSym(")");
        return;
    }

    if (LA().type == TokenType::Identifier)
    {

        if (LA().type == TokenType::Identifier)
        {
            if (p + 1 < (int)t.size() && LA(1).type == TokenType::Symbol && LA(1).value == "(")
            {
                const Token nameTok = LA();
                sem->useIdent(nameTok);
                upP();
                expectSym("(");
                if (!isSym(")"))
                    parseArgList();
                expectSym(")");
                return;
            }
            sem->useIdent(LA()); // định danh thường
            upP();
            return;
        }
    }
    // Không khớp gì cả -> lỗi
    reportSyntax("biểu thức không hợp lệ, thiếu toán hạng (identifier/number/(expr))", LA());
    upP();
}

void Parser::parseType()
{
    // có thể có const ở đầu
    if (acceptKw("const"))
    {
        while (acceptOp("*"))
            ; // con trỏ hằng
    }
    if (acceptKw("int"))
    {
        lastTypekind = TypeKind::Int;
    }
    else if (acceptKw("float"))
    {
        lastTypekind = TypeKind::Float;
    }
    else if (acceptKw("long"))
    {
        lastTypekind = TypeKind::Long;
    }
    else if (acceptKw("char"))
    {
        lastTypekind = TypeKind::Char;
    }
    else if (acceptKw("double"))
    {
        lastTypekind = TypeKind::Double;
    }
    else if (acceptKw("void"))
    {
        lastTypekind = TypeKind::Void;
    }
    else
    {
        reportSyntax("thiếu kiểu dữ liệu", LA());
        lastTypekind = TypeKind::Unknown;
        upP();
        return;
    }
    while (acceptOp("*"))
    {
    }
}
