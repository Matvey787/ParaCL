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

#include "log/log_api.hpp"

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

%type <std::vector<std::unique_ptr<ParaCL::Statement>>> program statements
%type <std::unique_ptr<ParaCL::BlockStmt>> block one_stmt_block
%type <std::unique_ptr<ParaCL::Statement>> statement assignment combined_assignment print_statement while_statement
%type <std::unique_ptr<ParaCL::Expression>> expression assignment_expression logical_or_expression logical_and_expression equality_expression relational_expression additive_expression multiplicative_expression unary_expression factor string_constant
%type <std::vector<std::unique_ptr<ParaCL::Expression>>> print_args

%type <std::unique_ptr<ParaCL::ConditionStatement>> condition_statement
%type <std::unique_ptr<ParaCL::IfStatement>> if_statement
%type <std::vector<std::unique_ptr<ParaCL::ElifStatement>>> elif_statements
%type <std::unique_ptr<ParaCL::ElseStatement>> else_statement

%start program

%%

program:
    statements {
        LOGINFO("paracl: parser: rule: program -> statements");
        program.statements = std::move($1);
    }
    ;

statements:
    %empty {
        LOGINFO("paracl: parser: rule: statements -> empty");
        $$ = std::vector<std::unique_ptr<ParaCL::Statement>>();
    }
    | statements statement {
        LOGINFO("paracl: parser: rule: statements -> statements statement");
        $1.push_back(std::move($2));
        $$ = std::move($1);
    }
    | statements LCUB block RCUB {
        LOGINFO("paracl: parser: rule: statements -> statements LCUB block RCUB");
        $1.push_back(std::move($3));
        $$ = std::move($1);
    }
    ;

statement:
    assignment SC {
        LOGINFO("paracl: parser: rule: statement -> assignment SC");
        $$ = std::move($1);
    }
    | combined_assignment SC {
        LOGINFO("paracl: parser: rule: statement -> combined_assignment SC");
        $$ = std::move($1);
    }
    | print_statement SC {
        LOGINFO("paracl: parser: rule: statement -> print_statement SC");
        $$ = std::move($1);
    }
    | while_statement {
        LOGINFO("paracl: parser: rule: statement -> while_statement");
        $$ = std::move($1);
    }
    | condition_statement {
        LOGINFO("paracl: parser: rule: statement -> condition_statement");
        $$ = std::move($1);
    }
    | SC {
        LOGINFO("paracl: parser: rule: statement -> SC");
        $$ = std::make_unique<ParaCL::BlockStmt>();
    }
    ;

assignment:
    VAR AS expression {
        LOGINFO("paracl: parser: rule: assignment -> VAR AS expression");
        $$ = std::make_unique<ParaCL::AssignStmt>($1, std::move($3));
    }
    | VAR AS error {
        LOGERR("paracl: parser: rule: assignment -> VAR AS error");
        ErrorHandler::throwError(@3, "expected expression after assignment");
        YYABORT;
    }
    ;

combined_assignment:
    VAR ADDASGN expression {
        LOGINFO("paracl: parser: rule: combined_assignment -> VAR ADDASGN expression");
        $$ = std::make_unique<ParaCL::CombinedAssingStmt>(
            ParaCL::token_t::ADDASGN,
            $1,
            std::move($3)
        );
    }
    | VAR SUBASGN expression {
        LOGINFO("paracl: parser: rule: combined_assignment -> VAR SUBASGN expression");
        $$ = std::make_unique<ParaCL::CombinedAssingStmt>(
            ParaCL::token_t::SUBASGN,
            $1,
            std::move($3)
        );
    }
    | VAR MULASGN expression {
        LOGINFO("paracl: parser: rule: combined_assignment -> VAR MULASGN expression");
        $$ = std::make_unique<ParaCL::CombinedAssingStmt>(
            ParaCL::token_t::MULASGN,
            $1,
            std::move($3)
        );
    }
    | VAR DIVASGN expression {
        LOGINFO("paracl: parser: rule: combined_assignment -> VAR DIVASGN expression");
        $$ = std::make_unique<ParaCL::CombinedAssingStmt>(
            ParaCL::token_t::DIVASGN,
            $1,
            std::move($3)
        );
    }
    | VAR ADDASGN error {
        LOGERR("paracl: parser: rule: combined_assignment -> VAR ADDASGN error");
        ErrorHandler::throwError(@3, "expected expression after '+=' assignment");
        YYABORT;
    }
    | VAR SUBASGN error {
        LOGERR("paracl: parser: rule: combined_assignment -> VAR SUBASGN error");
        ErrorHandler::throwError(@3, "expected expression after '-=' assignment");
        YYABORT;
    }
    | VAR MULASGN error {
        LOGERR("paracl: parser: rule: combined_assignment -> VAR MULASGN error");
        ErrorHandler::throwError(@3, "expected expression after '*=' assignment");
        YYABORT;
    }
    | VAR DIVASGN error {
        LOGERR("paracl: parser: rule: combined_assignment -> VAR DIVASGN error");
        ErrorHandler::throwError(@3, "expected expression after '/=' assignment");
        YYABORT;
    }
    ;

