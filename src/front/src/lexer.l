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
#include "paraCL_crutch_for_parsery.hpp"
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

#define YY_USER_ACTION \
    do { \
        yylloc->first_line = yylloc->last_line; \
        yylloc->first_column = yylloc->last_column; \
        for (int i = 0; i < yyleng; i++) { \
            if (yytext[i] == '\n') { \
                yylloc->last_line++; \
                yylloc->last_column = 0; \
            } else { \
                yylloc->last_column++; \
            } \
        } \
    } while (0);


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

"#".*          { }

"/*"([^*]|"*"+[^*/])*"*"+"/" { }

"and"     { return AND; }
"or"      { return OR; }
"+"       { return ADD; }
"-"       { return SUB; }
"*"       { return MUL; }
"/"       { return DIV; }
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
