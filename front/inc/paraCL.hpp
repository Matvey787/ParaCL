#ifndef PARACL_H
#define PARACL_H

#include "token_t.hpp"

#include <vector>
#include <variant>
#include <string>
#include <memory>

namespace ParaCL
{

// ================================ lexer ================================

namespace Lexer {

using tokenData_t = std::pair<token_t, std::variant<int, std::string>>;

std::vector<tokenData_t>
tokenize(const std::string& buffer);

void
dump(const std::vector<tokenData_t>& tokens);

}; // namespace Lexer

// ================================ parser ================================

namespace Parser {

// Abstract Syntax Tree (AST) node
struct ASTNode {
    virtual ~ASTNode() = default;
};

using ASTNodePtr = std::unique_ptr<ASTNode>;

// expressions
struct Expr : ASTNode {};

struct NumExpr : Expr {
    int value;
    NumExpr(int v) : value(v) {}
};

struct VarExpr final : Expr {
    std::string name;
    VarExpr(std::string n) : name(std::move(n)) {}
};

struct InputExpr final : Expr {};

struct BinExpr final : Expr {
    token_t op;
    ASTNodePtr left;
    ASTNodePtr right;
    BinExpr(token_t op, ASTNodePtr lhs, ASTNodePtr rhs)
        : op(op), left(std::move(lhs)), right(std::move(rhs)) {}
};

// statements (=, print, block, while)
struct Stmt : ASTNode {};

struct AssignStmt final : Stmt {
    std::string name;
    ASTNodePtr value;
    AssignStmt(std::string n, ASTNodePtr v)
        : name(std::move(n)), value(std::move(v)) {}
};

struct PrintStmt final : Stmt {
    ASTNodePtr expr;
    PrintStmt(ASTNodePtr e) : expr(std::move(e)) {}
};

struct BlockStmt final : Stmt {
    std::vector<std::unique_ptr<Stmt>> statements;
    BlockStmt() = default;
    BlockStmt(std::vector<std::unique_ptr<Stmt>> stmts)
        : statements(std::move(stmts)) {}
};

struct WhileStmt final : Stmt {
    std::unique_ptr<Expr> condition;
    std::unique_ptr<BlockStmt> body;
    WhileStmt(std::unique_ptr<Expr> cond, std::unique_ptr<BlockStmt> b)
        : condition(std::move(cond)), body(std::move(b)) {}
};

struct ProgramAST final : ASTNode {
    std::vector<std::unique_ptr<Stmt>> statements;
};

ProgramAST createAST(const std::vector<ParaCL::Lexer::tokenData_t>& tokens);
void dump(ProgramAST& progAST, const std::string& filename = "imgs/ast.dot");

}; // namespace Parser

// ================================ compiler ================================
namespace Compiler {

void compileByCpp(const ParaCL::Parser::ProgramAST& progAST);

}; // namespace Compiler

}; // namespace ParaCL


#endif