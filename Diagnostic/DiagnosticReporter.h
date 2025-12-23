#pragma once
#include <string>
#include <vector>
using namespace std;

enum class DiagSeverity
{
    Error = 1,
    Warning = 2,
    Info = 3
};

struct DiagnosticItem
{
    DiagSeverity severity;
    string code;
    string message;
    int line;
    int col;
    int length;
};

class DiagnosticReporter
{
    vector<DiagnosticItem> items;

public:
    void add(DiagSeverity sev, const string &code, const string &msg, int line, int col, int len);
    void syntax(const string &msg, int line, int col, int len);
    void undeclared(const string &name, int line, int col, int len, const string &suggestion = "");
    void redeclaration(const string &name, int line, int col, int len);
    
    const vector<DiagnosticItem> &all() const;
    bool empty() const;
    void clear();
};