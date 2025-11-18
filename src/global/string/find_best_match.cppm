/* DEEPSEEK FULL REALISATION :) */
module;

#include <cstddef>
#include <vector>
#include <string>
#include <algorithm>
#include <limits>

export module find_best_match;

export namespace FindBestMath {

}

size_t levenshtein_distance(const std::string& s1, const std::string& s2)
{
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<size_t>> dp(len1 + 1, std::vector<size_t>(len2 + 1));
    
    for (size_t i = 0; i <= len1; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= len2; ++j) dp[0][j] = j;
    
    for (size_t i = 1; i <= len1; ++i) {
        for (size_t j = 1; j <= len2; ++j) {
            size_t cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
            dp[i][j] = std::min(
            {
                dp[i-1][j] + 1,      // deletion
                dp[i][j-1] + 1,      // insertion  
                dp[i-1][j-1] + cost  // substitution
            });
        }
    }
    
    return dp[len1][len2];
}

double similarity_percentage(const std::string& s1, const std::string& s2) {
    size_t max_len = std::max(s1.length(), s2.length());
    if (max_len == 0) return 100.0;
    
    size_t distance = levenshtein_distance(s1, s2);
    return (1.0 - static_cast<double>(distance) / max_len) * 100.0;
}

std::string find_best_match(const std::string& target, 
                           const std::vector<std::string>& candidates) {
    if (candidates.empty()) return "";
    
    size_t min_distance = 100000;
    std::string best_match;
    
    for (const auto& candidate : candidates)
    {
        size_t distance = levenshtein_distance(target, candidate);
        if (distance < min_distance)
        {
            min_distance = distance;
            best_match = candidate;
        }
    }
    
    // Фильтр: если расстояние слишком большое, не предлагать
    size_t max_len = std::max<size_t>(target.length(), best_match.length());
    if (min_distance > max_len / 2)
        return "";
    
    return best_match;
}