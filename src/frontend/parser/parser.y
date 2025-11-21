%require "3.2"
%language "c++"
%locations
%define api.value.type variant
%define parse.error detailed

%define api.namespace {yy}
%define api.parser.class {parser}

%code requires {

#include <iostream>
#include <cstdio>
#include <memory>
#include <vector>
#include <string>
#include <string.h>
#include "parser/parser.hpp"
#include "global/global.hpp"

extern FILE* yyin;
extern std::string current_file;

extern int current_num_value;
extern std::string current_var_value;

}

%code {

#include "parser/parse_error.hpp"
#include "lexer/lexer.hpp"

ParaCL    ::ProgramAST        program;

int  yylex            (yy::parser::semantic_type* yylval, yy::parser::location_type* yylloc);
} /* %code */

%precedence OR
%precedence AND
%precedence ISEQ ISNE
%precedence ISAB ISABE ISLS ISLSE
%precedence ADD SUB
%precedence MUL DIV REM
%precedence NEG NOT
%precedence AS
%precedence ADDASGN
%precedence SUBASGN
%precedence MULASGN
%precedence DIVASGN

%token <int> NUM
%token <std::string> VAR
%token LCIB RCIB LCUB RCUB
%token WH IN PRINT IF ELIF ELSE
%token SC COMMA
%token <std::string> STRING

%type <std::vector<std::unique_ptr<ParaCL::Stmt>>> program statements
%type <std::unique_ptr<ParaCL::BlockStmt>> block one_stmt_block
%type <std::unique_ptr<ParaCL::Stmt>> statement assignment combined_assignment print_statement while_statement
%type <std::unique_ptr<ParaCL::Expr>> expression assignment_expression logical_or_expression logical_and_expression equality_expression relational_expression additive_expression multiplicative_expression unary_expression factor string_constant
%type <std::vector<std::unique_ptr<ParaCL::Expr>>> print_args

%type <std::unique_ptr<ParaCL::ConditionStatement>> condition_statement
%type <std::unique_ptr<ParaCL::IfStatement>> if_statement
%type <std::vector<std::unique_ptr<ParaCL::ElifStatement>>> elif_statements
%type <std::unique_ptr<ParaCL::ElseStatement>> else_statement

%start program

%%

program:
    statements {
        program.statements = std::move($1);
    }
    ;

statements:
    %empty {
        $$ = std::vector<std::unique_ptr<ParaCL::Stmt>>();
    }
    | statements statement {
        $1.push_back(std::move($2));
        $$ = std::move($1);
    }
    ;

statement:
    assignment SC { 
        $$ = std::move($1);
    }
    | combined_assignment SC {
        $$ = std::move($1);
    }
    | print_statement SC {
        $$ = std::move($1);
    }
    | while_statement {
        $$ = std::move($1);
    }
    | condition_statement {
        $$ = std::move($1);
    }
    | SC {
        $$ = std::make_unique<ParaCL::BlockStmt>();
    }
    ;

assignment:
    VAR AS expression {
        $$ = std::make_unique<ParaCL::AssignStmt>($1, std::move($3));
    }
    | VAR AS error {
        ErrorHandler::throwError(@3, "expected expression after assignment");
        YYABORT;
    }
    ;

combined_assignment:
    VAR ADDASGN expression {
        $$ = std::make_unique<ParaCL::CombinedAssingStmt>(
            ParaCL::token_t::ADDASGN,
            $1,
            std::move($3)
        );
    }
    | VAR SUBASGN expression {
        $$ = std::make_unique<ParaCL::CombinedAssingStmt>(
            ParaCL::token_t::SUBASGN,
            $1,
            std::move($3)
        );
    }
    | VAR MULASGN expression {
        $$ = std::make_unique<ParaCL::CombinedAssingStmt>(
            ParaCL::token_t::MULASGN,
            $1,
            std::move($3)
        );
    }
    | VAR DIVASGN expression {
        $$ = std::make_unique<ParaCL::CombinedAssingStmt>(
            ParaCL::token_t::DIVASGN,
            $1,
            std::move($3)
        );
    }
    | VAR ADDASGN error {
        ErrorHandler::throwError(@3, "expected expression after '+=' assignment");
        YYABORT;
    }
    | VAR SUBASGN error {
        ErrorHandler::throwError(@3, "expected expression after '-=' assignment");
        YYABORT;
    }
    | VAR MULASGN error {
        ErrorHandler::throwError(@3, "expected expression after '*=' assignment");
        YYABORT;
    }
    | VAR DIVASGN error {
        ErrorHandler::throwError(@3, "expected expression after '/=' assignment");
        YYABORT;
    }
    ;

