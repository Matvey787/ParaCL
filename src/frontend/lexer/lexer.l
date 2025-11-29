%{
#include <string>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <sstream>
#include "parser/parse_error.hpp"
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "global/global.hpp"

#include "log/log_api.hpp"

std::string current_file;

#define YY_DECL int yylex(yy::parser::semantic_type* yylval, yy::parser::location_type* yylloc)


/* ну я так хочу да */
ON_LOGGER(
namespace PineapLog{
static size_t tokens_counter = 0;
} /* почему не static в макросе?: макрос не всегда вызывает в одном и том же scope */
) /* ON_LOGGER */

#define YY_USER_ACTION do {                      \
    yylloc->begin.line = yylloc->end.line;        \
    yylloc->begin.column = yylloc->end.column;     \
    char c = '\0';                                  \
    for (size_t i = 0; (c = yytext[i]) != '\0'; ++i) \
    {                                                 \
        if (c == '\n') {                               \
            yylloc->end.line++;                         \
            yylloc->end.column = 1;                      \
            continue;                                     \
        } else if (c == '\t') {                            \
            yylloc->end.column += 4;                        \
            continue;                                        \
        } else if (c == '\r') {                               \
            continue;                                          \
        }                                                       \
        yylloc->end.column++;                                    \
    }                                                             \
    LOGINFO("paracl: lexer: parse token: '{}:{}:{}':\"{}\"", current_file, yylloc->begin.line, yylloc->begin.column, yytext); \
    HTMLCOLLOG(PineapLog::LogColor::White, "[", PineapLog::tokens_counter++, "]"); \
    HTMLCOLLOG((PineapLog::tokens_counter % 3 == 0) ? \
                PineapLog::LogColor::Green : (PineapLog::tokens_counter % 3 == 1) ? \
                PineapLog::LogColor::Yellow : PineapLog::LogColor::Red, \
                "{", "\n\tfirst line: ", yylloc->begin.line, \
                     "\n\tfirst col : ", yylloc->begin.column, \
                     "\n\tlast line: ", yylloc->end.line, \
                     "\n\tlast col : ", yylloc->end.column, \
                     "\n\tword = '", yytext, "'\n}"); \
} while (0);

%}

%option noyywrap
%option nounput
%option noinput
%option yylineno


DIGIT       [0-9]
LETTER      [a-zA-Z_]
SPACE       [ ]
TAB         [\t]
CARRIEGE    [\r]
NEWLINE     [\n]
WHITESPACE  {SPACE}|{TAB}|{CARRIEGE}
STRING      \"[^"]*\"

%%

{SPACE}         { }
{TAB}           { }
{CARRIEGE}      { }
{WHITESPACE}+   { }
{NEWLINE}       { }

"//".*           { /* one-line comment */ }
"/*"([^*]|"*"+[^*/])*"*"+"/" { /* multy-line comment */ }
"#!".* { /* shebang */ }
"and"     { return yy::parser::token::AND; }
"&&"      { return yy::parser::token::AND; }
"or"      { return yy::parser::token::OR; }
"||"      { return yy::parser::token::OR; }
"!"       { return yy::parser::token::NOT; }
"not"     { return yy::parser::token::NOT; }
"+"       { return yy::parser::token::ADD; }
"-"       { return yy::parser::token::SUB; }
"*"       { return yy::parser::token::MUL; }
"/"       { return yy::parser::token::DIV; }
"%"       { return yy::parser::token::REM; }
">"       { return yy::parser::token::ISAB; }
">="      { return yy::parser::token::ISABE; }
"<"       { return yy::parser::token::ISLS; }
"<="      { return yy::parser::token::ISLSE; }
"=="      { return yy::parser::token::ISEQ; }
"!="      { return yy::parser::token::ISNE; }
"+="      { return yy::parser::token::ADDASGN; }
"-="      { return yy::parser::token::SUBASGN; }
"*="      { return yy::parser::token::MULASGN; }
"/="      { return yy::parser::token::DIVASGN; }
"("       { return yy::parser::token::LCIB; }
")"       { return yy::parser::token::RCIB; }
"{"       { return yy::parser::token::LCUB; }
"}"       { return yy::parser::token::RCUB; }
"while"   { return yy::parser::token::WH; }
"?"       { return yy::parser::token::IN; }
"="       { return yy::parser::token::AS; }
"print"   { return yy::parser::token::PRINT; }
"if"      { return yy::parser::token::IF; }
"else if" { return yy::parser::token::ELIF; }
"else"    { return yy::parser::token::ELSE; }
";"       { return yy::parser::token::SC; }
","       { return yy::parser::token::COMMA; }
{STRING}    { 
                std::ostringstream result;

                for (size_t it = 1, ite = yyleng - 1; it < ite; ++it)
                {
                    const char c = *(yytext + it);
                    const char next_c = *(yytext + it + 1);

                    if (c != '\\' && next_c != '\0')
                    {
                        result << c;
                        continue;
                    }

                    switch (next_c)
                    {
                        case 'n':  result << '\n'; break;
                        case 't':  result << '\t'; break;
                        case 'r':  result << '\r'; break;
                        case '\\': result << '\\'; break;
                        case '\"': result << '\"'; break;
                        case '0':  result << '\0'; break;
                        case 'a':  result << '\a'; break;
                        case 'b':  result << '\b'; break;
                        case 'v':  result << '\v'; break;
                        case 'f':  result << '\f'; break;
                        default:   result << c   ; continue;
                    }
                    ++it;
                }
                yylval->build<std::string>(std::move(result.str()));
                return yy::parser::token::STRING;
            }

{DIGIT}+    {
                yylval->build<int>(atoi(yytext));
                return yy::parser::token::NUM;
            }

{LETTER}({LETTER}|{DIGIT})* {
                yylval->build<std::string>(yytext);
                return yy::parser::token::VAR;
            }

<<EOF>>     { return yy::parser::token::YYEOF; }

.           { 
                ErrorHandler::throwError(*yylloc,
                                         "unexpected character '" + std::string(yytext) + "'",
                                         {1, 1, 1});
                return yy::parser::token::YYerror;
            }

%%

void set_current_paracl_file(const std::string& file)
{
    current_file = file;
}
