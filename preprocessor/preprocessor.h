#pragma once
#include <string>
#include <vector>
#include <unordered_set>
using namespace std;

class Preprocessor
{
private:
    unordered_set<string> standardLibraries = {
        
        "stdio.h", "stdlib.h", "string.h", "math.h", 
        // "ctype.h", "time.h", "stdbool.h", "stdint.h",
        // "limits.h", "float.h", "assert.h", "errno.h",
    };
    
    unordered_set<string> includedLibs;

public:
    string process(const string& source, vector<string>& warnings);
    
    bool isValidLibrary(const string& libName);
    
    const unordered_set<string>& getIncludedLibraries() const;
    
    vector<string> getLibraryIdentifiers() const;
    
    void reset();
};