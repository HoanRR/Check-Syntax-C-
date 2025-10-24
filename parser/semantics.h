#include "..\symboltable\symboltable.h"
#include "..\Diagnostic\DiagnosticReporter.h"
class semantics
{
    SymbolTable sym;
    DiagnosticReporter *diag = nullptr;

    bool inFunction = false;
    string currentFunc;
    TypeKind currentRet = TypeKind::Void;
    Token funcTok;

public:
    semantics();
    void setReporter(DiagnosticReporter* r);
    
    void enterScope();
    void leaveScope();
    
    void beginFunction( TypeKind retKind, const Token &nameTok);
    void endFunction();
    
    void declareVar( TypeKind ty, const Token &nameTok);
    void declareParam( TypeKind ty, const Token &nameTok);
   
    void useIdent(const Token &identTok);

    void onReturnToken(const Token &retTok, bool hasExpr);
};
