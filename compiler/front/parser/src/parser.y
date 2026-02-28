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

import ast;

extern FILE* yyin;
extern std::string current_file;
extern int current_num_value;
extern std::string current_var_value;
}

%code {
#include "parse_error.hpp"

#include "lexer.hpp"
#include "check_variables.hpp"

ParaCL::front::AST::ProgramAST program;
ParaCL::ParserNameTable name_table;

int yylex(yy::parser::semantic_type* yylval, yy::parser::location_type* yylloc);
}

%precedence OR
%precedence AND
%precedence ISEQ ISNE
%precedence ISAB ISABE ISLS ISLSE
%precedence ADD SUB
%precedence MUL DIV REM
%precedence NEG NOT
%precedence AS
%precedence ADDASGN SUBASGN MULASGN DIVASGN

%token <int> NUM
%token <std::string> VAR
%token LCIB RCIB LCUB RCUB
%token WH IN PRINT IF ELIF ELSE
%token SC COMMA
%token <std::string> STRING

%type <std::vector<ParaCL::front::AST::AnonNode>> statements print_args
%type <ParaCL::front::AST::AnonNode> statement assignment combined_assignment
%type <ParaCL::front::AST::AnonNode> print_statement while_statement condition_statement
%type <ParaCL::front::AST::AnonNode> expression assignment_expression logical_or_expression
%type <ParaCL::front::AST::AnonNode> logical_and_expression equality_expression relational_expression
%type <ParaCL::front::AST::AnonNode> additive_expression multiplicative_expression unary_expression factor string_constant
%type <ParaCL::front::AST::AnonNode> scope one_stmt_scope if_statement else_statement
%type <std::vector<ParaCL::front::AST::AnonNode>> elif_statements

%start program
%%

program:
    create_global_scope statements leave_global_scope {
        program = ParaCL::front::AST::ProgramAST(std::move($2));
    }
    ;

create_global_scope:
    %empty { name_table.new_scope(); }
    ;

leave_global_scope:
    %empty { name_table.leave_scope(); }
    ;

statements:
    %empty { $$ = std::vector<ParaCL::front::AST::AnonNode>(); }
    | statements statement {
        $1.push_back(std::move($2));
        $$ = std::move($1);
    }
    | statements LCUB scope RCUB {
        $1.push_back(std::move($3));
        $$ = std::move($1);
    }
    ;

statement:
    assignment SC          { $$ = std::move($1); }
    | combined_assignment SC { $$ = std::move($1); }
    | print_statement SC   { $$ = std::move($1); }
    | while_statement      { $$ = std::move($1); }
    | condition_statement  { $$ = std::move($1); }
    | SC                   { $$ = ParaCL::front::AST::make_node<ParaCL::front::AST::Scope>(); }
    ;

assignment:
    VAR AS expression {
        auto binop = ParaCL::front::AST::BinaryOperator(
            ParaCL::front::AST::BinaryOperator::BinaryOperatorT::ASGN,
            ParaCL::front::AST::make_node<ParaCL::front::AST::Variable>($1),
            std::move($3)
        );
        $$ = ParaCL::front::AST::make_node<decltype(binop)>(std::move(binop));
        name_table.declare_or_do_nothing_if_already_declared($1);
    }
    | VAR AS error {
        std::cerr << "paracl: parser: assignment -> VAR AS error\n";
        ErrorHandler::throwError(@3, "expected expression after assignment");
        YYABORT;
    }
    ;