print_statement:
    PRINT print_args {
        LOGINFO("paracl: parser: rule: print_statement -> PRINT print_args");
        $$ = std::make_unique<ParaCL::PrintStmt>(std::move($2));
    }
    | PRINT error {
        LOGERR("paracl: parser: rule: print_statement -> PRINT error");
        ErrorHandler::throwError(@2, "expected expressions and string constants after print");
        YYABORT;
    }
    ;

print_args:
    %empty {
        LOGINFO("paracl: parser: rule: print_args -> empty");
        $$ = std::vector<std::unique_ptr<ParaCL::Expression>>();
    }
    | print_args expression {
        LOGINFO("paracl: parser: rule: print_args -> print_args expression");
        $1.push_back(std::move($2));
        $$ = std::move($1);
    }
    | print_args string_constant {
        LOGINFO("paracl: parser: rule: print_args -> print_args string_constant");
        $1.push_back(std::move($2));
        $$ = std::move($1);
    }
    | print_args COMMA expression {
        LOGINFO("paracl: parser: rule: print_args -> print_args COMMA expression");
        $1.push_back(std::move($3));
        $$ = std::move($1);
    }
    | print_args COMMA string_constant {
        LOGINFO("paracl: parser: rule: print_args -> print_args COMMA string_constant");
        $1.push_back(std::move($3));
        $$ = std::move($1);
    }
    ;

while_statement:
    WH LCIB expression RCIB LCUB block RCUB {
        LOGINFO("paracl: parser: rule: while_statement -> WH LCIB expression RCIB LCUB block RCUB");
        $$ = std::make_unique<ParaCL::WhileStmt>(
            std::move($3),
            std::move($6)
        );
    }
    | WH LCIB expression RCIB one_stmt_block {
        LOGINFO("paracl: parser: rule: while_statement -> WH LCIB expression RCIB one_stmt_block");
        $$ = std::make_unique<ParaCL::WhileStmt>(
            std::move($3), 
            std::move($5)
        );
    }
    | WH LCIB error RCIB LCUB block RCUB {
        LOGERR("paracl: parser: rule: while_statement -> WH LCIB error RCIB LCUB block RCUB");
        ErrorHandler::throwError(@3, "expected condition expression in while statement");
        YYABORT;
    }
    | WH LCIB expression error LCUB block RCUB {
        LOGERR("paracl: parser: rule: while_statement -> WH LCIB expression error LCUB block RCUB");
        ErrorHandler::throwError(@4, "expected ')' after while condition");
        YYABORT;
    }
    | WH LCIB expression RCIB error {
        LOGERR("paracl: parser: rule: while_statement -> WH LCIB expression RCIB error");
        ErrorHandler::throwError(@5, "expected block or statement after while condition");
        YYABORT;
    }
    | WH error {
        LOGERR("paracl: parser: rule: while_statement -> WH error");
        ErrorHandler::throwError(@2, "expected '(' after while");
        YYABORT;
    }
    ;

