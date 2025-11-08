%{
#include "token_t.hpp"
#include "parser.tab.hpp"
#include <cstdlib>
#include <cstring>
#include <iostream>

int current_line = 1;

int current_num_value;
std::string current_var_value;

void process_number(const char* text);
void process_variable(const char* text);

#define YY_DECL int yylex()

extern FILE* yyin;

int eof_reached = 0;
%}

%option noyywrap
%option nounput
%option noinput

DIGIT       [0-9]
LETTER      [a-zA-Z_]
WHITESPACE  [ \t\r]
NEWLINE     \n

%%

{WHITESPACE}+   { }
{NEWLINE}       { current_line++; }

"+"     { return ADD; }
"-"     { return SUB; }
"*"     { return MUL; }
"/"     { return DIV; }

">"     { return ISAB; }
">="    { return ISABE; }
"<"     { return ISLS; }
"<="    { return ISLSE; }
"=="    { return ISEQ; }

"("     { return LCIB; }
")"     { return RCIB; }
"{"     { return LCUB; }
"}"     { return RCUB; }

"while" { return WH; }
"?"     { return IN; }
"="     { return AS; }
"print" { return PRINT; }

";"     { return SC; }

{DIGIT}+    { 
                process_number(yytext); 
                yylval.num_value = current_num_value;
                return NUM; 
            }
{LETTER}({LETTER}|{DIGIT})* { 
                process_variable(yytext); 
                yylval.str_value = new std::string(current_var_value);
                return VAR; 
            }

<<EOF>>     { 
                return 0;
            }

.       { 
            std::cerr << "Error at line " << current_line << ": unknown character '" << yytext << "'" << std::endl;
            return 0;
        }

%%

void process_number(const char* text) {
    current_num_value = atoi(text);
}

void process_variable(const char* text) {
    current_var_value = text;
}

int get_current_line() {
    return current_line;
}