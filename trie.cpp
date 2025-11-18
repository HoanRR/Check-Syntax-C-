#include "trie.h"
#include <algorithm>
#include <cctype>
#include <queue>
#include <cmath>

Trie::Trie()
{
    root = new TrieNode();
}

Trie::~Trie()
{
    deleteTrie(root);
}

void Trie::deleteTrie(TrieNode* node)
{
    if (!node) return;
    for (auto& [c, child] : node->children)
        deleteTrie(child);
    delete node;
}

void Trie::insert(const string &word)
{
    if (word.empty()) return;
    
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
    node->originalWord = word;  // Lưu từ gốc để hiển thị
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

// Thu thập từ với prefix (sử dụng cho autocomplete đơn giản)
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
    if (prefix.empty()) return result;
    
    TrieNode *node = root;
    string normalizedPrefix;

    // Di chuyển đến node của prefix (normalized)
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

// ==================== A* SEARCH IMPLEMENTATION ====================

// State cho A* search
struct AStarState
{
    TrieNode* node;           // Node hiện tại trong Trie
    string currentWord;       // Từ đang xây dựng
    int inputPos;             // Vị trí trong input word
    int gCost;                // Chi phí thực tế (số thao tác đã thực hiện)
    int hCost;                // Chi phí ước lượng (heuristic)
    int fCost;                // Tổng chi phí f = g + h
    
    AStarState(TrieNode* n, string word, int pos, int g, int h)
        : node(n), currentWord(word), inputPos(pos), gCost(g), hCost(h), fCost(g + h) {}
    
    // So sánh cho priority queue (min heap)
    bool operator>(const AStarState& other) const
    {
        if (fCost != other.fCost)
            return fCost > other.fCost;
        // Ưu tiên state có từ ngắn hơn nếu fCost bằng nhau
        return currentWord.size() > other.currentWord.size();
    }
};

// Heuristic: ước lượng chi phí còn lại
static int calculateHeuristic(const string& input, int inputPos, const string& currentWord)
{
    int remainingInputChars = input.size() - inputPos;
    return remainingInputChars;
}

// Tính chi phí chính xác giữa input và candidate
static int calculateEditDistance(const string& input, const string& candidate)
{
    int n = input.size();
    int m = candidate.size();

    vector<int> prev(m + 1), curr(m + 1);
    
    for (int j = 0; j <= m; j++)
        prev[j] = j;
    
    for (int i = 1; i <= n; i++)
    {
        curr[0] = i;
        for (int j = 1; j <= m; j++)
        {
            if (tolower(input[i - 1]) == tolower(candidate[j - 1]))
                curr[j] = prev[j - 1];
            else
                curr[j] = 1 + min({prev[j], curr[j - 1], prev[j - 1]});
        }
        swap(prev, curr);
    }
    
    return prev[m];
}

static int calculateRankingScore(const string& input, const string& candidate)
{
    // Tính prefix match length
    int prefixLen = 0;
    size_t minLen = min(input.size(), candidate.size());
    
    for (size_t i = 0; i < minLen; i++)
    {
        if (tolower(input[i]) == tolower(candidate[i]))
            prefixLen++;
        else
            break;
    }
    
    // Perfect prefix match được ưu tiên cao nhất
    if (prefixLen == input.size())
    {
        return -10000 + (candidate.size() - input.size());
    }
    
    int distance = calculateEditDistance(input, candidate);
    int lengthDiff = abs((int)candidate.size() - (int)input.size());
    
    int score = distance * 100;        // Trọng số chính
    score -= prefixLen * 300;          // Bonus lớn cho prefix match
    score += lengthDiff * 10;          // Penalty nhẹ cho độ dài khác biệt
    
    return score;
}


vector<string> Trie::findSimilarWords(const string &word, int maxSuggestions)
{
    if (word.empty() || !root)
        return {};
    
    // Normalize input
    string normalizedInput;
    for (char c : word)
        normalizedInput += tolower(c);
    
    // Bước 1: Thử tìm với prefix trước (nhanh nhất)
    vector<string> prefixMatches = findWordsWithPrefix(normalizedInput, maxSuggestions);
    if (!prefixMatches.empty())
    {
        return prefixMatches;
    }
    
    // Bước 2: Sử dụng A* search để tìm từ tương tự
    
    // Priority queue cho A*
    priority_queue<AStarState, vector<AStarState>, greater<AStarState>> openSet;
    
    // Set để lưu các từ đã tìm thấy (tránh trùng lặp)
    unordered_set<string> visited;
    vector<pair<int, string>> candidates; // <score, word>
    
    // Khởi tạo với root node
    int initialH = calculateHeuristic(normalizedInput, 0, "");
    openSet.push(AStarState(root, "", 0, 0, initialH));
    
    // Ngưỡng khoảng cách tối đa
    int maxDistance;
    if (normalizedInput.size() <= 2)
        maxDistance = 2;
    else if (normalizedInput.size() <= 4)
        maxDistance = 3;
    else
        maxDistance = max(3, (int)(normalizedInput.size() * 0.5));
    
    int statesExplored = 0;
    const int MAX_STATES = 10000; // Giới hạn để tránh timeout
    
    while (!openSet.empty() && statesExplored < MAX_STATES)
    {
        AStarState current = openSet.top();
        openSet.pop();
        statesExplored++;
        
        // Kiểm tra nếu đã tìm đủ suggestions
        if (candidates.size() >= maxSuggestions * 3) // Lấy thừa để filter sau
        {
            // Pruning: nếu fCost hiện tại > worst candidate, dừng
            if (!candidates.empty())
            {
                int worstScore = candidates.back().first;
                if (current.fCost * 100 > worstScore + 5000)
                    break;
            }
        }
        
        // Nếu đến cuối từ trong Trie
        if (current.node->isEnd)
        {
            string candidateWord = current.node->originalWord.empty() 
                                   ? current.currentWord 
                                   : current.node->originalWord;
            
            // Tính edit distance thực tế
            int actualDistance = calculateEditDistance(normalizedInput, current.currentWord);
            
            if (actualDistance <= maxDistance && visited.find(candidateWord) == visited.end())
            {
                visited.insert(candidateWord);
                int score = calculateRankingScore(normalizedInput, current.currentWord);
                candidates.push_back({score, candidateWord});
            }
        }
        
        // Pruning: nếu gCost đã quá lớn, bỏ qua
        if (current.gCost > maxDistance + 2)
            continue;
        
        // Mở rộng các state con
        
        // 1. Match: tiến cả input và trie
        if (current.inputPos < normalizedInput.size())
        {
            char inputChar = normalizedInput[current.inputPos];
            
            if (current.node->children.count(inputChar))
            {
                TrieNode* nextNode = current.node->children[inputChar];
                string nextWord = current.currentWord + inputChar;
                int nextPos = current.inputPos + 1;
                int g = current.gCost; // Không tốn chi phí khi match
                int h = calculateHeuristic(normalizedInput, nextPos, nextWord);
                
                openSet.push(AStarState(nextNode, nextWord, nextPos, g, h));
            }
        }
        
        // 2. Substitution: thay thế ký tự
        if (current.inputPos < normalizedInput.size())
        {
            for (auto& [c, child] : current.node->children)
            {
                if (c != normalizedInput[current.inputPos])
                {
                    string nextWord = current.currentWord + c;
                    int nextPos = current.inputPos + 1;
                    int g = current.gCost + 1; // Chi phí thay thế
                    int h = calculateHeuristic(normalizedInput, nextPos, nextWord);
                    
                    if (g <= maxDistance) // Pruning
                        openSet.push(AStarState(child, nextWord, nextPos, g, h));
                }
            }
        }
        
        // 3. Insertion: thêm ký tự vào candidate (không tiến input)
        for (auto& [c, child] : current.node->children)
        {
            string nextWord = current.currentWord + c;
            int g = current.gCost + 1;
            int h = calculateHeuristic(normalizedInput, current.inputPos, nextWord);
            
            if (g <= maxDistance && nextWord.size() <= normalizedInput.size() + maxDistance)
                openSet.push(AStarState(child, nextWord, current.inputPos, g, h));
        }
        
        // 4. Deletion: bỏ qua ký tự input (tiến input, không tiến trie)
        if (current.inputPos < normalizedInput.size())
        {
            int g = current.gCost + 1;
            int h = calculateHeuristic(normalizedInput, current.inputPos + 1, current.currentWord);
            
            if (g <= maxDistance)
                openSet.push(AStarState(current.node, current.currentWord, current.inputPos + 1, g, h));
        }
    }
    
    // Sắp xếp candidates theo score
    sort(candidates.begin(), candidates.end());
    
    // Lấy top suggestions
    vector<string> result;
    for (int i = 0; i < min(maxSuggestions, (int)candidates.size()); i++)
    {
        result.push_back(candidates[i].second);
    }
    
    return result;
}