condition_statement:
    if_statement elif_statements else_statement {
        LOGINFO("paracl: parser: rule: condition_statement -> if_statement elif_statements else_statement");
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
        LOGINFO("paracl: parser: rule: if_statement -> IF LCIB expression RCIB LCUB block RCUB");
        $$ = std::make_unique<ParaCL::IfStatement>(
            std::move($3),
            std::move($6)
        );
    }
    | IF LCIB expression RCIB one_stmt_block {
        LOGINFO("paracl: parser: rule: if_statement -> IF LCIB expression RCIB one_stmt_block");
        $$ = std::make_unique<ParaCL::IfStatement>(
            std::move($3),
            std::move($5)
        );
    }
    | IF LCIB error RCIB LCUB block RCUB {
        LOGERR("paracl: parser: rule: if_statement -> IF LCIB error RCIB LCUB block RCUB");
        ErrorHandler::throwError(@3, "expected condition expression in if statement");
        YYABORT;
    }
    | IF LCIB expression error LCUB block RCUB {
        LOGERR("paracl: parser: rule: if_statement -> IF LCIB expression error LCUB block RCUB");
        ErrorHandler::throwError(@4, "expected ')' after if condition");
        YYABORT;
    }
    | IF LCIB expression RCIB error {
        LOGERR("paracl: parser: rule: if_statement -> IF LCIB expression RCIB error");
        ErrorHandler::throwError(@5, "expected block or statement after if condition");
        YYABORT;
    }
    | IF error {
        LOGERR("paracl: parser: rule: if_statement -> IF error");
        ErrorHandler::throwError(@2, "expected '(' after if");
        YYABORT;
    }
    ;

elif_statements:
    %empty {
        LOGINFO("paracl: parser: rule: elif_statements -> empty");
        $$ = std::vector<std::unique_ptr<ParaCL::ElifStatement>>();
    }
    | elif_statements ELIF LCIB expression RCIB LCUB block RCUB {
        LOGINFO("paracl: parser: rule: elif_statements -> elif_statements ELIF LCIB expression RCIB LCUB block RCUB");
        $1.push_back(std::make_unique<ParaCL::ElifStatement>(
            std::move($4),
            std::move($7)
        ));
        $$ = std::move($1);
    }
    | elif_statements ELIF LCIB expression RCIB one_stmt_block {
        LOGINFO("paracl: parser: rule: elif_statements -> elif_statements ELIF LCIB expression RCIB one_stmt_block");
        $1.push_back(std::make_unique<ParaCL::ElifStatement>(
            std::move($4),
            std::move($6)
        ));
        $$ = std::move($1);
    }
    | elif_statements ELIF LCIB error RCIB LCUB block RCUB {
        LOGERR("paracl: parser: rule: elif_statements -> elif_statements ELIF LCIB error RCIB LCUB block RCUB");
        ErrorHandler::throwError(@4, "expected condition expression in elif statement");
        YYABORT;
    }
    | elif_statements ELIF LCIB expression error LCUB block RCUB {
        LOGERR("paracl: parser: rule: elif_statements -> elif_statements ELIF LCIB expression error LCUB block RCUB");
        ErrorHandler::throwError(@5, "expected ')' after elif condition");
        YYABORT;
    }
    | elif_statements ELIF LCIB expression RCIB error {
        LOGERR("paracl: parser: rule: elif_statements -> elif_statements ELIF LCIB expression RCIB error");
        ErrorHandler::throwError(@6, "expected block or statement after elif condition");
        YYABORT;
    }
    | elif_statements ELIF error {
        LOGERR("paracl: parser: rule: elif_statements -> elif_statements ELIF error");
        ErrorHandler::throwError(@3, "expected '(' after elif");
        YYABORT;
    }
    ;

else_statement:
    %empty { 
        LOGINFO("paracl: parser: rule: else_statement -> empty");
        $$ = nullptr; 
    }
    | ELSE LCUB block RCUB {
        LOGINFO("paracl: parser: rule: else_statement -> ELSE LCUB block RCUB");
        $$ = std::make_unique<ParaCL::ElseStatement>(
            std::move($3)
        );
    }
    | ELSE one_stmt_block {
        LOGINFO("paracl: parser: rule: else_statement -> ELSE one_stmt_block");
        $$ = std::make_unique<ParaCL::ElseStatement>(
            std::move($2)
        );
    }
    | ELSE error {
        LOGERR("paracl: parser: rule: else_statement -> ELSE error");
        ErrorHandler::throwError(@2, "expected block or statement after else");
        YYABORT;
    }
    ;

expression:
    assignment_expression { 
        LOGINFO("paracl: parser: rule: expression -> assignment_expression");
        $$ = std::move($1); 
    }
    ;

