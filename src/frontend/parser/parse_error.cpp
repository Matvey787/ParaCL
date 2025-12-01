#include "parser/parse_error.hpp"
#include "parser.tab.hpp"
#include <algorithm>
#include <cstring>
#include <exception>
#include <iostream>
#include <limits>
#include <string.h>
#include <string>
#include <vector>

extern FILE *yyin;

void yy::parser::error(const location &loc, const std::string& msg)
{
    ErrorHandler::throwError(loc, msg, {false, true, false});
}

namespace ErrorHandler
{

namespace Detail
{

void show_error_context(const yy::location &loc);
size_t levenshtein_distance(std::string_view s1, std::string_view s2);
std::string find_possible_token(const char *unexpected);
std::string extract_token_at_position(const yy::location &loc);

} /* namespace Detail */

void throwError(const yy::location &loc, std::string_view msg, const ErrorParseOptions &options)
{
    std::cerr << current_file << ":" << loc.begin.line << ":" << loc.begin.column
              << ": paracl: error:"
                 " ---> "
              << msg << "\n";

    std::string bad_token = ErrorHandler::Detail::extract_token_at_position(loc);

    if (options.show_bad_token)
    {
        std::cerr << "Problematic place: '";
        bad_token.empty() ? std::cerr << "???" : std::cerr << bad_token;
        std::cerr << "'\n\n";
    }

    if (options.show_error_context)
    {
        ErrorHandler::Detail::show_error_context(loc);
    }

    if (options.show_posible_token)
    {
        std::string possible_token = ErrorHandler::Detail::find_possible_token(bad_token.c_str());
        std::cerr << "\nPossible fix: ";
        possible_token.empty() ? std::cerr << "no suggestions"
                               : std::cerr << "Did you mean: '" << possible_token << "'?";
        std::cerr << "\n";
    }
}

namespace Detail
{

void show_error_context(const yy::location &loc)
{
    if (!yyin)
        return;

    long current_file_pos = ftell(yyin);

    // Go to start of the file
    rewind(yyin);

    char buffer[1024];
    int current_line = 1;

    while (current_line < loc.begin.line && fgets(buffer, sizeof(buffer), yyin))
        current_line++;

    if (current_line == loc.begin.line && fgets(buffer, sizeof(buffer), yyin))
    {
        size_t len = strlen(buffer);

        if (len > 0 && buffer[len - 1] == '\n')
            buffer[len - 1] = '\0';

        std::cerr << loc.begin.line << " | " << buffer << std::endl;

        std::cerr << "  | ";
        int i = 1;
        for (; i < loc.begin.column; i++)
        {
            if (i < (int)strlen(buffer) && buffer[i - 1] == '\t')
                std::cerr << "~~~~";
            else
                std::cerr << "~";
        }
        std::cerr << "^";

        for (; i < (int)strlen(buffer); i++)
            std::cerr << "~";

        std::cerr << "\n";
    }

    // Restoring the position in the file
    fseek(yyin, current_file_pos, SEEK_SET);
}

size_t levenshtein_distance(std::string_view s1, std::string_view s2)
{
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<size_t>> dp(len1 + 1, std::vector<size_t>(len2 + 1));

    for (size_t i = 1; i <= len1; ++i)
        dp[i][0] = i;
    for (size_t j = 1; j <= len2; ++j)
        dp[0][j] = j;

    for (size_t i = 1; i <= len1; ++i)
    {
        for (size_t j = 1; j <= len2; ++j)
        {
            size_t cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            dp[i][j] = std::min({dp[i - 1][j] + 1, dp[i][j - 1] + 1, dp[i - 1][j - 1] + cost});
        }
    }

    return dp[len1][len2];
}

std::string find_possible_token(const char *unexpected)
{
    if (strlen(unexpected) == 0)
        return "";

    static const std::vector<std::string> known_tokens = {
        "if", "else if", "else", "while", "print", "&&", "||", "!",  "and", "or", "not", "+", "-", "*", "/", "%", "==",
        "!=", ">",       ">=",   "<",     "<=",    "=",  "+=", "-=", "*=",  "/=", "%=",  "(", ")", "{", "}", ";", "?",
    };

    std::string best_match;
    size_t min_distance = std::numeric_limits<size_t>::max();

    for (const auto &token : known_tokens)
    {
        size_t distance = levenshtein_distance(unexpected, token);
        if (distance < min_distance && distance <= token.length())
        {
            min_distance = distance;
            best_match = token;
        }
    }

    return best_match;
}

std::string extract_token_at_position(const yy::location &loc)
{
    if (!yyin)
        return "";

    long current_file_pos = ftell(yyin);

    rewind(yyin);

    char buffer[1024];
    int current_line = 1;
    std::string target_line;

    while (current_line < loc.begin.line && fgets(buffer, sizeof(buffer), yyin))
    {
        ++current_line;
    }

    if (current_line == loc.begin.line && fgets(buffer, sizeof(buffer), yyin))
    {
        target_line = buffer;
    }

    fseek(yyin, current_file_pos, SEEK_SET);

    if (target_line.empty())
        return "";

    // try to form token around the given position
    size_t pos = loc.begin.column - 1;

    if (pos >= target_line.length())
        return "";

    // go left from problem pos
    size_t start = pos;
    while (start > 0 && !isspace(target_line[start - 1]))
    {
        --start;
    }

    // find end of token (go right until space or end of line)
    size_t end = pos;
    while (end < target_line.length() && !isspace(target_line[end]) && target_line[end] != '\0' &&
           target_line[end] != '\n' && target_line[end] != '\r')
    {
        ++end;
    }

    std::string token = target_line.substr(start, end - start);

    while (!token.empty() && (token.front() == '(' || token.front() == ')' || token.front() == '{' ||
                              token.front() == '}' || token.front() == ';' || token.front() == ','))
    {
        token.erase(0, 1);
        ++start;
    }

    while (!token.empty() && (token.back() == '(' || token.back() == ')' || token.back() == '{' ||
                              token.back() == '}' || token.back() == ';' || token.back() == ','))
    {
        token.pop_back();
        --end;
    }

    return token;
}

} /* namespace Detail */

} /* namespace ErrorHandler */