combined_assignment:
    VAR ADDASGN expression {
        if (name_table.is_not_declare($1)) {
            std::cerr << "using undeclared variable: " << $1 << "\n";
            ErrorHandler::throwError(@1, "using undeclared variable: " + $1);
            YYABORT;
        }
        auto binop = ParaCL::front::AST::BinaryOperator(
            ParaCL::front::AST::BinaryOperator::BinaryOperatorT::ADDASGN,
            ParaCL::front::AST::make_node<ParaCL::front::AST::Variable>($1),
            std::move($3)
        );
        $$ = ParaCL::front::AST::make_node<decltype(binop)>(std::move(binop));
    }
    | VAR SUBASGN expression {
        if (name_table.is_not_declare($1)) {
            std::cerr << "using undeclared variable: " << $1 << "\n";
            ErrorHandler::throwError(@1, "using undeclared variable: " + $1);
            YYABORT;
        }
        auto binop = ParaCL::front::AST::BinaryOperator(
            ParaCL::front::AST::BinaryOperator::BinaryOperatorT::SUBASGN,
            ParaCL::front::AST::make_node<ParaCL::front::AST::Variable>($1),
            std::move($3)
        );
        $$ = ParaCL::front::AST::make_node<decltype(binop)>(std::move(binop));
    }
    | VAR MULASGN expression {
        if (name_table.is_not_declare($1)) {
            std::cerr << "using undeclared variable: " << $1 << "\n";
            ErrorHandler::throwError(@1, "using undeclared variable: " + $1);
            YYABORT;
        }
        auto binop = ParaCL::front::AST::BinaryOperator(
            ParaCL::front::AST::BinaryOperator::BinaryOperatorT::MULASGN,
            ParaCL::front::AST::make_node<ParaCL::front::AST::Variable>($1),
            std::move($3)
        );
        $$ = ParaCL::front::AST::make_node<decltype(binop)>(std::move(binop));
    }
    | VAR DIVASGN expression {
        if (name_table.is_not_declare($1)) {
            std::cerr << "using undeclared variable: " << $1 << "\n";
            ErrorHandler::throwError(@1, "using undeclared variable: " + $1);
            YYABORT;
        }
        auto binop = ParaCL::front::AST::BinaryOperator(
            ParaCL::front::AST::BinaryOperator::BinaryOperatorT::DIVASGN,
            ParaCL::front::AST::make_node<ParaCL::front::AST::Variable>($1),
            std::move($3)
        );
        $$ = ParaCL::front::AST::make_node<decltype(binop)>(std::move(binop));
    }
    /* ошибки для combined */
    | VAR ADDASGN error { ErrorHandler::throwError(@3, "expected expression after '+='"); YYABORT; }
    | VAR SUBASGN error { ErrorHandler::throwError(@3, "expected expression after '-='"); YYABORT; }
    | VAR MULASGN error { ErrorHandler::throwError(@3, "expected expression after '*='"); YYABORT; }
    | VAR DIVASGN error { ErrorHandler::throwError(@3, "expected expression after '/='"); YYABORT; }
    ;

print_statement:
    PRINT print_args {
        auto p = ParaCL::front::AST::Print(std::move($2));
        $$ = ParaCL::front::AST::make_node<ParaCL::front::AST::Print>(std::move(p));
    }
    | PRINT error {
        ErrorHandler::throwError(@2, "expected expressions after print");
        YYABORT;
    }
    ;

print_args:
    %empty { $$ = std::vector<ParaCL::front::AST::AnonNode>(); }
    | print_args expression               { $1.push_back(std::move($2)); $$ = std::move($1); }
    | print_args string_constant          { $1.push_back(std::move($2)); $$ = std::move($1); }
    | print_args COMMA expression         { $1.push_back(std::move($3)); $$ = std::move($1); }
    | print_args COMMA string_constant    { $1.push_back(std::move($3)); $$ = std::move($1); }
    ;

while_statement:
    WH LCIB expression RCIB LCUB scope RCUB {
        auto w = ParaCL::front::AST::While(std::move($3), std::move($6));
        $$ = ParaCL::front::AST::make_node<ParaCL::front::AST::While>(std::move(w));
    }
    | WH LCIB expression RCIB one_stmt_scope {
        auto w = ParaCL::front::AST::While(std::move($3), std::move($5));
        $$ = ParaCL::front::AST::make_node<ParaCL::front::AST::While>(std::move(w));
    }
    | WH LCIB error RCIB LCUB scope RCUB { ErrorHandler::throwError(@3, "expected condition in while"); YYABORT; }
    | WH LCIB expression error LCUB scope RCUB { ErrorHandler::throwError(@4, "expected ')' after while condition"); YYABORT; }
    | WH LCIB expression RCIB error { ErrorHandler::throwError(@5, "expected scope after while"); YYABORT; }
    | WH error { ErrorHandler::throwError(@2, "expected '(' after while"); YYABORT; }
    ;

condition_statement:
    if_statement elif_statements else_statement {
        ParaCL::front::AST::Condition cond;
        cond.addIf(std::move($1));
        for (auto&& e : $2) cond.addIf(std::move(e));
        if ($3) cond.setElse(std::move($3));
        $$ = ParaCL::front::AST::make_node<ParaCL::front::AST::Condition>(std::move(cond));
    }
    ;

