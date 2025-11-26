#pragma once
#include "../lexer/Lexer.h"
#include "../Diagnostic/DiagnosticReporter.h"
#include "semantics.h"
#include <iostream>
#include <sstream>
using namespace std;
class Parser
{
public:
    void parseProgram();
    Parser(const vector<Token> &);
    static int ERROR;
    DiagnosticReporter *diag = nullptr;

    void setDiagnosticReporter(DiagnosticReporter *);
    void setSemantics(semantics *);

private:
    const vector<Token> &t;
    TypeKind lastTypekind = TypeKind::Unknown;
    int p = 0;
    semantics *sem = nullptr;
    void reportSyntax(const string &, const Token &);
    void upP();

    const Token &LA(int = 0);

    bool isEnd();
    bool isOp(const string &);
    bool isSym(const string &);
    bool isKw(const string &);
    bool isSyncSym(const Token &);
    bool isExprStart();

    bool acceptOp(const string &);
    bool acceptSym(const string &);
    bool acceptKw(const string &);

    void expectOp(const string &);
    void expectSym(const string &);
    void expectKw(const string &);

    Token expectIdent();
    string expectNumber();

    bool lookLikeType();
    bool lookLikeFunction();
    // ===== Grammar (EBNF) =====
    void parseFunction();   // Function := Type Ident "(" [ParamList] ")" Block
    void parseDecl();       // Decl     := Type Declarator
    void parseDeclarator(); // Declarator := Ident { "," Ident } [ "=" Expr ] ";"
    void parseParamList();  // ParamList:= (Type Ident) { "," Type Ident }
    void parseBlock(bool);  // Block    := "{" { Stmt } "}"
    void parseStmt();       // Stmt         := Decl | ExprStmt | ReturnStmt | IfStmt | WhileStmt | ForStmt | Block
    void parseExprStmt();   // ExprStmt := [Expr] ";"
    void parseReturnStmt(); // ReturnStmt:= "return" [Expr] ";"
    void parseIfStmt();     // IfStmt   := "if" "(" Expr ")" Stmt ["else" Stmt]
    void parseWhileStmt();  // WhileStmt:= "while" "(" Expr ")" Stmt
    void parseForStmt();    // ForStmt := "for" "(" [ExprStmt] [Expr] ";" [Expr] ")" Stmt

    // Expr
    void parseExpr();       // Expr     := Assign
    void parseAssign();     // Assign   := LogicalOr ( "=" Assign )?
    void parseLogicalOr();  //   ||
    void parseLogicalAnd(); //  &&
    void parseEquality();   //  ==, !=
    void parseRelational(); // <, >, <=, >=
    void parseAdd();        // Add      := Mul (("+"|"-") Mul)*
    void parseMul();        // Mul      := Unary (("*"|"/"|"%") Unary)*
    void parseUnary();      // Unary    := ("+"|"-"|"!")? Primary
    void parseArgList();    // ArgList  := [Expr {"," Expr}]
    void parsePrimary();    // Primary  := Ident | Number | "(" Expr ")" | Call
    void parseShift();      //   <<, >>
    void parsePostfix();    //   hậu tố ++, --

    void parseType();                                                      // Type     := "int" | "float" | "double" | "void"
    void parseBinaryLeftAssoc(void (Parser::*)(), const vector<string> &); // helper for left-assoc binary ops
};
