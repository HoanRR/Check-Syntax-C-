#include "semantics.h"

semantics::semantics() : diag(nullptr),
                         inFunction(false),
                         currentFunc(),
                         currentRet(TypeKind::Void),
                         funcTok("", TokenType::Unknown, 0, 0, 0) {};

void semantics::setReporter(DiagnosticReporter *r)
{
    diag = r;
}
// ===== Scope =====
void semantics::enterScope()
{
    sym.enterScope();
}
void semantics::leaveScope()
{
    sym.leaveScope();
}

// ===== Hàm =====
void semantics::beginFunction(TypeKind retKind, const Token &nameTok)
{
    inFunction = true;
    currentFunc = nameTok.value;
    currentRet = retKind;
    funcTok = nameTok;

    str_Symbol s{nameTok.value, true, retKind, nameTok};

    if (!sym.declareSymbol(s))
    {
        if (diag)
            diag->redeclaration(nameTok.value, nameTok.line, nameTok.col, nameTok.length);
    }

    enterScope();
}

void semantics::endFunction()
{
    if (inFunction)
    {
        leaveScope();
        inFunction = false;
        currentFunc.clear();
        currentRet = TypeKind::Void;
    }
}

void semantics::declareVar(TypeKind ty, const Token &nameTok)
{
    str_Symbol s{nameTok.value, false, ty, nameTok};

    if (!sym.declareSymbol(s))
    {
        if (diag)
            diag->redeclaration(nameTok.value, nameTok.line, nameTok.col, nameTok.length);
    }
}

void semantics::declareParam(TypeKind ty, const Token &nameTok)
{
    declareVar(ty, nameTok);
}

// ===== Sử dụng định danh =====
void semantics::useIdent(const Token &identTok)
{
    if (sym.lookupSymbol(identTok.value) == nullptr)
    {
        if (diag)
            diag->undeclared(identTok.value, identTok.line, identTok.col, identTok.length);
    }
}

// ===== Return =====
void semantics::onReturnToken(const Token &retTok, bool hasExpr)
{

    if (currentRet == TypeKind::Void && hasExpr)
    {
        if (diag)
        {
            diag->add(DiagSeverity::Error, "E-RETVOID",
                      "Hàm 'void' không được trả về biểu thức.",
                      retTok.line, retTok.col, retTok.length);
        }
    }
    if (currentRet != TypeKind::Void && !hasExpr)
    {
        if (diag)
        {
            diag->add(DiagSeverity::Error, "E-RETEMPTY",
                      "Hàm không phải 'void' cần trả về một biểu thức.",
                      retTok.line, retTok.col, retTok.length);
        }
    }
}

void semantics::LibraryFunction(const string &name)
{
    Token libToken(name, TokenType::Identifier, 0, 0, name.length());

    str_Symbol s{name, true, TypeKind::Unknown, libToken};

    if (!sym.scopes.empty())
    {
        auto &globalScope = sym.scopes[0];
        globalScope.emplace(name, std::move(s));
    }
}
