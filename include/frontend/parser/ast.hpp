#pragma once

#include "parser/token_types.hpp"
#include <memory>
#include <string>
#include <vector>

namespace ParaCL
{

class ASTNode
{
public:
    virtual ~ASTNode() = default;
};

// expressions
class Expression  : public ASTNode
{
};

class NumExpr final : public Expression
{
    int value_;
    
public:
    explicit NumExpr(int value) : value_(value) {}
    
    int get_value() const { return value_; }
    void set_value(int value) { value_ = value; }
};

class VarExpr final : public Expression
{
    std::string name_;
    
public:
    explicit VarExpr(std::string name) : name_(std::move(name)) {}
    
    const std::string& get_name() const { return name_; }
    void set_name(std::string name) { name_ = std::move(name); }
};

class InputExpr final : public Expression
{
};

class UnExpr final : public Expression
{
    unary_op_t op_;
    std::unique_ptr<Expression> operand_;
    
public:
    UnExpr(unary_op_t op, std::unique_ptr<Expression> operand) 
        : op_(op), operand_(std::move(operand)) {}
    
    unary_op_t get_op() const { return op_; }
    void set_op(unary_op_t op) { op_ = op; }
    
    Expression* get_operand() const { return operand_.get(); }
    std::unique_ptr<Expression> release_operand() { return std::move(operand_); }
    void set_operand(std::unique_ptr<Expression> operand) { operand_ = std::move(operand); }
};

class BinExpr final : public Expression
{
    binary_op_t op_;
    std::unique_ptr<Expression> left_;
    std::unique_ptr<Expression> right_;
    
public:
    BinExpr(binary_op_t op, std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : op_(op), left_(std::move(left)), right_(std::move(right)) {}
    
    binary_op_t get_op() const { return op_; }
    void set_op(binary_op_t op) { op_ = op; }
    
    Expression* get_left() const { return left_.get(); }
    Expression* get_right() const { return right_.get(); }
    
    std::unique_ptr<Expression> release_left() { return std::move(left_); }
    std::unique_ptr<Expression> release_right() { return std::move(right_); }
    
    void set_left(std::unique_ptr<Expression> left) { left_ = std::move(left); }
    void set_right(std::unique_ptr<Expression> right) { right_ = std::move(right); }
};

class AssignExpr final : public Expression
{
    std::string name_;
    std::unique_ptr<Expression> value_;
    
public:
    AssignExpr(std::string name, std::unique_ptr<Expression> value)
        : name_(std::move(name)), value_(std::move(value)) {}
    
    const std::string& get_name() const { return name_; }
    void set_name(std::string name) { name_ = std::move(name); }
    
    Expression* get_value() const { return value_.get(); }
    std::unique_ptr<Expression> release_value() { return std::move(value_); }
    void set_value(std::unique_ptr<Expression> value) { value_ = std::move(value); }
};

class CombinedAssignExpr final : public Expression
{
    combined_assign_t op_;
    std::string name_;
    std::unique_ptr<Expression> value_;
    
public:
    CombinedAssignExpr(combined_assign_t op, std::string name, std::unique_ptr<Expression> value)
        : op_(op), name_(std::move(name)), value_(std::move(value)) {}
    
    combined_assign_t get_op() const { return op_; }
    void set_op(combined_assign_t op) { op_ = op; }
    
    const std::string& get_name() const { return name_; }
    void set_name(std::string name) { name_ = std::move(name); }
    
    Expression* get_value() const { return value_.get(); }
    std::unique_ptr<Expression> release_value() { return std::move(value_); }
    void set_value(std::unique_ptr<Expression> value) { value_ = std::move(value); }
};

// statements
class Statement : public ASTNode
{
};

class AssignStmt final : public Statement
{
    std::string name_;
    std::unique_ptr<Expression> value_;
    
public:
    AssignStmt(std::string name, std::unique_ptr<Expression> value)
        : name_(std::move(name)), value_(std::move(value)) {}
    
    const std::string& get_name() const { return name_; }
    void set_name(std::string name) { name_ = std::move(name); }
    
