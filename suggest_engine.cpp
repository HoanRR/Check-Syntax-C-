#include "suggest_engine.h"
#include <algorithm>

int levenshteinDistance(const std::string& s1, const std::string& s2) {
    int n = s1.size(), m = s2.size();
    std::vector<std::vector<int>> dp(n+1, std::vector<int>(m+1));
    for(int i=0;i<=n;i++) dp[i][0]=i;
    for(int j=0;j<=m;j++) dp[0][j]=j;
    for(int i=1;i<=n;i++) {
        for(int j=1;j<=m;j++) {
            if(s1[i-1]==s2[j-1]) dp[i][j]=dp[i-1][j-1];
            else dp[i][j]=1+std::min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
        }
    }
    return dp[n][m];
}

std::vector<std::string> smartSuggestList(const std::string& word, const std::vector<std::string>& dictionary, int maxSuggestions) {
    std::vector<std::pair<int,std::string>> dist;
    for(auto& w : dictionary) {
        int d = levenshteinDistance(word, w);
        if(d <= w.size()/2) // chỉ gợi ý khi khoảng cách <= 50% độ dài từ
            dist.push_back({d,w});
    }
    std::sort(dist.begin(), dist.end());
    std::vector<std::string> res;
    for(int i=0;i<std::min(maxSuggestions,(int)dist.size());i++)
        res.push_back(dist[i].second);
    return res;
}
