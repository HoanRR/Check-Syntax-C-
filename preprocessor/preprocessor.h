#pragma once
#include <string>
#include <vector>
#include <unordered_set>
using namespace std;

// Danh sách các thư viện chuẩn C được hỗ trợ
class Preprocessor
{
private:
    unordered_set<string> standardLibraries = {
        // Thư viện C chuẩn
        "stdio.h", "stdlib.h", "string.h", "math.h", 
        "ctype.h", "time.h", "stdbool.h", "stdint.h",
        "limits.h", "float.h", "assert.h", "errno.h",
        
        // Thư viện C++ phổ biến
        "iostream", "string", "vector", "map", "set",
        "algorithm", "queue", "stack", "list", "deque",
        "utility", "fstream", "sstream", "iomanip",
        "cmath", "cstring", "cctype", "cstdlib", "cstdio"
    };
    
    unordered_set<string> includedLibs;

public:
    // Xử lý các dòng #include và trả về mã đã loại bỏ #include
    string process(const string& source, vector<string>& warnings);
    
    // Kiểm tra thư viện có hợp lệ không
    bool isValidLibrary(const string& libName);
    
    // Lấy danh sách thư viện đã include
    const unordered_set<string>& getIncludedLibraries() const;
    
    vector<string> getLibraryIdentifiers() const;
    
    // Reset trạng thái
    void reset();
};