print_statement:
    PRINT expression SC {
        std::vector<std::unique_ptr<ParaCL::Expr>> v;
        v.push_back(std::move($2));
        $$ = std::make_unique<ParaCL::PrintStmt>(std::move(v));

    }
    | PRINT error {
        ErrorHandler::throwError(@2, "expected expressions and string constants after print");
        YYABORT;
    }
    ;

print_args:
    expression {
        $$ = std::vector<std::unique_ptr<ParaCL::Expr>>();
        $$.push_back(std::move($1));
    }
    /* %empty {
        $$ = std::vector<std::unique_ptr<ParaCL::Expr>>();
    }
    | print_args COMMA expression {
        $1.push_back(std::move($3));
        $$ = std::move($1);
    } */
    /* | print_args COMMA string_constant {
        $1.push_back(std::move($3));
        $$ = std::move($1);
    } */
    ;

while_statement:
    WH LCIB expression RCIB LCUB block RCUB {
        $$ = std::make_unique<ParaCL::WhileStmt>(
            std::move($3),
            std::move($6)
        );
    }
    | WH LCIB expression RCIB one_stmt_block {
        $$ = std::make_unique<ParaCL::WhileStmt>(
            std::move($3), 
            std::move($5)
        );
    }
    | WH LCIB error RCIB LCUB block RCUB {
        ErrorHandler::throwError(@3, "expected condition expression in while statement");
        YYABORT;
    }
    | WH LCIB expression error LCUB block RCUB {
        ErrorHandler::throwError(@4, "expected ')' after while condition");
        YYABORT;
    }
    | WH LCIB expression RCIB error {
        ErrorHandler::throwError(@5, "expected block or statement after while condition");
        YYABORT;
    }
    | WH error {
        ErrorHandler::throwError(@2, "expected '(' after while");
        YYABORT;
    }
    ;

condition_statement:
    if_statement elif_statements else_statement {
        auto cond_stmt = std::make_unique<ParaCL::ConditionStatement>(
            std::move($1)
        );

        for (auto&& elif_stmt: $2)
            cond_stmt->add_elif_condition(std::move(elif_stmt));

        if ($3) cond_stmt->add_else_condition(std::move($3));
        
        $$ = std::move(cond_stmt);
    }
    ;

if_statement:
    IF LCIB expression RCIB LCUB block RCUB {
        $$ = std::make_unique<ParaCL::IfStatement>(
            std::move($3),
            std::move($6)
        );
    }
    | IF LCIB expression RCIB one_stmt_block {
        $$ = std::make_unique<ParaCL::IfStatement>(
            std::move($3),
            std::move($5)
        );
    }
    | IF LCIB error RCIB LCUB block RCUB {
        ErrorHandler::throwError(@3, "expected condition expression in if statement");
        YYABORT;
    }
    | IF LCIB expression error LCUB block RCUB {
        ErrorHandler::throwError(@4, "expected ')' after if condition");
        YYABORT;
    }
    | IF LCIB expression RCIB error {
        ErrorHandler::throwError(@5, "expected block or statement after if condition");
        YYABORT;
    }
    | IF error {
        ErrorHandler::throwError(@2, "expected '(' after if");
        YYABORT;
    }
    ;

elif_statements:
    %empty {
        $$ = std::vector<std::unique_ptr<ParaCL::ElifStatement>>();
    }
    | elif_statements ELIF LCIB expression RCIB LCUB block RCUB {
        $1.push_back(std::make_unique<ParaCL::ElifStatement>(
            std::move($4),
            std::move($7)
        ));
        $$ = std::move($1);
    }
    | elif_statements ELIF LCIB expression RCIB one_stmt_block {
        $1.push_back(std::make_unique<ParaCL::ElifStatement>(
            std::move($4),
            std::move($6)
        ));
        $$ = std::move($1);
    }
    | elif_statements ELIF LCIB error RCIB LCUB block RCUB {
        ErrorHandler::throwError(@4, "expected condition expression in elif statement");
        YYABORT;
    }
    | elif_statements ELIF LCIB expression error LCUB block RCUB {
        ErrorHandler::throwError(@5, "expected ')' after elif condition");
        YYABORT;
    }
    | elif_statements ELIF LCIB expression RCIB error {
        ErrorHandler::throwError(@6, "expected block or statement after elif condition");
        YYABORT;
    }
    | elif_statements ELIF error {
        ErrorHandler::throwError(@3, "expected '(' after elif");
        YYABORT;
    }
    ;

