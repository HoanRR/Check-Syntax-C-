#include "../lexer/Lexer.h"
#include "../AST/AST.h"

class Parser
{
public:
    Parser(const vector<Token> &);
    static int ERROR;
    unique_ptr<TranslationUnit> parseProgram();

private:
    const vector<Token> &t;
    int p = 0;

    void error(const string &);
    void upP();

    const Token &LA(int);
    bool isEnd();

    bool isOp(const string &);
    bool isSym(const string &);
    bool isKw(const string &);

    bool acceptOp(const string &);
    bool acceptSym(const string &);
    bool acceptKw(const string &);

    void expectOp(const string &);
    void expectSym(const string &);
    void expectKw(const string &);

    string expectIdent();
    string expectNumber();

    bool lookLikeType();
    bool lookLikeFunction();
    bool lookIdent();

    unique_ptr<FuncDecl> parseFunction();
    unique_ptr<Decl> parseDecl(); // hiện tại chỉ VarDecl
    unique_ptr<BlockStmt> parseBlock();
    unique_ptr<Stmt> parseStmt();
    unique_ptr<Stmt> parseExprStmt();
    unique_ptr<ReturnStmt> parseReturnStmt();
    unique_ptr<IfStmt> parseIfStmt();
    unique_ptr<WhileStmt> parseWhileStmt();

    // Expr precedence
    unique_ptr<Expr> parseExpr();
    unique_ptr<Expr> parseAssign();  // right-assoc
    unique_ptr<Expr> parseLogic();   // && ||
    unique_ptr<Expr> parseRel();     // == != < > <= >=
    unique_ptr<Expr> parseAdd();     // + -
    unique_ptr<Expr> parseMul();     // * / %
    unique_ptr<Expr> parseUnary();   // + - !
    unique_ptr<Expr> parsePrimary(); // ident / number / (expr) / call

    // helpers cho type / params
    string parseType(); // trả về tên kiểu
    vector<ParamDecl> parseParamList();
};