if_statement:
    IF LCIB expression RCIB LCUB scope RCUB {
        auto i = ParaCL::front::AST::If(std::move($3), std::move($6));
        $$ = ParaCL::front::AST::make_node<ParaCL::front::AST::If>(std::move(i));
    }
    | IF LCIB expression RCIB one_stmt_scope {
        auto i = ParaCL::front::AST::If(std::move($3), std::move($5));
        $$ = ParaCL::front::AST::make_node<ParaCL::front::AST::If>(std::move(i));
    }
    /* ошибки для if */
    | IF LCIB error RCIB LCUB scope RCUB { ErrorHandler::throwError(@3, "expected condition in if"); YYABORT; }
    | IF LCIB expression error LCUB scope RCUB { ErrorHandler::throwError(@4, "expected ')' after if"); YYABORT; }
    | IF LCIB expression RCIB error { ErrorHandler::throwError(@5, "expected scope after if"); YYABORT; }
    | IF error { ErrorHandler::throwError(@2, "expected '(' after if"); YYABORT; }
    ;

elif_statements:
    %empty { $$ = std::vector<ParaCL::front::AST::AnonNode>(); }
    | elif_statements ELIF LCIB expression RCIB LCUB scope RCUB {
        auto e = ParaCL::front::AST::If(std::move($4), std::move($7));
        $1.push_back(ParaCL::front::AST::make_node<ParaCL::front::AST::If>(std::move(e)));
        $$ = std::move($1);
    }
    | elif_statements ELIF LCIB expression RCIB one_stmt_scope {
        auto e = ParaCL::front::AST::If(std::move($4), std::move($6));
        $1.push_back(ParaCL::front::AST::make_node<ParaCL::front::AST::If>(std::move(e)));
        $$ = std::move($1);
    }
    /* ошибки для elif */
    ;

else_statement:
    %empty { $$ = ParaCL::front::AST::AnonNode(); }
    | ELSE LCUB scope RCUB {
        auto e = ParaCL::front::AST::Else(std::move($3));
        $$ = ParaCL::front::AST::make_node<ParaCL::front::AST::Else>(std::move(e));
    }
    | ELSE one_stmt_scope {
        auto e = ParaCL::front::AST::Else(std::move($2));
        $$ = ParaCL::front::AST::make_node<ParaCL::front::AST::Else>(std::move(e));
    }
    | ELSE error { ErrorHandler::throwError(@2, "expected scope after else"); YYABORT; }
    ;

expression: assignment_expression { $$ = std::move($1); } ;

assignment_expression:
    logical_or_expression { $$ = std::move($1); }
    | VAR AS assignment_expression %prec AS {
        auto binop = ParaCL::front::AST::BinaryOperator(
            ParaCL::front::AST::BinaryOperator::BinaryOperatorT::ASGN,
            ParaCL::front::AST::make_node<ParaCL::front::AST::Variable>($1),
            std::move($3)
        );
        $$ = ParaCL::front::AST::make_node<decltype(binop)>(std::move(binop));
        name_table.declare_or_do_nothing_if_already_declared($1);
    }
    /* combined assignment expressions (в выражениях) — аналогично */
    ;

logical_or_expression:
    logical_and_expression { $$ = std::move($1); }
    | logical_or_expression OR logical_and_expression %prec OR {
        auto binop = ParaCL::front::AST::BinaryOperator(
            ParaCL::front::AST::BinaryOperator::BinaryOperatorT::OR,
            std::move($1), std::move($3)
        );
        $$ = ParaCL::front::AST::make_node<decltype(binop)>(std::move(binop));
    }
    ;

logical_and_expression:
    equality_expression { $$ = std::move($1); }
    | logical_and_expression AND equality_expression %prec AND {
        auto binop = ParaCL::front::AST::BinaryOperator(
            ParaCL::front::AST::BinaryOperator::BinaryOperatorT::AND,
            std::move($1), std::move($3)
        );
        $$ = ParaCL::front::AST::make_node<decltype(binop)>(std::move(binop));
    }
    ;

/* === Все остальные бинарные операторы === */
equality_expression:
    relational_expression { $$ = std::move($1); }
    | equality_expression ISEQ relational_expression %prec ISEQ {
        auto binop = ParaCL::front::AST::BinaryOperator(
            ParaCL::front::AST::BinaryOperator::BinaryOperatorT::ISEQ,
            std::move($1), std::move($3)
        );
        $$ = ParaCL::front::AST::make_node<decltype(binop)>(std::move(binop));
    }
    | equality_expression ISNE relational_expression %prec ISNE {
        auto binop = ParaCL::front::AST::BinaryOperator(
            ParaCL::front::AST::BinaryOperator::BinaryOperatorT::ISNE,
            std::move($1), std::move($3)
        );
        $$ = ParaCL::front::AST::make_node<decltype(binop)>(std::move(binop));
    }
    ;

