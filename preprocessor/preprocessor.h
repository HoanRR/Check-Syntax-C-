#pragma once
#include <string>
#include <vector>
#include "../Diagnostic/DiagnosticReporter.h"
#include <unordered_set>
using namespace std;

class Preprocessor
{
private:
    unordered_set<string> standardLibraries = {
        
        "stdio.h", "stdlib.h", "string.h", "math.h", 
    };
    DiagnosticReporter *diag;
    unordered_set<string> includedLibs;

public:
    void setDiagnosticReporter(DiagnosticReporter*);
    string process(const string&  );
    
    bool isValidLibrary(const string& );
    
    const unordered_set<string>& getIncludedLibraries() const;
    
    vector<string> getLibraryIdentifiers() const;
    
    void reset();
};