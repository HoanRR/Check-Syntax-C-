#include "Parser_AST.h"
#include <iostream>

int Parser::ERROR = 0;

Parser::Parser(const vector<Token> &tok) : t(tok) {}

const Token &Parser::LA(int k = 0) { return p + k < t.size() ? t[p + k] : t.back(); }

void Parser::error(const string &msg)
{
    const Token tok = LA();
    // std::ostringstream oss;
    cout << "Syntax error at token '" << tok.value << "' (pos " << tok.col << "): " << "line :" << tok.line << " " << msg << endl;
    // throw std::runtime_error(oss.str());
    ERROR++;
}

void Parser::upP()
{
    if (!isEnd())
        ++p;
}

bool Parser::isEnd() { return LA().type == End; }

bool Parser::isOp(const string &s) { return LA().type == Operator && LA().value == s; }

bool Parser::isSym(const string &s) { return LA().type == Symbol && LA().value == s; }

bool Parser::isKw(const string &s) { return LA().type == Keyword && LA().value == s; }

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

bool Parser::lookLikeType()
{
    return isKw("int") || isKw("float") || isKw("double") || isKw("long") || isKw("void") || isKw("char");
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
bool Parser::lookIdent()
{
    return LA().type == Identifier;
}

unique_ptr<TranslationUnit> Parser::parseProgram()
{
    auto tu = make_unique<TranslationUnit>();
    while (!isEnd())
    {
        if (lookLikeFunction())
        {
            tu->decls.push_back(parseFunction());
        }
        else
        {
            tu->decls.push_back(parseDecl());
        }
    }
    return tu;
}

unique_ptr<FuncDecl> Parser::parseFunction()
{
    string retTy = parseType();
    string name = expectIdent();
    expectSym("(");
    vector<ParamDecl> params;
    if (!isSym(")"))
        params = parseParamList();
    expectSym(")");
    auto body = parseBlock();
    return make_unique<FuncDecl>(retTy, name, move(params), move(body));
}


unique_ptr<Decl> Parser::parseDecl()
{
    string ty = parseType();
    string name = expectIdent();
    unique_ptr<Expr> init;
    if (acceptOp("="))
    {
        init = parseExpr();
    }
    expectSym(";");
    return make_unique<VarDecl>(ty, name, move(init));
}

vector<ParamDecl> Parser::parseParamList()
{
    vector<ParamDecl> ps;
    ParamDecl first{parseType(), expectIdent()};
    ps.push_back(move(first));
    while (acceptSym(","))
    {
        ParamDecl more{parseType(), expectIdent()};
        ps.push_back(move(more));
    }
    return ps;
}

unique_ptr<BlockStmt> Parser::parseBlock()
{
    expectSym("{");
    auto block = make_unique<BlockStmt>();
    if (isEnd())
    {
        error("unexpected EOF after '{'");
        return block;
    }
    while (!isEnd() && !isSym("}"))
    {
        size_t guard = p;
        auto st = parseStmt();
        if (st)
            block->body.push_back(move(st));
        if (p == guard)
        { // recovery
            error("stuck in block; recovering by skipping one token");
            upP();
            if (isEnd())
                break;
        }
    }
    if (isEnd())
    {
        error("missing '}' before end of file");
        return block;
    }
    expectSym("}");
    return block;
}

unique_ptr<Stmt> Parser::parseStmt()
{
    if (lookLikeType())
    {
        auto d = parseDecl();
        return make_unique<DeclStmt>(move(d));
    }
    if (isKw("return"))
        return parseReturnStmt();
    if (isKw("if"))
        return parseIfStmt();
    if (isKw("while"))
        return parseWhileStmt();
    if (isSym("{"))
        return parseBlock();
    return parseExprStmt();
}

unique_ptr<Stmt> Parser::parseExprStmt()
{
    if (!isSym(";"))
    {
        auto e = parseExpr();
        expectSym(";");
        return make_unique<ExprStmt>(move(e));
    }
    expectSym(";"); // empty stmt
    return make_unique<ExprStmt>(nullptr);
}

unique_ptr<ReturnStmt> Parser::parseReturnStmt()
{
    expectKw("return");
    if (!isSym(";"))
    {
        auto e = parseExpr();
        expectSym(";");
        return make_unique<ReturnStmt>(move(e));
    }
    expectSym(";");
    return make_unique<ReturnStmt>(nullptr);
}

unique_ptr<IfStmt> Parser::parseIfStmt()
{
    expectKw("if");
    expectSym("(");
    auto cond = parseExpr();
    expectSym(")");
    auto thenS = parseStmt();
    unique_ptr<Stmt> elseS;
    if (acceptKw("else"))
        elseS = parseStmt();
    return make_unique<IfStmt>(move(cond), move(thenS), move(elseS));
}

unique_ptr<WhileStmt> Parser::parseWhileStmt()
{
    expectKw("while");
    expectSym("(");
    auto cond = parseExpr();
    expectSym(")");
    auto body = parseStmt();
    return make_unique<WhileStmt>(move(cond), move(body));
}

// ========== Biểu thức và độ ưu tiên ==========

unique_ptr<Expr> Parser::parseExpr() { return parseAssign(); }

// assign: right-assoc  a = b = c
unique_ptr<Expr> Parser::parseAssign()
{
    auto lhs = parseLogic();
    if (acceptOp("="))
    {
        auto rhs = parseAssign();
        return make_unique<BinaryExpr>("=", move(lhs), move(rhs));
    }
    return lhs;
}

unique_ptr<Expr> Parser::parseLogic()
{
    auto e = parseRel();
    while (isOp("&&") || isOp("||"))
    {
        string op = LA().value;
        upP();
        auto r = parseRel();
        e = make_unique<BinaryExpr>(op, move(e), move(r));
    }
    return e;
}

unique_ptr<Expr> Parser::parseRel()
{
    auto e = parseAdd();
    while (isOp("==") || isOp("!=") || isOp("<") || isOp(">") || isOp("<=") || isOp(">="))
    {
        string op = LA().value;
        upP();
        auto r = parseAdd();
        e = make_unique<BinaryExpr>(op, move(e), move(r));
    }
    return e;
}

unique_ptr<Expr> Parser::parseAdd()
{
    auto e = parseMul();
    while (isOp("+") || isOp("-"))
    {
        string op = LA().value;
        upP();
        auto r = parseMul();
        e = make_unique<BinaryExpr>(op, move(e), move(r));
    }
    return e;
}

unique_ptr<Expr> Parser::parseMul()
{
    auto e = parseUnary();
    while (isOp("*") || isOp("/") || isOp("%"))
    {
        string op = LA().value;
        upP();
        auto r = parseUnary();
        e = make_unique<BinaryExpr>(op, move(e), move(r));
    }
    return e;
}

unique_ptr<Expr> Parser::parseUnary()
{
    if (isOp("+") || isOp("-") || isOp("!"))
    {
        string op = LA().value;
        upP();
        auto inner = parseUnary();
        return make_unique<UnaryExpr>(op, move(inner));
    }
    return parsePrimary();
}

unique_ptr<Expr> Parser::parsePrimary()
{
    // ident / call
    if (LA().type == Identifier)
    {
        string name = LA().value;
        upP();
        unique_ptr<Expr> base = make_unique<IdentExpr>(name);
        if (acceptSym("("))
        { // call
            auto call = make_unique<CallExpr>(move(base));
            if (!isSym(")"))
            {
                call->args.push_back(parseExpr());
                while (acceptSym(","))
                    call->args.push_back(parseExpr());
            }
            expectSym(")");
            return call;
        }
        return base;
    }
    // number
    if (LA().type == Number)
    {
        string v = LA().value;
        upP();
        return make_unique<NumberExpr>(v);
    }
    // (expr)
    if (acceptSym("("))
    {
        auto e = parseExpr();
        expectSym(")");
        return e;
    }
    error("expected primary (identifier/number/(expr))");
    upP();
    // trả về “nút lỗi” tối giản để tiếp tục
    return make_unique<IdentExpr>("<error>");
}

// type: trả về chuỗi tên kiểu
string Parser::parseType()
{
    if (acceptKw("int"))
        return "int";
    if (acceptKw("float"))
        return "float";
    if (acceptKw("long"))
        return "long";
    if (acceptKw("char"))
        return "char";
    if (acceptKw("double"))
        return "double";
    if (acceptKw("void"))
        return "void";
    error("expected type (int|float|double|void)");
    upP();
    return "<error-type>";
}
