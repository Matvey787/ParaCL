%{

#include <iostream>
#include <cstdio>
#include <unordered_map>
#include <memory>
#include <vector>
#include "paraCL_crutch_for_parsery.hpp"
#include "lexer.hpp"
#include "global/global.hpp"

extern FILE* yyin;
extern int yyparse();

#ifdef DEBUG
    #define DEBUG_COUT_M(message)                \
        do {                                     \
            std::cout << (message) << std::endl; \
        } while(0)
#else
    #define DEBUG_COUT_M(message)                \
        do { } while(0)
#endif

ParaCL::ProgramAST program;

extern std::string current_file;

typedef struct YYLTYPE YYLTYPE;
typedef union YYSTYPE YYSTYPE;

void yyerror(YYLTYPE* loc, const char* msg);
int yylex(YYSTYPE* yylval_param, YYLTYPE* yylloc_param);

%}

%define api.pure full
%locations

%union {
    int num_value;
    std::string* str_value;
    ParaCL::Stmt* stmt;
    ParaCL::Expr* expr;
    std::vector<ParaCL::Stmt*>* stmt_vector;
}

%token <num_value> NUM
%token <str_value> VAR
%token AND OR
%token ADDASGN SUBASGN MULASGN DIVASGN
%token ADD SUB MUL DIV
%token ISAB ISABE ISLS ISLSE ISEQ ISNE
%token LCIB RCIB LCUB RCUB
%token WH IN AS PRINT
%token SC
%token IF // ELIF ELSE

%type <stmt_vector> program statements
%type <stmt> statement assignment combined_assignment print_statement while_statement input_statement if_statement // condition_statement // lif_statement else_statement
%type <expr> expression bool_expression and_or_expression math_comparison_expression add_sub_expression mul_div_expression factor assignment_expression

%%

program:
    statements {
        for (auto stmt : *$1) {
            // std::cerr << "token {" << yylloc.first_line << ":" << yylloc.first_column << "}\n";
            // std::cerr << format_location(yylloc) << "\n";
            program.statements.push_back(std::unique_ptr<ParaCL::Stmt>(stmt));
        }
        delete $1;
        DEBUG_COUT_M("AST construction completed!"); 
    }
    ;