relational_expression:
    additive_expression { $$ = std::move($1); }
    | relational_expression ISAB additive_expression %prec ISAB {
        auto binop = ParaCL::front::AST::BinaryOperator(
            ParaCL::front::AST::BinaryOperator::BinaryOperatorT::ISAB,
            std::move($1), std::move($3)
        );
        $$ = ParaCL::front::AST::make_node<decltype(binop)>(std::move(binop));
    }
    /* ISABE, ISLS, ISLSE — аналогично */
    ;

additive_expression:
    multiplicative_expression { $$ = std::move($1); }
    | additive_expression ADD multiplicative_expression %prec ADD {
        auto binop = ParaCL::front::AST::BinaryOperator(
            ParaCL::front::AST::BinaryOperator::BinaryOperatorT::ADD,
            std::move($1), std::move($3)
        );
        $$ = ParaCL::front::AST::make_node<decltype(binop)>(std::move(binop));
    }
    | additive_expression SUB multiplicative_expression %prec SUB {
        auto binop = ParaCL::front::AST::BinaryOperator(
            ParaCL::front::AST::BinaryOperator::BinaryOperatorT::SUB,
            std::move($1), std::move($3)
        );
        $$ = ParaCL::front::AST::make_node<decltype(binop)>(std::move(binop));
    }
    ;

multiplicative_expression:
    unary_expression { $$ = std::move($1); }
    | multiplicative_expression MUL unary_expression %prec MUL {
        auto binop = ParaCL::front::AST::BinaryOperator(
            ParaCL::front::AST::BinaryOperator::BinaryOperatorT::MUL,
            std::move($1), std::move($3)
        );
        $$ = ParaCL::front::AST::make_node<decltype(binop)>(std::move(binop));
    }
    | multiplicative_expression DIV unary_expression %prec DIV {
        auto binop = ParaCL::front::AST::BinaryOperator(
            ParaCL::front::AST::BinaryOperator::BinaryOperatorT::DIV,
            std::move($1), std::move($3)
        );
        $$ = ParaCL::front::AST::make_node<decltype(binop)>(std::move(binop));
    }
    | multiplicative_expression REM unary_expression %prec REM {
        auto binop = ParaCL::front::AST::BinaryOperator(
            ParaCL::front::AST::BinaryOperator::BinaryOperatorT::REM,
            std::move($1), std::move($3)
        );
        $$ = ParaCL::front::AST::make_node<decltype(binop)>(std::move(binop));
    }
    ;

unary_expression:
    factor { $$ = std::move($1); }
    | SUB unary_expression %prec NEG {
        auto unop = ParaCL::front::AST::UnaryOperator(
            ParaCL::front::AST::UnaryOperator::UnaryOperatorT::MINUS,
            std::move($2)
        );
        $$ = ParaCL::front::AST::make_node<decltype(unop)>(std::move(unop));
    }
    | NOT unary_expression %prec NOT {
        auto unop = ParaCL::front::AST::UnaryOperator(
            ParaCL::front::AST::UnaryOperator::UnaryOperatorT::NOT,
            std::move($2)
        );
        $$ = ParaCL::front::AST::make_node<decltype(unop)>(std::move(unop));
    }
    | ADD unary_expression %prec NEG { $$ = std::move($2); }
    ;

factor:
    NUM { $$ = ParaCL::front::AST::make_node<ParaCL::front::AST::NumberLiteral>($1); }
    | VAR {
        if (name_table.is_not_declare($1)) {
            std::cerr << "using undeclared variable: " << $1 << "\n";
            ErrorHandler::throwError(@1, "using undeclared variable: " + $1);
            YYABORT;
        }
        $$ = ParaCL::front::AST::make_node<ParaCL::front::AST::Variable>($1);
    }
    | LCIB expression RCIB { $$ = std::move($2); }
    | IN { $$ = ParaCL::front::AST::make_node<ParaCL::front::AST::Scan>(); }
    | STRING { $$ = ParaCL::front::AST::make_node<ParaCL::front::AST::StringLiteral>($1); }
    ;

scope:
    scope_enter_action statements scope_leave_action {
        $$ = ParaCL::front::AST::make_node<ParaCL::front::AST::Scope>(std::move($2));
    }
    ;

one_stmt_scope:
    scope_enter_action statement scope_leave_action {
        std::vector<ParaCL::front::AST::AnonNode> vec{std::move($2)};
        $$ = ParaCL::front::AST::make_node<ParaCL::front::AST::Scope>(std::move(vec));
    }
    ;

scope_enter_action: %empty { name_table.new_scope(); } ;
scope_leave_action: %empty { name_table.leave_scope(); } ;

string_constant:
    STRING { $$ = ParaCL::front::AST::make_node<ParaCL::front::AST::StringLiteral>($1); }
    ;

%%