assignment_expression:
    logical_or_expression { 
        LOGINFO("paracl: parser: rule: assignment_expression -> logical_or_expression");
        $$ = std::move($1); 
    }
    | VAR AS assignment_expression %prec AS {
        LOGINFO("paracl: parser: rule: assignment_expression -> VAR AS assignment_expression");
        $$ = std::make_unique<ParaCL::AssignExpr>($1, std::move($3));
    }
    | VAR ADDASGN assignment_expression %prec ADDASGN {
        LOGINFO("paracl: parser: rule: assignment_expression -> VAR ADDASGN assignment_expression");
        $$ = std::make_unique<ParaCL::CombinedAssingExpr>(
            ParaCL::token_t::ADDASGN,
            $1,
            std::move($3)
        );
    }
    | VAR SUBASGN assignment_expression %prec SUBASGN {
        LOGINFO("paracl: parser: rule: assignment_expression -> VAR SUBASGN assignment_expression");
        $$ = std::make_unique<ParaCL::CombinedAssingExpr>(
            ParaCL::token_t::SUBASGN,
            $1,
            std::move($3)
        );
    }
    | VAR MULASGN assignment_expression %prec MULASGN {
        LOGINFO("paracl: parser: rule: assignment_expression -> VAR MULASGN assignment_expression");
        $$ = std::make_unique<ParaCL::CombinedAssingExpr>(
            ParaCL::token_t::MULASGN,
            $1,
            std::move($3)
        );
    }
    | VAR DIVASGN assignment_expression %prec DIVASGN {
        LOGINFO("paracl: parser: rule: assignment_expression -> VAR DIVASGN assignment_expression");
        $$ = std::make_unique<ParaCL::CombinedAssingExpr>(
            ParaCL::token_t::DIVASGN,
            $1,
            std::move($3)
        );
    }
    | VAR AS error {
        LOGERR("paracl: parser: rule: assignment_expression -> VAR AS error");
        ErrorHandler::throwError(@3, "expected expression after assignment");
        YYABORT;
    }
    | VAR ADDASGN error {
        LOGERR("paracl: parser: rule: assignment_expression -> VAR ADDASGN error");
        ErrorHandler::throwError(@3, "expected expression after '+=' assignment");
        YYABORT;
    }
    | VAR SUBASGN error {
        LOGERR("paracl: parser: rule: assignment_expression -> VAR SUBASGN error");
        ErrorHandler::throwError(@3, "expected expression after '-=' assignment");
        YYABORT;
    }
    | VAR MULASGN error {
        LOGERR("paracl: parser: rule: assignment_expression -> VAR MULASGN error");
        ErrorHandler::throwError(@3, "expected expression after '*=' assignment");
        YYABORT;
    }
    | VAR DIVASGN error {
        LOGERR("paracl: parser: rule: assignment_expression -> VAR DIVASGN error");
        ErrorHandler::throwError(@3, "expected expression after '/=' assignment");
        YYABORT;
    }
    ;

logical_or_expression:
    logical_and_expression { 
        LOGINFO("paracl: parser: rule: logical_or_expression -> logical_and_expression");
        $$ = std::move($1); 
    }
    | logical_or_expression OR logical_and_expression %prec OR {
        LOGINFO("paracl: parser: rule: logical_or_expression -> logical_or_expression OR logical_and_expression");
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::OR,
            std::move($1),
            std::move($3)
        );
    }
    | logical_or_expression OR error {
        LOGERR("paracl: parser: rule: logical_or_expression -> logical_or_expression OR error");
        ErrorHandler::throwError(@3, "expected expression after 'or' operator");
        YYABORT;
    }
    ;

logical_and_expression:
    equality_expression { 
        LOGINFO("paracl: parser: rule: logical_and_expression -> equality_expression");
        $$ = std::move($1); 
    }
    | logical_and_expression AND equality_expression %prec AND {
        LOGINFO("paracl: parser: rule: logical_and_expression -> logical_and_expression AND equality_expression");
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::AND,
            std::move($1),
            std::move($3)
        );
    }
    | logical_and_expression AND error {
        LOGERR("paracl: parser: rule: logical_and_expression -> logical_and_expression AND error");
        ErrorHandler::throwError(@3, "expected expression after 'and' operator");
        YYABORT;
    }
    ;

