#ifndef TRIE_H
#define TRIE_H

#include <map>
#include <vector>
#include <string>

struct TrieNode {
    std::map<char, TrieNode*> children;
    bool isEnd;
    TrieNode() : isEnd(false) {}
};

class Trie {
private:
    TrieNode* root;
    void dfs(TrieNode* node, std::string prefix, std::vector<std::string>& result);

public:
    Trie();
    void insert(const std::string& word);
    bool search(const std::string& word);
    std::vector<std::string> getAllWords();
};

#endif // TRIE_H
