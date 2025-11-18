%{
#include <iostream>
#include <cstdio>
#include <unordered_map>
#include <memory>
#include <vector>
#include <string.h>
#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "global/global.hpp"

extern FILE* yyin;
extern int yyparse();

inline ParaCL::ProgramAST program;

extern std::string current_file;

typedef struct YYLTYPE YYLTYPE;
typedef union YYSTYPE YYSTYPE;

void show_error_context(YYLTYPE* loc);

void yyerror(YYLTYPE* loc, const char* msg, std::string possible_token = "");

int yylex(YYSTYPE* yylval_param, YYLTYPE* yylloc_param);

%}

%define api.pure full
%locations

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

%union {
    int                                     num_value      ;
    std   ::string*                         str_value      ;
    ParaCL::Stmt*                           stmt           ;
    ParaCL::Expr*                           expr           ;
    ParaCL::BlockStmt*                      block          ;
    ParaCL::ConditionStatement*             condition_stmt ;
    ParaCL::IfStatement*                    if_stmt        ;
    std   ::vector<ParaCL::ElifStatement*>* elif_stmts     ;
    ParaCL::ElifStatement*                  elif_stmt      ;
    ParaCL::ElseStatement*                  else_stmt      ;
    std   ::vector<ParaCL::Stmt*>*          stmt_vector    ;
}

%token <num_value> NUM
%token <str_value> VAR
%token LCIB RCIB LCUB RCUB
%token WH IN PRINT IF ELIF ELSE
%token SC

%type <stmt_vector> program statements
%type <block> block one_stmt_block
%type <stmt> statement assignment combined_assignment print_statement while_statement
%type <expr> expression assignment_expression logical_or_expression logical_and_expression equality_expression relational_expression additive_expression multiplicative_expression unary_expression factor

%type <condition_stmt> condition_statement
%type <if_stmt>               if_statement
%type <elif_stmts>          elif_statements
%type <else_stmt>           else_statement

