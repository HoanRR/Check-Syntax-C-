#include <string>
#include <vector>
#include <unordered_map>

using namespace std;

struct TrieNode;

// State cho A* search
struct AStarState
{
    TrieNode *node;     // Node hiện tại trong Trie
    string currentWord; // Từ đang xây dựng
    int inputPos;       // Vị trí trong input word
    int gCost;          // Chi phí thực tế (số thao tác đã thực hiện)
    int hCost;          // Chi phí ước lượng (heuristic)
    int fCost;          // Tổng chi phí f = g + h

    AStarState(TrieNode *n, string word, int pos, int g, int h);

    bool operator>(const AStarState &other) const;
};

int calculateHeuristic(const string &input, int inputPos);

int calculateEditDistance(const string &input, const string &candidate);

int calculateRankingScore(const string &input, const string &candidate);

vector<string> findSimilarWordsAStar(
    TrieNode *root,
    const string &word,
    int maxSuggestions,
    vector<string> (*findWordsWithPrefix)(TrieNode *, const string &, int));