else_statement:
    %empty { $$ = nullptr; }
    | ELSE LCUB block RCUB {
        $$ = std::make_unique<ParaCL::ElseStatement>(
            std::move($3)
        );
    }
    | ELSE one_stmt_block {
        $$ = std::make_unique<ParaCL::ElseStatement>(
            std::move($2)
        );
    }
    | ELSE error {
        ErrorHandler::throwError(@2, "expected block or statement after else");
        YYABORT;
    }
    ;

expression:
    assignment_expression { $$ = std::move($1); }
    ;

assignment_expression:
    logical_or_expression { $$ = std::move($1); }
    | VAR AS assignment_expression %prec AS {
        $$ = std::make_unique<ParaCL::AssignExpr>($1, std::move($3));
    }
    | VAR ADDASGN assignment_expression %prec ADDASGN {
        $$ = std::make_unique<ParaCL::CombinedAssingExpr>(
            ParaCL::token_t::ADDASGN,
            $1,
            std::move($3)
        );
    }
    | VAR SUBASGN assignment_expression %prec SUBASGN {
        $$ = std::make_unique<ParaCL::CombinedAssingExpr>(
            ParaCL::token_t::SUBASGN,
            $1,
            std::move($3)
        );
    }
    | VAR MULASGN assignment_expression %prec MULASGN {
        $$ = std::make_unique<ParaCL::CombinedAssingExpr>(
            ParaCL::token_t::MULASGN,
            $1,
            std::move($3)
        );
    }
    | VAR DIVASGN assignment_expression %prec DIVASGN {
        $$ = std::make_unique<ParaCL::CombinedAssingExpr>(
            ParaCL::token_t::DIVASGN,
            $1,
            std::move($3)
        );
    }
    | VAR AS error {
        ErrorHandler::throwError(@3, "expected expression after assignment");
        YYABORT;
    }
    | VAR ADDASGN error {
        ErrorHandler::throwError(@3, "expected expression after '+=' assignment");
        YYABORT;
    }
    | VAR SUBASGN error {
        ErrorHandler::throwError(@3, "expected expression after '-=' assignment");
        YYABORT;
    }
    | VAR MULASGN error {
        ErrorHandler::throwError(@3, "expected expression after '*=' assignment");
        YYABORT;
    }
    | VAR DIVASGN error {
        ErrorHandler::throwError(@3, "expected expression after '/=' assignment");
        YYABORT;
    }
    ;

logical_or_expression:
    logical_and_expression { $$ = std::move($1); }
    | logical_or_expression OR logical_and_expression %prec OR {
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::OR,
            std::move($1),
            std::move($3)
        );
    }
    | logical_or_expression OR error {
        ErrorHandler::throwError(@3, "expected expression after 'or' operator");
        YYABORT;
    }
    ;

logical_and_expression:
    equality_expression { $$ = std::move($1); }
    | logical_and_expression AND equality_expression %prec AND {
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::AND,
            std::move($1),
            std::move($3)
        );
    }
    | logical_and_expression AND error {
        ErrorHandler::throwError(@3, "expected expression after 'and' operator");
        YYABORT;
    }
    ;

equality_expression:
    relational_expression { $$ = std::move($1); }
    | equality_expression ISEQ relational_expression %prec ISEQ {
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::ISEQ,
            std::move($1),
            std::move($3)
        );
    }
    | equality_expression ISNE relational_expression %prec ISNE {
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::ISNE,
            std::move($1),
            std::move($3)
        );
    }
    | equality_expression ISEQ error {
        ErrorHandler::throwError(@3, "expected expression after '==' operator");
        YYABORT;
    }
    | equality_expression ISNE error {
        ErrorHandler::throwError(@3, "expected expression after '!=' operator");
        YYABORT;
    }
    ;

