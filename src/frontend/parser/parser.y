%{

#include <iostream>
#include <cstdio>
#include <unordered_map>
#include <memory>
#include <vector>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "global/global.hpp"

extern FILE* yyin;
extern int yyparse();

inline ParaCL::ProgramAST program;

extern std::string current_file;

typedef struct YYLTYPE YYLTYPE;
typedef union YYSTYPE YYSTYPE;

void yyerror(YYLTYPE* loc, const char* msg);
int yylex(YYSTYPE* yylval_param, YYLTYPE* yylloc_param);

%}

%define api.pure full
%locations



%union {
    int                                      num_value     ;
    std   ::string                         * str_value     ;
    ParaCL::Stmt                           * stmt          ;
    ParaCL::Expr                           * expr          ;
    ParaCL::BlockStmt                      * block         ;
    ParaCL::ConditionStatement             * condition_stmt;
    ParaCL::IfStatement                    * if_stmt       ;
    std   ::vector<std::unique_ptr<ParaCL::ElifStatement>>* elif_stmts    ;
    ParaCL::ElifStatement                  * elif_stmt     ;
    ParaCL::ElseStatement                  * else_stmt     ;
    std   ::vector<ParaCL::Stmt*>          * stmt_vector   ;
}

%token <num_value> NUM
%token <str_value> VAR
%token AND OR
%token ADDASGN SUBASGN MULASGN DIVASGN
%token ADD SUB MUL DIV REM
%token ISAB ISABE ISLS ISLSE ISEQ ISNE
%token LCIB RCIB LCUB RCUB
%token WH IN AS PRINT IF ELIF ELSE
%token SC

%type <stmt_vector> program statements
%type <block> block one_stmt_block
%type <stmt> statement assignment combined_assignment print_statement while_statement input_statement
%type <expr> expression and_or_expression math_comparison_expression add_sub_expression mul_div_expression remains_expression factor assignment_expression

%type <condition_stmt> condition_statement
%type <if_stmt>               if_statement
%type <elif_stmts>          elif_statements
%type <elif_stmt>           elif_statement
%type <else_stmt>           else_statement

%%

program:
    statements {
        for (auto stmt : *$1)
        {
            program.statements.push_back(std::unique_ptr<ParaCL::Stmt>(stmt));
        }
        delete $1;
    }
    ;

statements:
    {
        $$ = new std::vector<ParaCL::Stmt*>();
    }
    | statements statement {
        $1->push_back($2);
        $$ = $1;
    }
    ;

statement:
    assignment SC { 
        $$ = $1;
    }
    | combined_assignment SC {
        $$ = $1;
    }
    | print_statement SC {
        $$ = $1;
    }
    | while_statement {
        $$ = $1;
    }
    | input_statement SC {
        $$ = $1;
    }
    | condition_statement {
        $$ = $1;
    }
    ;

assignment:
    VAR AS expression {
        $$ = new ParaCL::AssignStmt(*$1, std::unique_ptr<ParaCL::Expr>($3));
        delete $1;
    }
    ;

combined_assignment:
    VAR ADDASGN expression {
        $$ = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::ADDASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        delete $1;
    }
    | VAR SUBASGN expression {
        $$ = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::SUBASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        delete $1;
    }
    | VAR MULASGN expression {
        $$ = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::MULASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        delete $1;
    }
    | VAR DIVASGN expression {
        $$ = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::DIVASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        delete $1;
    }
    ;

print_statement:
    PRINT expression {
        $$ = new ParaCL::PrintStmt(std::unique_ptr<ParaCL::Expr>($2));
    }
    ;

while_statement:
    WH LCIB expression RCIB LCUB block RCUB {
        $$ = new ParaCL::WhileStmt(
            std::unique_ptr<ParaCL::Expr>($3),
            std::unique_ptr<ParaCL::BlockStmt>($6)
        );
    }
    | WH LCIB expression RCIB statement {
        auto body_stmts = std::vector<std::unique_ptr<ParaCL::Stmt>>();
        body_stmts.push_back(std::unique_ptr<ParaCL::Stmt>($5));
        auto body = new ParaCL::BlockStmt(std::move(body_stmts));

        $$ = new ParaCL::WhileStmt(
            std::unique_ptr<ParaCL::Expr>($3), 
            std::unique_ptr<ParaCL::BlockStmt>(body)
        );
    }
    ;

input_statement:
    IN AS VAR {
    }
    ;

condition_statement:
    if_statement elif_statements else_statement {
        $$ = new ParaCL::ConditionStatement(
            std::unique_ptr<ParaCL::IfStatement>($1)
    );
    
        ($$)->add_elif_conditions($2);
        delete $2;
    
        ($$)->add_else_condition(std::unique_ptr<ParaCL::ElseStatement>($3));
    }
    ;