statements:
    {
        /* FIXME: stament on unique_ptr*/
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
    | if_statement {
        $$ = $1;
    }
    ;

assignment:
    VAR AS expression {
        $$ = new ParaCL::AssignStmt(*$1, std::unique_ptr<ParaCL::Expr>($3));
        ON_DEBUG(std::cout << "Created assignment for variable " << *$1 << std::endl;)
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
        ON_DEBUG(std::cout << "Created add assignment for variable " << *$1 << std::endl;)
        delete $1;
    }
    | VAR SUBASGN expression {
        $$ = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::SUBASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        ON_DEBUG(std::cout << "Created sub assignment for variable " << *$1 << std::endl;)
        delete $1;
    }
    | VAR MULASGN expression {
        $$ = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::MULASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        ON_DEBUG(std::cout << "Created mul assignment for variable " << *$1 << std::endl;)
        delete $1;
    }
    | VAR DIVASGN expression {
        $$ = new ParaCL::CombinedAssingStmt(
            ParaCL::token_t::DIVASGN,
            *$1,
            std::unique_ptr<ParaCL::Expr>($3)
        );
        ON_DEBUG(std::cout << "Created div assignment for variable " << *$1 << std::endl;)
        delete $1;
    }
    ;

print_statement:
    PRINT expression {
        $$ = new ParaCL::PrintStmt(std::unique_ptr<ParaCL::Expr>($2));
    }
    ;

while_statement:
    WH LCIB expression RCIB LCUB statements RCUB {
        auto body_stmts = std::vector<std::unique_ptr<ParaCL::Stmt>>();
        for (auto stmt : *$6) {
            body_stmts.push_back(std::unique_ptr<ParaCL::Stmt>(stmt));
        }
        auto body = new ParaCL::BlockStmt(std::move(body_stmts));
        $$ = new ParaCL::WhileStmt(
            std::unique_ptr<ParaCL::Expr>($3), 
            std::unique_ptr<ParaCL::BlockStmt>(body)
        );
        ON_DEBUG(std::cout << "Created while statement" << std::endl;)
        delete $6;
    }
    | WH LCIB expression RCIB statement {
        auto body_stmts = std::vector<std::unique_ptr<ParaCL::Stmt>>();
        body_stmts.push_back(std::unique_ptr<ParaCL::Stmt>($5));
        auto body = new ParaCL::BlockStmt(std::move(body_stmts));
    
        $$ = new ParaCL::WhileStmt(
            std::unique_ptr<ParaCL::Expr>($3), 
            std::unique_ptr<ParaCL::BlockStmt>(body)
        );
        ON_DEBUG(std::cout << "Created while statement" << std::endl;)
    }
    ;

input_statement:
    IN AS VAR {
    }
    ;

/* condition_statement:
    if_statement { 
        $$ = new ParaCL::ConditionStatement(
            std::unique_ptr<ParaCL::IfStatement>($1)
        );
    }
    ; */

if_statement:
    IF LCIB expression RCIB LCUB statements RCUB {
        auto body_stmts = std::vector<std::unique_ptr<ParaCL::Stmt>>();
        for (auto stmt : *$6) {
            body_stmts.push_back(std::unique_ptr<ParaCL::Stmt>(stmt));
        }
        auto body = new ParaCL::BlockStmt(std::move(body_stmts));
        $$ = new ParaCL::IfStatement(
            std::unique_ptr<ParaCL::Expr>($3),
            std::unique_ptr<ParaCL::BlockStmt>(body)
        );
        ON_DEBUG(std::cout << "Created if statement with {}" << std::endl;)
        delete $6;
    }
    | IF LCIB expression RCIB statement {
        auto body_stmts = std::vector<std::unique_ptr<ParaCL::Stmt>>();
        body_stmts.push_back(std::unique_ptr<ParaCL::Stmt>($5));
        auto body = new ParaCL::BlockStmt(std::move(body_stmts));

        $$ = new ParaCL::IfStatement(
            std::unique_ptr<ParaCL::Expr>($3),
            std::unique_ptr<ParaCL::BlockStmt>(body)
        );
        ON_DEBUG(std::cout << "Created if statement whithOUT {}" << std::endl;)
    }
    ;

/* elif_statement:
    { ELIF LCIB expression RCIB LCUB statement RCUB }  { $$ = }
    | elif_statement
    ; */

expression:
    bool_expression { $$ = $1; }
    ;

bool_expression:
    and_or_expression { $$ = $1; }
    ;

and_or_expression:
    math_comparison_expression { $$ = $1; }
    | and_or_expression AND math_comparison_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::AND,
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | and_or_expression OR math_comparison_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::OR,
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

math_comparison_expression:
    add_sub_expression { $$ = $1; }
    | math_comparison_expression ISAB add_sub_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISAB,
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | math_comparison_expression ISABE add_sub_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISABE,
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | math_comparison_expression ISLS add_sub_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISLS,
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | math_comparison_expression ISLSE add_sub_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISLSE,
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | math_comparison_expression ISEQ add_sub_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISEQ,
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | math_comparison_expression ISNE add_sub_expression {
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ISNE,
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

add_sub_expression:
    mul_div_expression { $$ = $1; }
    | add_sub_expression ADD mul_div_expression { 
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::ADD, 
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | add_sub_expression SUB mul_div_expression { 
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::SUB, 
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    ;

mul_div_expression:
    factor { $$ = $1; }
    | mul_div_expression MUL factor { 
        $$ = new ParaCL::BinExpr(
            ParaCL::token_t::MUL, 
            std::unique_ptr<ParaCL::Expr>($1), 
            std::unique_ptr<ParaCL::Expr>($3)
        );
    }
    | mul_div_expression DIV factor { 
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

%%

/* int yylex(YYSTYPE* yylval_param, YYLTYPE* yylloc_param); */


void yyerror(YYLTYPE* loc, const char* msg)
{
    std::cerr << current_file     << ":"
              << loc->first_line   << ":"
              << loc->first_column << ":"
              " paracl: error: " << msg << "\n";
}