    Expression* get_value() const { return value_.get(); }
    std::unique_ptr<Expression> release_value() { return std::move(value_); }
    void set_value(std::unique_ptr<Expression> value) { value_ = std::move(value); }
};

class CombinedAssignStmt final : public Statement
{
    combined_assign_t op_;
    std::string name_;
    std::unique_ptr<Expression> value_;
    
public:
    CombinedAssignStmt(combined_assign_t op, std::string name, std::unique_ptr<Expression> value)
        : op_(op), name_(std::move(name)), value_(std::move(value)) {}
    
    combined_assign_t get_op() const { return op_; }
    void set_op(combined_assign_t op) { op_ = op; }
    
    const std::string& get_name() const { return name_; }
    void set_name(std::string name) { name_ = std::move(name); }
    
    Expression* get_value() const { return value_.get(); }
    std::unique_ptr<Expression> release_value() { return std::move(value_); }
    void set_value(std::unique_ptr<Expression> value) { value_ = std::move(value); }
};

class StringConstant final : public Expression
{
    std::string value_;
    
public:
    explicit StringConstant(std::string value) : value_(std::move(value)) {}
    
    const std::string& get_value() const { return value_; }
    void set_value(std::string value) { value_ = std::move(value); }
};

class PrintStmt : public Statement
{
    std::vector<std::unique_ptr<Expression>> args_;
    
public:
    explicit PrintStmt(std::vector<std::unique_ptr<Expression>> args = {})
        : args_(std::move(args)) {}
    
    size_t get_arg_count() const { return args_.size(); }
    Expression* get_arg(size_t index) const { return index < args_.size() ? args_[index].get() : nullptr; }
    
    void add_arg(std::unique_ptr<Expression> arg) { args_.push_back(std::move(arg)); }
    
    std::unique_ptr<Expression> release_arg(size_t index) {
        if (index >= args_.size()) return nullptr;
        auto arg = std::move(args_[index]);
        args_.erase(args_.begin() + index);
        return arg;
    }
    
    const std::vector<std::unique_ptr<Expression>>& get_args() const { return args_; }
};

class BlockStmt final : public Statement
{
    std::vector<std::unique_ptr<Statement>> statements_;
    
public:
    BlockStmt() = default;
    explicit BlockStmt(std::vector<std::unique_ptr<Statement>> statements)
        : statements_(std::move(statements)) {}
    
    size_t get_statement_count() const { return statements_.size(); }
    Statement* get_statement(size_t index) const { return index < statements_.size() ? statements_[index].get() : nullptr; }
    
    void add_statement(std::unique_ptr<Statement> stmt) { statements_.push_back(std::move(stmt)); }
    
    std::unique_ptr<Statement> release_statement(size_t index) {
        if (index >= statements_.size()) return nullptr;
        auto stmt = std::move(statements_[index]);
        statements_.erase(statements_.begin() + index);
        return stmt;
    }
    
    const std::vector<std::unique_ptr<Statement>>& get_statements() const { return statements_; }
};

class WhileStmt final : public Statement
{
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<BlockStmt> body_;
    
public:
    WhileStmt(std::unique_ptr<Expression> condition, std::unique_ptr<BlockStmt> body)
        : condition_(std::move(condition)), body_(std::move(body)) {}
    
    Expression* get_condition() const { return condition_.get(); }
    BlockStmt* get_body() const { return body_.get(); }
    
    std::unique_ptr<Expression> release_condition() { return std::move(condition_); }
    std::unique_ptr<BlockStmt> release_body() { return std::move(body_); }
    
    void set_condition(std::unique_ptr<Expression> condition) { condition_ = std::move(condition); }
    void set_body(std::unique_ptr<BlockStmt> body) { body_ = std::move(body); }
};

class IfStatement
{
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<BlockStmt> body_;
    
public:
    IfStatement(std::unique_ptr<Expression> condition, std::unique_ptr<BlockStmt> body)
        : condition_(std::move(condition)), body_(std::move(body)) {}
    
    Expression* get_condition() const { return condition_.get(); }
    BlockStmt* get_body() const { return body_.get(); }
    
