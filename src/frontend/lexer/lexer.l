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

extern void yyerror(YYLTYPE* loc, const char* msg);

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
    std::ostringstream oss;
    oss << "use of undeclared identifier '" << yytext << "'";

    yyerror(yylloc, oss.str().c_str());
    return EXIT_FAILURE;
}

%%

void process_number(const char* text) {
    current_num_value = atoi(text);
}

void process_variable(const char* text) {
    current_var_value = text;
}

void set_current_paracl_file(const std::string& file)
{
    current_file = file;
}