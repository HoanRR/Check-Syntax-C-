#pragma once
#include "DiagnosticReporter.h"
#include <string>
#include <sstream>
using namespace std;

string Diagnostic_to_JSON(const vector<DiagnosticItem> &items)
{
    ostringstream o;
    o << "{\n  \"diagnostic\": [\n";
    for (const auto &d : items)
    {
        o << "   {"
          << "\"severity\":" << (int)d.severity << ","
          << "\"code\":\"" << d.code << "\","
          << "\"message\":\"" << d.message << "\","
          << "\"line\":" << d.line << ","
          << "\"column\":" << d.col << ","
          << "\"length\":" << d.length
          << "}";
    }
    o << "\n  ]\n}";
    return o.str();
}
