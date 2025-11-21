#pragma once

#include "parser/token_types.hpp"
#include <memory>
#include <string>
#include <vector>

namespace ParaCL
{

struct ASTNode
{
    virtual ~ASTNode() = default;
};

// expressions
struct Expr : ASTNode
{
};

struct NumExpr : Expr
{
    int value;
    NumExpr(int v) : value(v)
    {
    }
};

struct VarExpr : Expr
{
    std::string name;
    VarExpr(std::string n) : name(std::move(n))
    {
    }
};

struct InputExpr : Expr
{
};

struct UnExpr : Expr
{
    token_t op;
    std::unique_ptr<Expr> operand;
    UnExpr(token_t op, std::unique_ptr<Expr> v) : op(op), operand(std::move(v))
    {
    }
};

struct BinExpr : Expr
{
    token_t op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    BinExpr(token_t op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
        : op(op), left(std::move(lhs)), right(std::move(rhs))
    {
    }
};

struct AssignExpr : Expr
{
    std::string name;
    std::unique_ptr<Expr> value;
    AssignExpr(std::string n, std::unique_ptr<Expr> v) : name(std::move(n)), value(std::move(v))
    {
    }
};

struct CombinedAssingExpr : Expr
{
    token_t op;
    std::string name;
    std::unique_ptr<Expr> value;
    CombinedAssingExpr(token_t op, std::string n, std::unique_ptr<Expr> value)
        : op(op), name(std::move(n)), value(std::move(value))
    {
    }
};

// statements
struct Stmt : ASTNode
{
};

struct AssignStmt : Stmt
{
    std::string name;
    std::unique_ptr<Expr> value;
    AssignStmt(std::string n, std::unique_ptr<Expr> v) : name(std::move(n)), value(std::move(v))
    {
    }
};

struct CombinedAssingStmt : Stmt
{
    token_t op;
    std::string name;
    std::unique_ptr<Expr> value;
    CombinedAssingStmt(token_t op, std::string n, std::unique_ptr<Expr> value)
        : op(op), name(std::move(n)), value(std::move(value))
    {
    }
};

struct StringConstant : Expr
{
    std::string value;
    StringConstant(std::string str) : value(std::move(str))
    {
    }
};

struct PrintStmt : Stmt
{
    std::vector<std::unique_ptr<Expr>> args;
    PrintStmt(std::vector<std::unique_ptr<Expr>> arguments) : args(std::move(arguments))
    {
    }
};

struct BlockStmt : Stmt
{
    std::vector<std::unique_ptr<Stmt>> statements;
    BlockStmt() = default;
    BlockStmt(std::vector<std::unique_ptr<Stmt>> stmts) : statements(std::move(stmts))
    {
    }
};

struct WhileStmt : Stmt
{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> body;
    WhileStmt(std::unique_ptr<Expr> cond, std::unique_ptr<BlockStmt> b) : condition(std::move(cond)), body(std::move(b))
    {
    }
};

struct IfStatement
{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> body;

    IfStatement(std::unique_ptr<Expr> cond, std::unique_ptr<BlockStmt> b)
        : condition(std::move(cond)), body(std::move(b))
    {
    }
};

struct ElifStatement
{
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> body;
    ElifStatement(std::unique_ptr<Expr> cond, std::unique_ptr<BlockStmt> b)
        : condition(std::move(cond)), body(std::move(b))
    {
    }
};

struct ElseStatement
{
    std::unique_ptr<BlockStmt> body;
    ElseStatement(std::unique_ptr<BlockStmt> b) : body(std::move(b))
    {
    }
};

struct ConditionStatement : Stmt
{
    std::unique_ptr<IfStatement> if_stmt;
    std::vector<std::unique_ptr<ElifStatement>> elif_stmts;
    std::unique_ptr<ElseStatement> else_stmt;

    ConditionStatement(std::unique_ptr<IfStatement> base_if_stmt) : if_stmt(std::move(base_if_stmt))
    {
    }

    void add_elif_condition(std::unique_ptr<ElifStatement> elif_stmt)
    {
        elif_stmts.push_back(std::move(elif_stmt));
    }

    void add_else_condition(std::unique_ptr<ElseStatement> else_stmt_)
    {
        if (not else_stmt_.get())
            return;

        else_stmt = std::move(else_stmt_);
    }
};

struct ProgramAST
{
    std::vector<std::unique_ptr<Stmt>> statements;
};

}; // namespace ParaCL