relational_expression:
    additive_expression { $$ = std::move($1); }
    | relational_expression ISAB additive_expression %prec ISAB {
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::ISAB,
            std::move($1),
            std::move($3)
        );
    }
    | relational_expression ISABE additive_expression %prec ISABE {
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::ISABE,
            std::move($1),
            std::move($3)
        );
    }
    | relational_expression ISLS additive_expression %prec ISLS {
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::ISLS,
            std::move($1),
            std::move($3)
        );
    }
    | relational_expression ISLSE additive_expression %prec ISLSE {
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::ISLSE,
            std::move($1),
            std::move($3)
        );
    }
    | relational_expression ISAB error {
        ErrorHandler::throwError(@3, "expected expression after '>' operator");
        YYABORT;
    }
    | relational_expression ISABE error {
        ErrorHandler::throwError(@3, "expected expression after '>=' operator");
        YYABORT;
    }
    | relational_expression ISLS error {
        ErrorHandler::throwError(@3, "expected expression after '<' operator");
        YYABORT;
    }
    | relational_expression ISLSE error {
        ErrorHandler::throwError(@3, "expected expression after '<=' operator");
        YYABORT;
    }
    ;

additive_expression:
    multiplicative_expression { $$ = std::move($1); }
    | additive_expression ADD multiplicative_expression %prec ADD { 
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::ADD,
            std::move($1),
            std::move($3)
        );
    }
    | additive_expression SUB multiplicative_expression %prec SUB { 
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::SUB,
            std::move($1),
            std::move($3)
        );
    }
    | additive_expression ADD error {
        ErrorHandler::throwError(@3, "expected expression after '+' operator");
        YYABORT;
    }
    | additive_expression SUB error {
        ErrorHandler::throwError(@3, "expected expression after '-' operator");
        YYABORT;
    }
    ;

multiplicative_expression:
    unary_expression { $$ = std::move($1); }
    | multiplicative_expression MUL unary_expression %prec MUL { 
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::MUL, 
            std::move($1),
            std::move($3)
        );
    }
    | multiplicative_expression DIV unary_expression %prec DIV {
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::DIV, 
            std::move($1),
            std::move($3)
        );
    }
    | multiplicative_expression REM unary_expression %prec REM {
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::REM,
            std::move($1),
            std::move($3)
        );
    }
    | multiplicative_expression MUL error {
        ErrorHandler::throwError(@3, "expected expression after '*' operator");
        YYABORT;
    }
    | multiplicative_expression DIV error {
        ErrorHandler::throwError(@3, "expected expression after '/' operator");
        YYABORT;
    }
    | multiplicative_expression REM error {
        ErrorHandler::throwError(@3, "expected expression after '%' operator");
        YYABORT;
    }
    ;

unary_expression:
    factor { $$ = std::move($1); }
    | SUB unary_expression %prec NEG {
        $$ = std::make_unique<ParaCL::UnExpr>(
            ParaCL::token_t::SUB,
            std::move($2)
        );
    }
    | NOT unary_expression %prec NOT {
        $$ = std::make_unique<ParaCL::UnExpr>(
            ParaCL::token_t::NOT,
            std::move($2)
        );
    }
    | ADD unary_expression %prec NEG {
        $$ = std::move($2);
    }
    | SUB error {
        ErrorHandler::throwError(@2, "expected expression after unary '-'");
        YYABORT;
    }
    | NOT error {
        ErrorHandler::throwError(@2, "expected expression after 'not' operator");
        YYABORT;
    }
    | ADD error {
        ErrorHandler::throwError(@2, "expected expression after unary '+'");
        YYABORT;
    }
    ;

factor:
    NUM { 
        $$ = std::make_unique<ParaCL::NumExpr>($1);
    }
    | VAR { 
        $$ = std::make_unique<ParaCL::VarExpr>(std::move($1));
    }
    | LCIB expression RCIB { 
        $$ = std::move($2);
    }
    | IN {
        $$ = std::make_unique<ParaCL::InputExpr>();
    }
    | LCIB error RCIB {
        ErrorHandler::throwError(@2, "expected expression inside parentheses");
        YYABORT;
    }
    | LCIB expression error {
        ErrorHandler::throwError(@3, "expected ')' after expression");
        YYABORT;
    }
    ;

block:
    statements {
        $$ = std::make_unique<ParaCL::BlockStmt>(std::move($1));
    }
    ;

one_stmt_block:
    statement {
        std::vector<std::unique_ptr<ParaCL::Stmt>> body_stmts;
        body_stmts.push_back(std::move($1));
        $$ = std::make_unique<ParaCL::BlockStmt>(std::move(body_stmts));
    }
    ;

string_constant:
    STRING {
        $$ = std::make_unique<ParaCL::StringConstant>(std::move($1));
    }
    ;

%%