%start program

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
    %empty {
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
    | WH LCIB expression RCIB one_stmt_block {
        $$ = new ParaCL::WhileStmt(
            std::unique_ptr<ParaCL::Expr>($3), 
            std::unique_ptr<ParaCL::BlockStmt>($5)
        );
    }
    ;

condition_statement:
    if_statement elif_statements else_statement {
        auto cond_stmt = new ParaCL::ConditionStatement(
            std::unique_ptr<ParaCL::IfStatement>($1)
        );

        cond_stmt->add_elif_conditions($2);
        delete $2;

        if ($3) cond_stmt->add_else_condition(std::unique_ptr<ParaCL::ElseStatement>($3));

        $$ = cond_stmt;
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
    %empty {
        $$ = new std::vector<ParaCL::ElifStatement*>();
    }
    | elif_statements ELIF LCIB expression RCIB LCUB block RCUB {
        $1->push_back(new ParaCL::ElifStatement(
            std::unique_ptr<ParaCL::Expr>($4),
            std::unique_ptr<ParaCL::BlockStmt>($7)
        ));
        $$ = $1;
    }
    | elif_statements ELIF LCIB expression RCIB one_stmt_block {
        $1->push_back(new ParaCL::ElifStatement(
            std::unique_ptr<ParaCL::Expr>($4),
            std::unique_ptr<ParaCL::BlockStmt>($6)
        ));
        $$ = $1;
    }
    ;

else_statement:
    %empty { $$ = nullptr; }
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
    assignment_expression { $$ = $1; }
    ;

assignment_expression:
    logical_or_expression { $$ = $1; }
    | VAR AS assignment_expression %prec AS {
        $$ = new ParaCL::AssignExpr(*$1, std::unique_ptr<ParaCL::Expr>($3));
        delete $1;
    }
    | VAR ADDASGN assignment_expression %prec ADDASGN {
        $$ = new ParaCL::CombinedAssingExpr(
            ParaCL::token_t::ADDASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        delete $1;
    }
    | VAR SUBASGN assignment_expression %prec SUBASGN {
        $$ = new ParaCL::CombinedAssingExpr(
            ParaCL::token_t::SUBASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        delete $1;
    }
    | VAR MULASGN assignment_expression %prec MULASGN {
        $$ = new ParaCL::CombinedAssingExpr(
            ParaCL::token_t::MULASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        delete $1;
    }
    | VAR DIVASGN assignment_expression %prec DIVASGN {
        $$ = new ParaCL::CombinedAssingExpr(
            ParaCL::token_t::DIVASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        delete $1;
    }
    ;

logical_or_expression:
    logical_and_expression { $$ = $1; }
    | logical_or_expression OR logical_and_expression %prec OR {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::OR,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

logical_and_expression:
    equality_expression { $$ = $1; }
    | logical_and_expression AND equality_expression %prec AND {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::AND,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

equality_expression:
    relational_expression { $$ = $1; }
    | equality_expression ISEQ relational_expression %prec ISEQ {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISEQ,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | equality_expression ISNE relational_expression %prec ISNE {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISNE,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

relational_expression:
    additive_expression { $$ = $1; }
    | relational_expression ISAB additive_expression %prec ISAB {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISAB,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | relational_expression ISABE additive_expression %prec ISABE {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISABE,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | relational_expression ISLS additive_expression %prec ISLS {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISLS,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | relational_expression ISLSE additive_expression %prec ISLSE {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISLSE,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

additive_expression:
    multiplicative_expression { $$ = $1; }
    | additive_expression ADD multiplicative_expression %prec ADD { 
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ADD,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | additive_expression SUB multiplicative_expression %prec SUB { 
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::SUB,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

multiplicative_expression:
    unary_expression { $$ = $1; }
    | multiplicative_expression MUL unary_expression %prec MUL { 
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::MUL, 
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | multiplicative_expression DIV unary_expression %prec DIV {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::DIV, 
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | multiplicative_expression REM unary_expression %prec REM {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::REM,
            std::unique_ptr<ParaCL::Expr>($1),
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

unary_expression:
    factor { $$ = $1; }
    | SUB unary_expression %prec NEG {
        $$ = new ParaCL::UnExpr(
            ParaCL::token_t::SUB,
            std::unique_ptr<ParaCL::Expr>($2)
        );
    }
    | NOT unary_expression %prec NOT {
        $$ = new ParaCL::UnExpr(
            ParaCL::token_t::NOT,
            std::unique_ptr<ParaCL::Expr>($2)
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
    | IN {
        $$ = new ParaCL::InputExpr();
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

void yyerror(YYLTYPE* loc, const char* msg, std::string possible_token)
{
    std::cerr << current_file << ":"
              << loc->first_line << ":"
              << loc->first_column << ":"
              " ---> " << msg << "\n";
    
    if (!possible_token.empty()) {
        std::cerr << "Did you mean: '" << possible_token << "'?\n";
    }
    
    show_error_context(loc);
}

void show_error_context(YYLTYPE* loc) {
    if (!yyin) return;
    
    long current_file_pos = ftell(yyin);
    
    // Go to start of the file
    rewind(yyin);
    
    char buffer[1024];
    int current_line = 1;

    while (current_line < loc->first_line && fgets(buffer, sizeof(buffer), yyin)) current_line++;

    if (current_line == loc->first_line && fgets(buffer, sizeof(buffer), yyin))
    {
        size_t len = strlen(buffer);

        if (len > 0 && buffer[len-1] == '\n') buffer[len-1] = '\0';
        
        std::cerr << loc->first_line << " | " << buffer << std::endl;
        
        std::cerr << "  | ";
        for (int i = 1; i < loc->first_column; i++)
        {
            if (i < (int)strlen(buffer) && buffer[i-1] == '\t')
                std::cerr << "    ";
            else
                std::cerr << " ";
        }
        std::cerr << "^--- Unexpected token" << std::endl;
    }
    
    // Restoring the position in the file
    fseek(yyin, current_file_pos, SEEK_SET);
}
