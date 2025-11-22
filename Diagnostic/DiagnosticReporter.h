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
    void add(DiagSeverity sev, const string &code, const string &msg, int line, int col, int len)
    {
        items.push_back({sev, code, msg, line, col, len});
    }

    void syntax(const string &msg, int line, int col, int len)
    {
        add(DiagSeverity::Error, "E1", "Lỗi cú pháp: " + msg, line, col, len);
    }

    void undeclared(const string &name, int line, int col, int len, const string &suggestion = "")
    {
        string msg = "Biến '" + name + "' chưa được khai báo";

        if (!suggestion.empty())
        {
            msg += ". Có phải ý bạn là '" + suggestion + "'?";
        }

        add(DiagSeverity::Error, "E2", msg, line, col, len);
    }

    void redeclaration(const string &name, int line, int col, int len)
    {
        add(DiagSeverity::Warning, "W1", "Biến " + name + " đã được khai báo trong phạm vi này", line, col, len);
    }

    const vector<DiagnosticItem> &all() const
    {
        return this->items;
    }
    bool empty() const
    {
        return items.empty();
    }
    void clear()
    {
        items.clear();
    }
};