
#pragma once
#include <string>
#include <vector>
using namespace std;

enum class DiagSeverity { Error=1, Warning=2, Info=3 };

struct DiagnosticItem {
    DiagSeverity severity;
    string code;
    string message;
    int line;   // 1-based
    int col;    // 1-based
    int length; // characters
};

class DiagnosticReporter {
    vector<DiagnosticItem> items;
public:
    void add(DiagSeverity sev, const string& code, const string& msg, int line, int col, int length) {
        items.push_back({sev, code, msg, line, col, length});
    }
    // Shorthands for common cases
    void syntax(const string& msg, int line, int col, int length) {
        add(DiagSeverity::Error, "E-SYN", "Lỗi cú pháp: " + msg, line, col, length);
    }
    void undeclared(const string& name, int line, int col, int length) {
        add(DiagSeverity::Error, "E-UNDECL", "Biến '" + name + "' chưa được khai báo.", line, col, length);
    }
    void redeclaration(const string& name, int line, int col, int length) {
        add(DiagSeverity::Error, "E-REDECL", "Tên '" + name + "' đã được khai báo trong phạm vi này.", line, col, length);
    }
    void typeMismatch(const string& fromT, const string& toT, int line, int col, int length) {
        add(DiagSeverity::Error, "E-TYPE", "Không thể gán '" + fromT + "' cho '" + toT + "'.", line, col, length);
    }
    const vector<DiagnosticItem>& all() const { return items; }
    bool empty() const { return items.empty(); }
    void clear() { items.clear(); }
};