equality_expression:
    relational_expression { 
        LOGINFO("paracl: parser: rule: equality_expression -> relational_expression");
        $$ = std::move($1); 
    }
    | equality_expression ISEQ relational_expression %prec ISEQ {
        LOGINFO("paracl: parser: rule: equality_expression -> equality_expression ISEQ relational_expression");
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::ISEQ,
            std::move($1),
            std::move($3)
        );
    }
    | equality_expression ISNE relational_expression %prec ISNE {
        LOGINFO("paracl: parser: rule: equality_expression -> equality_expression ISNE relational_expression");
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::ISNE,
            std::move($1),
            std::move($3)
        );
    }
    | equality_expression ISEQ error {
        LOGERR("paracl: parser: rule: equality_expression -> equality_expression ISEQ error");
        ErrorHandler::throwError(@3, "expected expression after '==' operator");
        YYABORT;
    }
    | equality_expression ISNE error {
        LOGERR("paracl: parser: rule: equality_expression -> equality_expression ISNE error");
        ErrorHandler::throwError(@3, "expected expression after '!=' operator");
        YYABORT;
    }
    ;

relational_expression:
    additive_expression { 
        LOGINFO("paracl: parser: rule: relational_expression -> additive_expression");
        $$ = std::move($1); 
    }
    | relational_expression ISAB additive_expression %prec ISAB {
        LOGINFO("paracl: parser: rule: relational_expression -> relational_expression ISAB additive_expression");
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::ISAB,
            std::move($1),
            std::move($3)
        );
    }
    | relational_expression ISABE additive_expression %prec ISABE {
        LOGINFO("paracl: parser: rule: relational_expression -> relational_expression ISABE additive_expression");
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::ISABE,
            std::move($1),
            std::move($3)
        );
    }
    | relational_expression ISLS additive_expression %prec ISLS {
        LOGINFO("paracl: parser: rule: relational_expression -> relational_expression ISLS additive_expression");
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::ISLS,
            std::move($1),
            std::move($3)
        );
    }
    | relational_expression ISLSE additive_expression %prec ISLSE {
        LOGINFO("paracl: parser: rule: relational_expression -> relational_expression ISLSE additive_expression");
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::ISLSE,
            std::move($1),
            std::move($3)
        );
    }
    | relational_expression ISAB error {
        LOGERR("paracl: parser: rule: relational_expression -> relational_expression ISAB error");
        ErrorHandler::throwError(@3, "expected expression after '>' operator");
        YYABORT;
    }
    | relational_expression ISABE error {
        LOGERR("paracl: parser: rule: relational_expression -> relational_expression ISABE error");
        ErrorHandler::throwError(@3, "expected expression after '>=' operator");
        YYABORT;
    }
    | relational_expression ISLS error {
        LOGERR("paracl: parser: rule: relational_expression -> relational_expression ISLS error");
        ErrorHandler::throwError(@3, "expected expression after '<' operator");
        YYABORT;
    }
    | relational_expression ISLSE error {
        LOGERR("paracl: parser: rule: relational_expression -> relational_expression ISLSE error");
        ErrorHandler::throwError(@3, "expected expression after '<=' operator");
        YYABORT;
    }
    ;

additive_expression:
    multiplicative_expression { 
        LOGINFO("paracl: parser: rule: additive_expression -> multiplicative_expression");
        $$ = std::move($1); 
    }
    | additive_expression ADD multiplicative_expression %prec ADD { 
        LOGINFO("paracl: parser: rule: additive_expression -> additive_expression ADD multiplicative_expression");
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::ADD,
            std::move($1),
            std::move($3)
        );
    }
    | additive_expression SUB multiplicative_expression %prec SUB { 
        LOGINFO("paracl: parser: rule: additive_expression -> additive_expression SUB multiplicative_expression");
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::SUB,
            std::move($1),
            std::move($3)
        );
    }
    | additive_expression ADD error {
        LOGERR("paracl: parser: rule: additive_expression -> additive_expression ADD error");
        ErrorHandler::throwError(@3, "expected expression after '+' operator");
        YYABORT;
    }
    | additive_expression SUB error {
        LOGERR("paracl: parser: rule: additive_expression -> additive_expression SUB error");
        ErrorHandler::throwError(@3, "expected expression after '-' operator");
        YYABORT;
    }
    ;

