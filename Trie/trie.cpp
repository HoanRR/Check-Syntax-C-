#include "trie.h"
#include <algorithm>
#include <cctype>
#include <queue>
#include <cmath>
#include <functional>

Trie::Trie()
{
    root = new TrieNode();
}

Trie::~Trie()
{
    deleteTrie(root);
}

void Trie::deleteTrie(TrieNode *node)
{
    if (!node)
        return;
    for (auto &[c, child] : node->children)
        deleteTrie(child);
    delete node;
}

void Trie::insert(const string &word)
{
    if (word.empty())
        return;

    TrieNode *node = root;
    string normalized;

    for (char c : word)
    {
        char lowerC = tolower(c);
        normalized += lowerC;

        if (!node->children.count(lowerC))
            node->children[lowerC] = new TrieNode();
        node = node->children[lowerC];
    }
    node->isEnd = true;
    node->originalWord = word; // Lưu từ gốc để hiển thị
}

bool Trie::search(const string &word)
{
    TrieNode *node = root;
    for (char c : word)
    {
        char lowerC = tolower(c);
        if (!node->children.count(lowerC))
            return false;
        node = node->children[lowerC];
    }
    return node->isEnd;
}

void Trie::dfs(TrieNode *node, string prefix, vector<string> &result)
{
    if (node->isEnd)
        result.push_back(node->originalWord.empty() ? prefix : node->originalWord);

    for (auto &[c, child] : node->children)
        dfs(child, prefix + c, result);
}

vector<string> Trie::getAllWords()
{
    vector<string> result;
    dfs(root, "", result);
    return result;
}

void Trie::collectWordsWithPrefix(TrieNode *node, string prefix, vector<string> &result, int limit)
{
    if (result.size() >= limit)
        return;

    if (node->isEnd)
    {
        result.push_back(node->originalWord.empty() ? prefix : node->originalWord);
    }

    if (result.size() >= limit)
        return;

    for (auto &[c, child] : node->children)
    {
        collectWordsWithPrefix(child, prefix + c, result, limit);
        if (result.size() >= limit)
            return;
    }
}

vector<string> Trie::findWordsWithPrefix(const string &prefix, int limit)
{
    vector<string> result;
    if (prefix.empty())
        return result;

    TrieNode *node = root;
    string normalizedPrefix;

    for (char c : prefix)
    {
        char lowerC = tolower(c);
        normalizedPrefix += lowerC;

        if (!node->children.count(lowerC))
            return result; // Không tìm thấy prefix

        node = node->children[lowerC];
    }

    // Thu thập tất cả từ từ node này
    collectWordsWithPrefix(node, normalizedPrefix, result, limit);

    return result;
}

// Helper function để wrap findWordsWithPrefix cho A* search
static vector<string> findWordsWithPrefixWrapper(TrieNode* root, const string& prefix, int limit)
{
    // Tạo Trie tạm để gọi findWordsWithPrefix
    // Note: Đây là wrapper function, thực tế A* sẽ gọi trực tiếp
    vector<string> result;
    if (prefix.empty())
        return result;

    TrieNode *node = root;
    string normalizedPrefix;

    for (char c : prefix)
    {
        char lowerC = tolower(c);
        normalizedPrefix += lowerC;

        if (!node->children.count(lowerC))
            return result;

        node = node->children[lowerC];
    }

    // Thu thập từ đây (simplified version)
    function<void(TrieNode*, string)> collect = [&](TrieNode* n, string p) {
        if (result.size() >= (size_t)limit) return;
        if (n->isEnd) {
            result.push_back(n->originalWord.empty() ? p : n->originalWord);
        }
        for (auto &[c, child] : n->children) {
            collect(child, p + c);
            if (result.size() >= (size_t)limit) return;
        }
    };
    
    collect(node, normalizedPrefix);
    return result;
}

vector<string> Trie::findSimilarWords(const string &word, int maxSuggestions)
{
    return findSimilarWordsAStar(root, word, maxSuggestions, findWordsWithPrefixWrapper);
}