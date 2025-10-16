
#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include "DiagnosticReporter.h"

inline std::string escape_json(const std::string& s) {
    std::ostringstream o;
    for (auto c = s.cbegin(); c != s.cend(); c++) {
        switch (*c) {
            case '\"': o << "\\\""; break;
            case '\\': o << "\\\\"; break;
            case '\b': o << "\\b"; break;
            case '\f': o << "\\f"; break;
            case '\n': o << "\\n"; break;
            case '\r': o << "\\r"; break;
            case '\t': o << "\\t"; break;
            default:
                if ('\x00' <= *c && *c <= '\x1f') {
                    o << "\\u"
                      << std::hex << std::setw(4) << std::setfill('0') << (int)*c;
                } else {
                    o << *c;
                }
        }
    }
    return o.str();
}

inline std::string diagnostics_to_json(const std::vector<DiagnosticItem>& items) {
    std::ostringstream out;
    out << "{\n  \"diagnostics\": [\n";
    for (size_t i = 0; i < items.size(); ++i) {
        const auto& d = items[i];
        out << "    {"
            << "\"severity\":" << (int)d.severity << ","
            << "\"code\":\"" << escape_json(d.code) << "\","
            << "\"message\":\"" << escape_json(d.message) << "\","
            << "\"line\":" << d.line << ","
            << "\"col\":" << d.col << ","
            << "\"length\":" << d.length
            << "}";
        if (i + 1 != items.size()) out << ",";
        out << "\n";
    }
    out << "  ]\n}\n";
    return out.str();
}
