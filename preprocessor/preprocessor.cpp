#include "preprocessor.h"
#include <sstream>
#include <algorithm>

string Preprocessor::process(const string &source, vector<string> &warnings)
{
    reset();
    stringstream result;
    istringstream iss(source);
    string line;
    int lineNum = 1;

    while (getline(iss, line))
    {
        // Trim leading spaces
        size_t start = line.find_first_not_of(" \t");
        if (start == string::npos)
        {
            result << "\n";
            lineNum++;
            continue;
        }

        string trimmed = line.substr(start);

        // Kiểm tra xem có phải dòng #include không
        if (trimmed.size() > 8 && trimmed.substr(0, 8) == "#include")
        {
            string rest = trimmed.substr(8);

            // Tìm tên thư viện
            size_t openBracket = rest.find('<');
            size_t closeBracket = rest.find('>');
            size_t openQuote = rest.find('"');
            size_t closeQuote = rest.rfind('"');

            string libName;

            if (openBracket != string::npos && closeBracket != string::npos && closeBracket > openBracket)
            {
                // #include <stdio.h>
                libName = rest.substr(openBracket + 1, closeBracket - openBracket - 1);
            }
            else if (openQuote != string::npos && closeQuote != string::npos && closeQuote > openQuote)
            {
                // #include "myheader.h"
                libName = rest.substr(openQuote + 1, closeQuote - openQuote - 1);
            }
            else
            {
                warnings.push_back("Dòng " + to_string(lineNum) + ": Cú pháp #include không hợp lệ");
                result << "\n";
                lineNum++;
                continue;
            }

            // Trim spaces từ tên thư viện
            libName.erase(0, libName.find_first_not_of(" \t"));
            libName.erase(libName.find_last_not_of(" \t") + 1);

            // Kiểm tra thư viện có hợp lệ không
            if (isValidLibrary(libName))
            {
                includedLibs.insert(libName);
            }
            else
            {
                warnings.push_back("Dòng " + to_string(lineNum) + ": Thư viện '" + libName + "' không được hỗ trợ");
            }

            // Thay thế dòng #include bằng dòng trống để giữ số dòng
            result << "\n";
        }
        else
        {
            // Giữ nguyên các dòng khác
            result << line << "\n";
        }

        lineNum++;
    }

    return result.str();
}

bool Preprocessor::isValidLibrary(const string &libName)
{
    return standardLibraries.find(libName) != standardLibraries.end();
}

const unordered_set<string> &Preprocessor::getIncludedLibraries() const
{
    return includedLibs;
}

vector<string> Preprocessor::getLibraryIdentifiers() const
{
    vector<string> identifiers;

    // Thêm identifiers từ các thư viện đã include
    for (const auto &lib : includedLibs)
    {
        if (lib == "iostream")
        {
            identifiers.insert(identifiers.end(), {"cout", "cin", "cerr", "clog", "endl"});
        }
        else if (lib == "stdio.h" || lib == "cstdio")
        {
            identifiers.insert(identifiers.end(), {"printf", "scanf", "fprintf", "fscanf", "sprintf", "sscanf",
                                                   "getchar", "putchar", "puts", "FILE"});
        }
        else if (lib == "math.h" || lib == "cmath")
        {
            identifiers.insert(identifiers.end(), {"sin", "cos", "tan", "asin", "acos", "atan", "atan2",
                                                   "sqrt", "pow", "exp", "log", "log10", "ceil", "floor",
                                                   "fabs", "abs", "round"});
        }
        else if (lib == "algorithm")
        {
            identifiers.insert(identifiers.end(), {"sort", "reverse", "max", "min", "swap",
                                                   "find", "binary_search", "lower_bound", "upper_bound"});
        }
    }

    return identifiers;
}

void Preprocessor::reset()
{
    includedLibs.clear();
}