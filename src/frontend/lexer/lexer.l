%{
#if defined(LOGGER)
#include "pineaplog.hpp"
#endif /* defined(LOGGER) */

#include <string>
#include <cstdlib>
#include <cassert>
#include <cstring>
#include <iostream>
#include <vector>
#include <memory>
#include <cassert>
#include <sstream>
#include <source_location>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "parser.tab.hpp"
#include "global/global.hpp"
#include "global/custom_console_output.hpp"

std::string current_file;
int current_num_value;
std::string current_var_value;
int eof_reached = 0;

void process_number  (const char* text);
void process_variable(const char* text);

extern void yyerror(YYLTYPE* loc, const char* msg, std::string possible_token);

std::string find_possible_token(const char* unexpected);
size_t levenshtein_distance(const std::string& s1, const std::string& s2);

extern FILE* yyin;

/* ну я так хочу да */
ON_LOGGER(
namespace PineapLog{
static size_t tokens_counter = 0;
} /* почему не static в макросе?: макрос не всегда вызывает в одном и том же scope */
) /* ON_LOGGER */

#define YY_USER_ACTION do {                      \
    yylloc->first_line = yylloc->last_line;       \
    yylloc->first_column = yylloc->last_column;    \
    char c = '\0';                                  \
    for (size_t i = 0; (c = yytext[i]) != '\0'; ++i) \
    {                                                 \
        if (c == '\n') {                               \
            yylloc->last_line++;                        \
            yylloc->last_column = 1;                     \
            continue;                                     \
        } else if (c == '\t') {                            \
            yylloc->last_column += 4;                       \
            continue;                                        \
        } else if (c == '\r') {                               \
            continue;                                          \
        }                                                       \
        yylloc->last_column++;                                   \
    }                                                             \
    ON_LOGGER( \
        PineapLog::glog.logc(PineapLog::LogColor::White, "[", PineapLog::tokens_counter, "]"); \
        PineapLog::tokens_counter++; \
        PineapLog::LogColor color = (PineapLog::tokens_counter % 3 == 0) ? PineapLog::LogColor::Green : (PineapLog::tokens_counter % 3 == 1) ? PineapLog::LogColor::Yellow : PineapLog::LogColor::Red; \
        PineapLog::glog.logc(color, "{", "\n\tfirst line: ", yylloc->first_line, "\n\tfirst col : ", yylloc->first_column, "\n\tlast line: ", yylloc->last_line, "\n\tlast col : ", yylloc->last_column, "\n\tword = '", yytext, "'\n}"); \
    ) \
} while (0); /* YY_USER_ACTION */

%}

%option noyywrap
%option nounput
%option noinput
%option yylineno
%option bison-bridge
%option bison-locations

DIGIT       [0-9]
LETTER      [a-zA-Z_]
SPACE       [ ]
TAB         [\t]
CARRIEGE    [\r]
NEWLINE     [\n]
WHITESPACE  {SPACE}|{TAB}|{CARRIEGE}

%%

SPACE          

{SPACE}         { }
{TAB}           { }
{CARRIEGE}      { }
{WHITESPACE}+   { }
{NEWLINE}       { }

"#".*           { /* one-line comment */ }

"/*"([^*]|"*"+[^*/])*"*"+"/" { /* multy-line comment */ }

"and"     { return AND; }
"&&"      { return AND; }
"or"      { return OR; }
"||"      { return OR; }
"!"       { return NOT; }
"not "    { return NOT; }
"+"       { return ADD; }
"-"       { return SUB; }
"*"       { return MUL; }
"/"       { return DIV; }
"%"       { return REM; }
">"       { return ISAB; }
">="      { return ISABE; }
"<"       { return ISLS; }
"<="      { return ISLSE; }
"=="      { return ISEQ; }
"!="      { return ISNE; }
"+="      { return ADDASGN; }
"-="      { return SUBASGN; }
"*="      { return MULASGN; }
"/="      { return DIVASGN; }
"("       { return LCIB; }
")"       { return RCIB; }
"{"       { return LCUB; }
"}"       { return RCUB; }
"while"   { return WH; }
"?"       { return IN; }
"="       { return AS; }
"print"   { return PRINT; }
"if"      { return IF; }
"else if" { return ELIF; }
"else"    { return ELSE; }
";"       { return SC; }

[+-]?{DIGIT}+    { 
                process_number(yytext); 
                yylval->num_value = current_num_value;
                return NUM; 
            }

{LETTER}({LETTER}|{DIGIT})* { 
                process_variable(yytext); 
                yylval->str_value = new std::string(current_var_value);
                return VAR; 
            }

<<EOF>>     {
                return EXIT_SUCCESS;
            }

. {
    std::string unexpected = yytext;
    std::string possible_token = find_possible_token(yytext);
    
    yyerror(yylloc, ("unexpected character '" + unexpected + "'").c_str(), possible_token);
    return EXIT_FAILURE;
}

%%

void process_number(const char* text) {
    current_num_value = atoi(text);
}

void process_variable(const char* text) {
    current_var_value = text;
}
find_possible_token
void set_current_paracl_file(const std::string& file)
{
    current_file = file;
}

size_t levenshtein_distance(const std::string& s1, const std::string& s2)
{
    const size_t len1 = s1.size(), len2 = s2.size();
    std::vector<std::vector<size_t>> dp(len1 + 1, std::vector<size_t>(len2 + 1));
    
    for (size_t i = 0; i <= len1; ++i) dp[i][0] = i;
    for (size_t j = 0; j <= len2; ++j) dp[0][j] = j;
    
    for (size_t i = 1; i <= len1; ++i)
    {
        for (size_t j = 1; j <= len2; ++j)
        {
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

std::string find_possible_token(const char* unexpected) {
    static const std::vector<std::string> known_tokens = {
        "if", "else", "while", "print", 
        "and", "or", "not",
        "+", "-", "*", "/", "%",
        "==", "!=", ">", ">=", "<", "<=",
        "=", "+=", "-=", "*=", "/=",
        "(", ")", "{", "}", ";", "?"
    };
    
    std::string best_match;
    size_t min_distance = std::numeric_limits<size_t>::max();
    
    for (const auto& token : known_tokens) {
        size_t distance = levenshtein_distance(unexpected, token);
        if (distance < min_distance && distance <= token.length() / 2) {
            min_distance = distance;
            best_match = token;
        }
    }
    
    return best_match;
}
