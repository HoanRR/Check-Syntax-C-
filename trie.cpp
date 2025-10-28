#include "trie.h"

Trie::Trie() {
    root = new TrieNode();
}

void Trie::insert(const std::string& word) {
    TrieNode* node = root;
    for(char c : word) {
        if(!node->children.count(c))
            node->children[c] = new TrieNode();
        node = node->children[c];
    }
    node->isEnd = true;
}

bool Trie::search(const std::string& word) {
    TrieNode* node = root;
    for(char c : word) {
        if(!node->children.count(c))
            return false;
        node = node->children[c];
    }
    return node->isEnd;
}

void Trie::dfs(TrieNode* node, std::string prefix, std::vector<std::string>& result) {
    if(node->isEnd) result.push_back(prefix);
    for(auto& [c, child] : node->children)
        dfs(child, prefix + c, result);
}

std::vector<std::string> Trie::getAllWords() {
    std::vector<std::string> result;
    dfs(root, "", result);
    return result;
}