multiplicative_expression:
    unary_expression { 
        LOGINFO("paracl: parser: rule: multiplicative_expression -> unary_expression");
        $$ = std::move($1); 
    }
    | multiplicative_expression MUL unary_expression %prec MUL { 
        LOGINFO("paracl: parser: rule: multiplicative_expression -> multiplicative_expression MUL unary_expression");
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::MUL, 
            std::move($1),
            std::move($3)
        );
    }
    | multiplicative_expression DIV unary_expression %prec DIV {
        LOGINFO("paracl: parser: rule: multiplicative_expression -> multiplicative_expression DIV unary_expression");
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::DIV, 
            std::move($1),
            std::move($3)
        );
    }
    | multiplicative_expression REM unary_expression %prec REM {
        LOGINFO("paracl: parser: rule: multiplicative_expression -> multiplicative_expression REM unary_expression");
        $$ = std::make_unique<ParaCL::BinExpr>(
            ParaCL::token_t::REM,
            std::move($1),
            std::move($3)
        );
    }
    | multiplicative_expression MUL error {
        LOGERR("paracl: parser: rule: multiplicative_expression -> multiplicative_expression MUL error");
        ErrorHandler::throwError(@3, "expected expression after '*' operator");
        YYABORT;
    }
    | multiplicative_expression DIV error {
        LOGERR("paracl: parser: rule: multiplicative_expression -> multiplicative_expression DIV error");
        ErrorHandler::throwError(@3, "expected expression after '/' operator");
        YYABORT;
    }
    | multiplicative_expression REM error {
        LOGERR("paracl: parser: rule: multiplicative_expression -> multiplicative_expression REM error");
        ErrorHandler::throwError(@3, "expected expression after '%' operator");
        YYABORT;
    }
    ;

unary_expression:
    factor { 
        LOGINFO("paracl: parser: rule: unary_expression -> factor");
        $$ = std::move($1); 
    }
    | SUB unary_expression %prec NEG {
        LOGINFO("paracl: parser: rule: unary_expression -> SUB unary_expression");
        $$ = std::make_unique<ParaCL::UnExpr>(
            ParaCL::token_t::SUB,
            std::move($2)
        );
    }
    | NOT unary_expression %prec NOT {
        LOGINFO("paracl: parser: rule: unary_expression -> NOT unary_expression");
        $$ = std::make_unique<ParaCL::UnExpr>(
            ParaCL::token_t::NOT,
            std::move($2)
        );
    }
    | ADD unary_expression %prec NEG {
        LOGINFO("paracl: parser: rule: unary_expression -> ADD unary_expression");
        $$ = std::move($2);
    }
    | SUB error {
        LOGERR("paracl: parser: rule: unary_expression -> SUB error");
        ErrorHandler::throwError(@2, "expected expression after unary '-'");
        YYABORT;
    }
    | NOT error {
        LOGERR("paracl: parser: rule: unary_expression -> NOT error");
        ErrorHandler::throwError(@2, "expected expression after 'not' operator");
        YYABORT;
    }
    | ADD error {
        LOGERR("paracl: parser: rule: unary_expression -> ADD error");
        ErrorHandler::throwError(@2, "expected expression after unary '+'");
        YYABORT;
    }
    ;

factor:
    NUM { 
        LOGINFO("paracl: parser: rule: factor -> NUM");
        $$ = std::make_unique<ParaCL::NumExpr>($1);
    }
    | VAR { 
        LOGINFO("paracl: parser: rule: factor -> VAR");
        $$ = std::make_unique<ParaCL::VarExpr>(std::move($1));
    }
    | LCIB expression RCIB { 
        LOGINFO("paracl: parser: rule: factor -> LCIB expression RCIB");
        $$ = std::move($2);
    }
    | IN {
        LOGINFO("paracl: parser: rule: factor -> IN");
        $$ = std::make_unique<ParaCL::InputExpr>();
    }
    | LCIB error RCIB {
        LOGERR("paracl: parser: rule: factor -> LCIB error RCIB");
        ErrorHandler::throwError(@2, "expected expression inside parentheses");
        YYABORT;
    }
    | LCIB expression error {
        LOGERR("paracl: parser: rule: factor -> LCIB expression error");
        ErrorHandler::throwError(@3, "expected ')' after expression");
        YYABORT;
    }
    ;

block:
    statements {
        LOGINFO("paracl: parser: rule: block -> statements");
        $$ = std::make_unique<ParaCL::BlockStmt>(std::move($1));
    }
    ;

one_stmt_block:
    statement {
        LOGINFO("paracl: parser: rule: one_stmt_block -> statement");
        std::vector<std::unique_ptr<ParaCL::Statement>> body_stmts;
        body_stmts.push_back(std::move($1));
        $$ = std::make_unique<ParaCL::BlockStmt>(std::move(body_stmts));
    }
    ;

string_constant:
    STRING {
        LOGINFO("paracl: parser: rule: string_constant -> STRING");
        $$ = std::make_unique<ParaCL::StringConstant>(std::move($1));
    }
    ;

%%