    std::unique_ptr<Expression> release_condition() { return std::move(condition_); }
    std::unique_ptr<BlockStmt> release_body() { return std::move(body_); }
    
    void set_condition(std::unique_ptr<Expression> condition) { condition_ = std::move(condition); }
    void set_body(std::unique_ptr<BlockStmt> body) { body_ = std::move(body); }
};

class ElifStatement
{
    std::unique_ptr<Expression> condition_;
    std::unique_ptr<BlockStmt> body_;
    
public:
    ElifStatement(std::unique_ptr<Expression> condition, std::unique_ptr<BlockStmt> body)
        : condition_(std::move(condition)), body_(std::move(body)) {}
    
    Expression* get_condition() const { return condition_.get(); }
    BlockStmt* get_body() const { return body_.get(); }
    
    std::unique_ptr<Expression> release_condition() { return std::move(condition_); }
    std::unique_ptr<BlockStmt> release_body() { return std::move(body_); }
    
    void set_condition(std::unique_ptr<Expression> condition) { condition_ = std::move(condition); }
    void set_body(std::unique_ptr<BlockStmt> body) { body_ = std::move(body); }
};

class ElseStatement
{
    std::unique_ptr<BlockStmt> body_;
    
public:
    explicit ElseStatement(std::unique_ptr<BlockStmt> body) : body_(std::move(body)) {}
    
    BlockStmt* get_body() const { return body_.get(); }
    
    std::unique_ptr<BlockStmt> release_body() { return std::move(body_); }
    void set_body(std::unique_ptr<BlockStmt> body) { body_ = std::move(body); }
};

class ConditionStatement final : public Statement
{
    std::unique_ptr<IfStatement> if_stmt_;
    std::vector<std::unique_ptr<ElifStatement>> elif_stmts_;
    std::unique_ptr<ElseStatement> else_stmt_;
    
public:
    explicit ConditionStatement(std::unique_ptr<IfStatement> if_stmt)
        : if_stmt_(std::move(if_stmt)) {}
    
    IfStatement* get_if_stmt() const { return if_stmt_.get(); }
    ElseStatement* get_else_stmt() const { return else_stmt_.get(); }
    size_t get_elif_count() const { return elif_stmts_.size(); }
    ElifStatement* get_elif_stmt(size_t index) const {
        return index < elif_stmts_.size() ? elif_stmts_[index].get() : nullptr;
    }
    
    std::unique_ptr<IfStatement> release_if_stmt() { return std::move(if_stmt_); }
    std::unique_ptr<ElseStatement> release_else_stmt() { return std::move(else_stmt_); }
    
    void set_if_stmt(std::unique_ptr<IfStatement> if_stmt) { if_stmt_ = std::move(if_stmt); }
    
    void add_elif(std::unique_ptr<ElifStatement> elif_stmt) {
        elif_stmts_.push_back(std::move(elif_stmt));
    }
    
    void set_else_stmt(std::unique_ptr<ElseStatement> else_stmt) {
        else_stmt_ = std::move(else_stmt);
    }
    
    const std::vector<std::unique_ptr<ElifStatement>>& get_elif_stmts() const { return elif_stmts_; }
};

class ProgramAST final
{
    std::vector<std::unique_ptr<Statement>> statements_;
    
public:
    ProgramAST() = default;
    explicit ProgramAST(std::vector<std::unique_ptr<Statement>> statements)
        : statements_(std::move(statements)) {}
    
    size_t get_statement_count() const { return statements_.size(); }
    Statement* get_statement(size_t index) const { return index < statements_.size() ? statements_[index].get() : nullptr; }
    
    void add_statement(std::unique_ptr<Statement> stmt) { statements_.push_back(std::move(stmt)); }
    
    std::unique_ptr<Statement> release_statement(size_t index) {
        if (index >= statements_.size()) return nullptr;
        auto stmt = std::move(statements_[index]);
        statements_.erase(statements_.begin() + index);
        return stmt;
    }
    
    const std::vector<std::unique_ptr<Statement>>& get_statements() const { return statements_; }
};

} // namespace ParaCL