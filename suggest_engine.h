#ifndef SUGGEST_ENGINE_H
#define SUGGEST_ENGINE_H

#include <vector>
#include <string>
using namespace std;
int levenshteinDistance(const string& s1, const string& s2);
vector<string> smartSuggestList(const string& word, const vector<string>& dictionary, int maxSuggestions);

#endif // SUGGEST_ENGINE_H
