#include "DiagnosticReporter.h"

void DiagnosticReporter::add(DiagSeverity sev, const string &code, const string &msg, int line, int col, int len)
{
    items.push_back({sev, code, msg, line, col, len});
}

void DiagnosticReporter::syntax(const string &msg, int line, int col, int len)
{
    add(DiagSeverity::Error, "E1", "Lỗi cú pháp: " + msg, line, col, len);
}

void DiagnosticReporter::undeclared(const string &name, int line, int col, int len, const string &suggestion)
{
    string msg = "Biến '" + name + "' chưa được khai báo";

    if (!suggestion.empty())
    {
        msg += ". Có phải ý bạn là '" + suggestion + "'?";
    }

    add(DiagSeverity::Error, "E2", msg, line, col, len);
}

void DiagnosticReporter::redeclaration(const string &name, int line, int col, int len)
{
    add(DiagSeverity::Warning, "W1", "Biến " + name + " đã được khai báo trong phạm vi này", line, col, len);
}

const vector<DiagnosticItem> &DiagnosticReporter::all() const
{
    return this->items;
}

bool DiagnosticReporter::empty() const
{
    return items.empty();
}

void DiagnosticReporter::clear()
{
    items.clear();
}