if_statement:
    IF LCIB expression RCIB LCUB block RCUB {
        $$ = new ParaCL::IfStatement(
            std::unique_ptr<ParaCL::Expr>($3),
            std::unique_ptr<ParaCL::BlockStmt>($6)
        );
    }
    | IF LCIB expression RCIB one_stmt_block {
        $$ = new ParaCL::IfStatement(
            std::unique_ptr<ParaCL::Expr>($3),
            std::unique_ptr<ParaCL::BlockStmt>($5)
        );
    }
    ;

elif_statements:
    {
        $$ = new std::vector<std::unique_ptr<ParaCL::ElifStatement>>();
    }
    | elif_statements elif_statement {
        if ($2) $1->push_back(std::unique_ptr<ParaCL::ElifStatement>($2));
        $$ = $1;
    }
    ;

elif_statement:
    ELIF LCIB expression RCIB LCUB block RCUB {
        $$ = new ParaCL::ElifStatement(
            std::unique_ptr<ParaCL::Expr>($3),
            std::unique_ptr<ParaCL::BlockStmt>($6)
        );
    }
    | ELIF LCIB expression RCIB one_stmt_block {
        $$ = new ParaCL::ElifStatement(
            std::unique_ptr<ParaCL::Expr>($3),
            std::unique_ptr<ParaCL::BlockStmt>($5)
        );
    }
    ;

else_statement:
    { $$ = nullptr; }
    | ELSE LCUB block RCUB {
        $$ = new ParaCL::ElseStatement(
            std::unique_ptr<ParaCL::BlockStmt>($3)
        );
    }
    | ELSE one_stmt_block {
        $$ = new ParaCL::ElseStatement(
            std::unique_ptr<ParaCL::BlockStmt>($2)
        );
    }
    ;

expression:
    remains_expression { $$ = $1; }
    ;

remains_expression:
    and_or_expression { $$ = $1; }
    | remains_expression REM remains_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::REM,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

and_or_expression:
    math_comparison_expression { $$ = $1; }
    | and_or_expression AND and_or_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::AND,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | and_or_expression OR and_or_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::OR,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

math_comparison_expression:
    add_sub_expression { $$ = $1; }
    | math_comparison_expression ISAB math_comparison_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISAB,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | math_comparison_expression ISABE math_comparison_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISABE,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | math_comparison_expression ISLS math_comparison_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISLS,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | math_comparison_expression ISLSE math_comparison_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISLSE,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | math_comparison_expression ISEQ math_comparison_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISEQ,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | math_comparison_expression ISNE math_comparison_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISNE,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

add_sub_expression:
    mul_div_expression { $$ = $1; }
    | add_sub_expression ADD add_sub_expression { 
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ADD,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | add_sub_expression SUB add_sub_expression { 
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::SUB,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

mul_div_expression:
    factor { $$ = $1; }
    | mul_div_expression MUL mul_div_expression { 
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::MUL, 
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | mul_div_expression DIV factor { /* factor because problem with /: (10 / 2 / 5) is a (10 / 2) / 5, not a 10 / (2 / 5) */
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::DIV, 
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;


factor:
    NUM { 
        $$ = new ParaCL::NumExpr($1);
    }
    | VAR { 
        $$ = new ParaCL::VarExpr(*$1);
        delete $1;
    }
    | LCIB expression RCIB { 
        $$ = $2;
    }
    | assignment_expression { 
        $$ = $1;
    }
    | IN {
        $$ = new ParaCL::InputExpr();
    }
    ;

assignment_expression:
    VAR AS expression {
        $$ = new ParaCL::AssignExpr(*$1, std::unique_ptr<ParaCL::Expr>($3));
        delete $1;
    }
    ;

block:
    statements {
        std::vector<std::unique_ptr<ParaCL::Stmt>> body_stmts;
        for (auto stmt : *$1)
            body_stmts.push_back(std::unique_ptr<ParaCL::Stmt>(stmt));

        $$ = new ParaCL::BlockStmt(std::move(body_stmts));
        delete $1; /* delete vector of statements, created in 'statements' rule */
    }
    ;

one_stmt_block:
    statement {
        std::vector<std::unique_ptr<ParaCL::Stmt>> body_stmts;
        body_stmts.push_back(std::unique_ptr<ParaCL::Stmt>($1));
        $$ = new ParaCL::BlockStmt(std::move(body_stmts));
    }
    ;
%%

/* int yylex(YYSTYPE* yylval_param, YYLTYPE* yylloc_param); */


void yyerror(YYLTYPE* loc, const char* msg)
{
    std::cerr << current_file     << ":"
              << loc->first_line   << ":"
              << loc->first_column << ":"
              " paracl: error: " << msg << "\n";
}
