%{
#include "token_t.hpp"
#include "lexer.hpp"
#include <iostream>
#include <cstdio>
#include <unordered_map>
#include <memory>

extern FILE* yyin;
extern int yylex();
extern int yyparse();
extern int get_current_line();

std::unordered_map<std::string, int> variables;

void yyerror(const char* s) {
    std::cerr << "Error at line " << get_current_line() << ": " << s << std::endl;
}

%}

%union {
    int num_value;
    std::string* str_value;
}

%token <num_value> NUM
%token <str_value> VAR
%token ADD SUB MUL DIV
%token ISAB ISABE ISLS ISLSE ISEQ
%token LCIB RCIB LCUB RCUB
%token WH IN AS PRINT
%token SC

%type <num_value> expression simple_expression term factor

%%

program:
    statements { std::cout << "Execution completed!" << std::endl; }
    ;

statements:
    /* empty */
    | statements statement
    ;

statement:
    assignment SC
    | print_statement SC
    | while_statement
    | input_statement SC
    ;

assignment:
    VAR AS expression {
        variables[*$1] = $3;
        std::cout << "Assigned " << $3 << " to variable " << *$1 << std::endl;
        delete $1;
    }
    ;

print_statement:
    PRINT expression {
        std::cout << "Output: " << $2 << std::endl;
    }
    ;

while_statement:
    WH LCIB expression RCIB LCUB statements RCUB {
        std::cout << "While loop executed" << std::endl;
    }
    ;

input_statement:
    IN AS VAR {
        int value;
        std::cout << "Enter value for " << *$3 << ": ";
        std::cin >> value;
        variables[*$3] = value;
        delete $3;
    }
    ;

expression:
    simple_expression { $$ = $1; }
    | expression ISAB simple_expression  { $$ = ($1 > $3) ? 1 : 0; }
    | expression ISABE simple_expression { $$ = ($1 >= $3) ? 1 : 0; }
    | expression ISLS simple_expression  { $$ = ($1 < $3) ? 1 : 0; }
    | expression ISLSE simple_expression { $$ = ($1 <= $3) ? 1 : 0; }
    | expression ISEQ simple_expression  { $$ = ($1 == $3) ? 1 : 0; }
    ;

simple_expression:
    term { $$ = $1; }
    | simple_expression ADD term { $$ = $1 + $3; }
    | simple_expression SUB term { $$ = $1 - $3; }
    ;

term:
    factor { $$ = $1; }
    | term MUL factor { $$ = $1 * $3; }
    | term DIV factor { 
          if ($3 == 0) {
              yyerror("Division by zero");
              $$ = 0;
          } else {
              $$ = $1 / $3;
          }
      }
    ;

factor:
    NUM { $$ = $1; }
    | VAR { 
          auto it = variables.find(*$1);
          if (it != variables.end()) {
              $$ = it->second;
          } else {
              yyerror(("Undefined variable: " + *$1).c_str());
              $$ = 0;
          }
          delete $1;
      }
    | LCIB expression RCIB { $$ = $2; }
    ;

%%
