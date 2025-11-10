#ifndef TRIE_H
#define TRIE_H

#include <map>
#include <vector>
#include <string>
using namespace std;
struct TrieNode {
    map<char, TrieNode*> children;
    bool isEnd;
    TrieNode() : isEnd(false) {}
};

class Trie {
private:
    TrieNode* root;
    void dfs(TrieNode* node, string prefix, vector<string>& result);

public:
    Trie();
    void insert(const string& word);
    bool search(const string& word);
    vector<string> getAllWords();
};

#endif // TRIE_H
