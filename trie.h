// trie.h
#ifndef TRIE_H
#define TRIE_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

using namespace std;

struct TrieNode
{
    unordered_map<char, TrieNode*> children;
    bool isEnd = false;
    string originalWord; // Lưu từ gốc với chữ hoa/thường gốc
};

class Trie
{
private:
    TrieNode* root;
    
    // Các hàm helper private
    void dfs(TrieNode* node, string prefix, vector<string>& result);
    void collectWordsWithPrefix(TrieNode* node, string prefix, vector<string>& result, int limit);
    void deleteTrie(TrieNode* node);

public:
    // Constructor & Destructor
    Trie();
    ~Trie();
    
    // Các hàm công khai
    void insert(const string& word);
    bool search(const string& word);
    vector<string> getAllWords();
    vector<string> findWordsWithPrefix(const string& prefix, int limit = 10);
    
    // A* Search cho fuzzy matching
    vector<string> findSimilarWords(const string& word, int maxSuggestions = 5);
};

#endif // TRIE_H