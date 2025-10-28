#ifndef SUGGEST_ENGINE_H
#define SUGGEST_ENGINE_H

#include <vector>
#include <string>

int levenshteinDistance(const std::string& s1, const std::string& s2);
std::vector<std::string> smartSuggestList(const std::string& word, const std::vector<std::string>& dictionary, int maxSuggestions);

#endif // SUGGEST_ENGINE_H
