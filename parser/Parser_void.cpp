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
    if (tok.type == Symbol && (tok.value == ";" || tok.value == "}"))
        return true;

    if (tok.type == Symbol && tok.value == "{")
        return true;

    if (tok.type == Keyword)
    {
        string v = tok.value;
        return v == "if" || v == "else" || v == "while" || v == "for" ||
               v == "return" || v == "break" || v == "continue" ||
               v == "int" || v == "float" || v == "double" ||
               v == "void" || v == "char" || v == "long" || v == "const";
    }

    return false;
}
bool Parser::isExprStart()
{
    Token tok = LA();
    if (tok.type == TokenType::Identifier ||
        tok.type == TokenType::Number ||
        tok.type == TokenType::String ||
        tok.type == TokenType::Char)
    {
        return true;
    }

    if (isSym("("))
    {
        return true;
    }

    if (tok.type == TokenType::Operator)
    {
        string v = tok.value;
        return v == "+" || v == "-" || v == "!" || v == "++" || v == "--";
    }

    return false;
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
    int k = 0;

    // 1. Kiểm tra xem có bắt đầu bằng 'const' không
    if (LA(k).type == Keyword && LA(k).value == "const")
    {
        k++;

        // 2. Nhảy qua các dấu '*' nếu có (con trỏ hằng)
        while (LA(k).type == Operator && LA(k).value == "*")
        {
            k++;
        }
    }

    // 3.kiểm tra xem có phải là kiểu cơ bản không
    if (LA(k).type == Keyword)
    {
        string val = LA(k).value;
        return val == "int" || val == "float" || val == "double" ||
               val == "long" || val == "void" || val == "char";
    }

    return false;
}
bool Parser::lookLikeFunction()
{
    int k = 0;
    if (LA(k).type == Keyword && LA(k).value == "const")
    {
        k++;
        while (LA(k).type == Operator && LA(k).value == "*")
            k++;
    }

    // có kiểu dữ liệu
    if (LA(k).type == Keyword)
    {
        string val = LA(k).value;
        bool isType = (val == "int" || val == "float" || val == "double" ||
                       val == "long" || val == "void" || val == "char");
        if (isType)
        {
            k++;
            while (LA(k).type == Operator && LA(k).value == "*")
                k++;
            if (LA(k).type == Identifier && p + k + 1 < t.size() && LA(k + 1).value == "(")
                return true;
        }
    }

    // Hàm thiếu kiểu
    if (LA(0).type == Identifier)
    {
        if (p + 1 < t.size() && LA(1).type == Symbol && LA(1).value == "(")
        {
            return true;
        }
    }

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
        reportSyntax("thiếu '}' ", t.back());
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
        reportSyntax("thiếu '}' ", t.back());
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

    if (isKw("for"))
    {
        parseForStmt();
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

    if (acceptSym("("))
    {
        parseExpr();
        expectSym(")");
    }
    else
    {
        reportSyntax("thiếu '('", LA());

        if (isExprStart())
        {
            parseExpr();
        }

        if (!acceptSym(")"))
        {
            reportSyntax("thiếu ')' ", LA());
        }
    }

    parseStmt();

    if (acceptKw("else"))
    {
        parseStmt();
    }
}

void Parser::parseWhileStmt()
{
    expectKw("while");
    if (acceptSym("("))
    {
        parseExpr();
        expectSym(")");
    }
    else
    {
        reportSyntax("thiếu '(' ", LA());

        if (isExprStart())
        {
            parseExpr();
        }

        if (!acceptSym(")"))
        {
            reportSyntax("thiếu ')' ", LA());
        }
    }
    parseStmt();
}

void Parser::parseForStmt()
{
    expectKw("for");
    expectSym("(");

    if (!isSym(";"))
    {
        if (lookLikeType())
            parseDecl();
        else
            parseExprStmt();
    }
    else
        expectSym(";");

    if (!isSym(";"))
        parseExpr();
    expectSym(";");

    if (!isSym(")"))
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
    if (isOp("&"))
    {
        upP();
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
    string val = LA().value;
    bool isStopper = (val == ";" || val == "}" || val == ")");

    if (!isStopper)
    {
        upP();
    }
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
        if (LA().type == Identifier)
        {
            reportSyntax("thiếu kiểu dữ liệu", LA());
            lastTypekind = TypeKind::Int;
            return;
        }

        reportSyntax("thiếu kiểu dữ liệu", LA());
        lastTypekind = TypeKind::Unknown;
        upP();
        return;
    }
    while (acceptOp("*"))
    {
    }
}
