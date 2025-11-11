#ifndef PARACL_H
#define PARACL_H

#include "token_t.hpp"
#include <vector>
#include <string>
#include <memory>

namespace ParaCL
{

namespace Parser {

struct ASTNode {
    virtual ~ASTNode() = default;
};

// expressions
struct Expr : ASTNode {};

struct NumExpr : Expr {
    int value;
    NumExpr(int v) : value(v) {}
};

struct VarExpr : Expr {
    std::string name;
    VarExpr(std::string n) : name(std::move(n)) {}
};

struct InputExpr : Expr {};

struct BinExpr : Expr {
    token_t op;
    std::unique_ptr<Expr> left;
    std::unique_ptr<Expr> right;
    BinExpr(token_t op, std::unique_ptr<Expr> lhs, std::unique_ptr<Expr> rhs)
        : op(op), left(std::move(lhs)), right(std::move(rhs)) {}
};

struct AssignExpr : Expr {
    std::string name;
    std::unique_ptr<Expr> value;
    AssignExpr(std::string n, std::unique_ptr<Expr> v)
        : name(std::move(n)), value(std::move(v)) {}
};

// statements
struct Stmt : ASTNode {};

struct AssignStmt : Stmt {
    std::string name;
    std::unique_ptr<Expr> value;
    AssignStmt(std::string n, std::unique_ptr<Expr> v)
        : name(std::move(n)), value(std::move(v)) {}
};

struct PrintStmt : Stmt {
    std::unique_ptr<Expr> expr;
    PrintStmt(std::unique_ptr<Expr> e) : expr(std::move(e)) {}
};

struct BlockStmt : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
    BlockStmt() = default;
    BlockStmt(std::vector<std::unique_ptr<Stmt>> stmts)
        : statements(std::move(stmts)) {}
};

struct WhileStmt : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> body;
    WhileStmt(std::unique_ptr<Expr> cond, std::unique_ptr<BlockStmt> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

struct ProgramAST {
    std::vector<std::unique_ptr<Stmt>> statements;
};

}; // namespace Parser
}; // namespace ParaCL

#endif
