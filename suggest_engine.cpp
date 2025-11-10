#include "suggest_engine.h"
#include <algorithm>
using namespace std;
int levenshteinDistance(const string& s1, const string& s2) {
    int n = s1.size(), m = s2.size();
    vector<vector<int>> dp(n+1, vector<int>(m+1));
    for(int i=0;i<=n;i++) dp[i][0]=i;
    for(int j=0;j<=m;j++) dp[0][j]=j;
    for(int i=1;i<=n;i++) {
        for(int j=1;j<=m;j++) {
            if(s1[i-1]==s2[j-1]) dp[i][j]=dp[i-1][j-1];
            else dp[i][j]=1+min({dp[i-1][j], dp[i][j-1], dp[i-1][j-1]});
        }
    }
    return dp[n][m];
}

vector<string> smartSuggestList(const string& word, const vector<string>& dictionary, int maxSuggestions) {
    vector<pair<int,string>> dist;
    for(auto& w : dictionary) {
        int d = levenshteinDistance(word, w);
        if(d <= w.size()/2) // chỉ gợi ý khi khoảng cách <= 50% độ dài từ
            dist.push_back({d,w});
    }
    sort(dist.begin(), dist.end());
    vector<string> res;
    for(int i=0;i<min(maxSuggestions,(int)dist.size());i++)
        res.push_back(dist[i].second);
    return res;
}
