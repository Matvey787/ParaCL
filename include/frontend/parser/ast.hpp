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
struct Expression : ASTNode
{
};

struct NumExpr : Expression
{
    int value;
    NumExpr(int v) : value(v)
    {
    }
};

struct VarExpr : Expression
{
    std::string name;
    VarExpr(std::string n) : name(std::move(n))
    {
    }
};

struct InputExpr : Expression
{
};

struct UnExpr : Expression
{
    token_t op;
    std::unique_ptr<Expression> operand;
    UnExpr(token_t op, std::unique_ptr<Expression> v) : op(op), operand(std::move(v))
    {
    }
};

struct BinExpr : Expression
{
    token_t op;
    std::unique_ptr<Expression> left;
    std::unique_ptr<Expression> right;
    BinExpr(token_t op, std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs)
        : op(op), left(std::move(lhs)), right(std::move(rhs))
    {
    }
};

struct AssignExpr : Expression
{
    std::string name;
    std::unique_ptr<Expression> value;
    AssignExpr(std::string n, std::unique_ptr<Expression> v) : name(std::move(n)), value(std::move(v))
    {
    }
};

struct CombinedAssingExpr : Expression
{
    token_t op;
    std::string name;
    std::unique_ptr<Expression> value;
    CombinedAssingExpr(token_t op, std::string n, std::unique_ptr<Expression> value)
        : op(op), name(std::move(n)), value(std::move(value))
    {
    }
};

// statements
struct Statement : ASTNode
{
};

struct AssignStmt : Statement
{
    std::string name;
    std::unique_ptr<Expression> value;
    AssignStmt(std::string n, std::unique_ptr<Expression> v) : name(std::move(n)), value(std::move(v))
    {
    }
};

struct CombinedAssingStmt : Statement
{
    token_t op;
    std::string name;
    std::unique_ptr<Expression> value;
    CombinedAssingStmt(token_t op, std::string n, std::unique_ptr<Expression> value)
        : op(op), name(std::move(n)), value(std::move(value))
    {
    }
};

struct StringConstant : Expression
{
    std::string value;
    StringConstant(std::string str) : value(std::move(str))
    {
    }
};

struct PrintStmt : Statement
{
    std::vector<std::unique_ptr<Expression>> args;
    PrintStmt(std::vector<std::unique_ptr<Expression>> arguments) : args(std::move(arguments))
    {
    }
};

struct BlockStmt : Statement
{
    std::vector<std::unique_ptr<Statement>> statements;
    BlockStmt() = default;
    BlockStmt(std::vector<std::unique_ptr<Statement>> stmts) : statements(std::move(stmts))
    {
    }
};

struct WhileStmt : Statement
{
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStmt> body;
    WhileStmt(std::unique_ptr<Expression> cond, std::unique_ptr<BlockStmt> b)
        : condition(std::move(cond)), body(std::move(b))
    {
    }
};

struct IfStatement
{
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStmt> body;

    IfStatement(std::unique_ptr<Expression> cond, std::unique_ptr<BlockStmt> b)
        : condition(std::move(cond)), body(std::move(b))
    {
    }
};

struct ElifStatement
{
    std::unique_ptr<Expression> condition;
    std::unique_ptr<BlockStmt> body;
    ElifStatement(std::unique_ptr<Expression> cond, std::unique_ptr<BlockStmt> b)
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

struct ConditionStatement : Statement
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
    std::vector<std::unique_ptr<Statement>> statements;
};

}; /* namespace ParaCL */
