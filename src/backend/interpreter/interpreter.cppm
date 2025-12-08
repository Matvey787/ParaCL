module;

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "global/global.hpp"
#include "parser/parser.hpp"

#include "log/log_api.hpp"

export module paracl_interpreter;

import interpreter_name_table;

namespace ParaCL
{

export class Interpreter
{
  private:
    const ProgramAST &ast_;
    InterpreterNameTable nametable_;

    /* Statement execution functions */
    void execute(const Statement *stmt);
    void execute(const AssignStmt *assign);
    void execute(const CombinedAssignStmt *combined_assign_statement);
    void execute(const PrintStmt *print_statement);
    void execute(const WhileStmt *while_stmt);
    void execute(const BlockStmt *block);
    void execute(const ConditionStatement *condition);

    /* Expression execution functions */
    int execute(const Expression *expr);
    int execute(const BinExpr *bin);
    int execute(const UnExpr *un);
    int execute(const NumExpr *num);
    int execute(const VarExpr *var);
    int execute([[maybe_unused]] const InputExpr *in);
    int execute(const AssignExpr *assignExpr);
    int execute(const CombinedAssignExpr *combinedAssignExpr);

    int execute(int lhs, int rhs, binary_op_t binary_operator);
    int execute(int lhs, int value, combined_assign_t combined_assign);
    int execute(int rhs, unary_op_t unary_operator);

  public:
    Interpreter(const ProgramAST &ast);

    void interpret();
};

Interpreter::Interpreter(const ProgramAST &ast) : ast_(ast), nametable_()
{
    LOGINFO("paracl: interpreter: ctor");

    /* create global scope */
    nametable_.new_scope();
}

void Interpreter::interpret()
{
    LOGINFO("paracl: interpreter: start interpret");

    for (const auto &stmt : ast_.get_statements())
        execute(stmt.get());

    LOGINFO("paracl: interpreter: interpret completed");
}

void Interpreter::execute(const Statement *stmt)
{
    if (auto assign = dynamic_cast<const AssignStmt *>(stmt))
        return execute(assign);

    if (auto combined_assign_statement = dynamic_cast<const CombinedAssignStmt *>(stmt))
        return execute(combined_assign_statement);

    if (auto print_statement = dynamic_cast<const PrintStmt *>(stmt))
        return execute(print_statement);

    if (auto while_stmt = dynamic_cast<const WhileStmt *>(stmt))
        return execute(while_stmt);

    if (auto block = dynamic_cast<const BlockStmt *>(stmt))
        return execute(block);

    if (auto condition = dynamic_cast<const ConditionStatement *>(stmt))
        return execute(condition);

    builtin_unreachable_wrapper("we must return in some else-if");
}

int Interpreter::execute(const Expression *expr)
{
    if (auto bin = dynamic_cast<const BinExpr *>(expr))
        return execute(bin);

    if (auto un = dynamic_cast<const UnExpr *>(expr))
        return execute(un);

    if (auto num = dynamic_cast<const NumExpr *>(expr))
        return execute(num);

    if (auto var = dynamic_cast<const VarExpr *>(expr))
        return execute(var);

    if (auto in = dynamic_cast<const InputExpr *>(expr))
        return execute(in);

    if (auto assignExpr = dynamic_cast<const AssignExpr *>(expr))
        return execute(assignExpr);

    if (auto combinedAssignExpr = dynamic_cast<const CombinedAssignExpr *>(expr))
        return execute(combinedAssignExpr);

    builtin_unreachable_wrapper("we must return in some else-if");
}

void Interpreter::execute(const WhileStmt *while_stmt)
{
    LOGINFO("paracl: interpreter: execute WHILE statement");

    nametable_.new_scope();
    
    const BlockStmt* body = while_stmt->get_body();
    msg_assert(body, "While statement must have a body");

    while (execute(while_stmt->get_condition()))
    {
        const auto& bodyStmts = body->get_statements();
        for (const auto &s : bodyStmts)
            execute(s.get());
    }

    nametable_.leave_scope();
    LOGINFO("paracl: interpreter: leave_scope WHILE statement");
}

void Interpreter::execute(const BlockStmt *block)
{
    LOGINFO("paracl: interpreter: execute block statement");

    const auto &blockStmts = block->get_statements();
    bool blockIsEmpty = blockStmts.empty();

    if (not blockIsEmpty)
        nametable_.new_scope();

    for (const auto &s : blockStmts)
        execute(s.get());

    if (not blockIsEmpty)
        nametable_.leave_scope();
}

void Interpreter::execute(const ConditionStatement *condition)
{
    LOGINFO("paracl: interpreter: execute condition statement");

    const IfStatement* if_stmt = condition->get_if_stmt();
    msg_assert(if_stmt, "in condition we always expect IF");

    bool flag = execute(if_stmt->get_condition());
    LOGINFO("paracl: interpreter: condition in IF: {}", flag ? "true" : "false");
    if (flag)
        return execute(if_stmt->get_body());

    for (size_t i = 0; i < condition->get_elif_count(); ++i)
    {
        const ElifStatement* elif_stmt = condition->get_elif_stmt(i);
        msg_assert(elif_stmt, "Elif statement is null");
        
        flag = execute(elif_stmt->get_condition());
        LOGINFO("paracl: interpreter: condition in ELSE-IF: {}", flag ? "true" : "false");

        if (not flag)
            continue;

        return execute(elif_stmt->get_body());
    }

    LOGINFO("paracl: interpreter: no ELSE in condition");

    const ElseStatement* else_stmt = condition->get_else_stmt();

    if (not else_stmt)
        return; /* no ELSE */

    LOGINFO("paracl: interpreter: condition ELSE");

    return execute(else_stmt->get_body());
}

int Interpreter::execute(const BinExpr *bin)
{
    const Expression* leftExpr = bin->get_left();
    const Expression* rightExpr = bin->get_right();

    msg_assert(leftExpr, "left bin expr child is not expr");
    msg_assert(rightExpr, "right bin expr child is not expr");

    int leftResult = execute(leftExpr);
    int rightResult = execute(rightExpr);
    int result = execute(leftResult, rightResult, bin->get_op());

    LOGINFO("paracl: interpreter: execute binary expression: {}", result);

    return result;
}

int Interpreter::execute(const UnExpr *un)
{
    const Expression* child_expr = un->get_operand();
    msg_assert(child_expr, "unary operand is not expression");
    
    int child_value = execute(child_expr);
    int result = execute(child_value, un->get_op());
    LOGINFO("paracl: interpreter: execute unary expression: {}", result);
    return result;
}

int Interpreter::execute(const NumExpr *num)
{
    int value = num->get_value();
    LOGINFO("paracl: interpreter: get number value: {}", value);
    return value;
}

int Interpreter::execute(const VarExpr *var)
{
    const std::string& name = var->get_name();
    std::optional<int> varValue = nametable_.get_variable_value(name);
    if (not varValue.has_value())
        throw std::runtime_error("'" + name + "' was not declared in this scope\n");

    LOGINFO("paracl: interpreter: get variable value: \"{}\" = {}", name, varValue.value());

    return varValue.value();
}

int Interpreter::execute([[maybe_unused]] const InputExpr *in)
{
    int value = 0;
    std::cin >> value;
    LOGINFO("paracl: interpreter: read input value: {}", value);
    return value;
}

int Interpreter::execute(const AssignExpr *assignExpr)
{
    const Expression* e = assignExpr->get_value();
    msg_assert(e, "AssignExpr value is not Expression");

    int result = execute(e);
    const std::string& name = assignExpr->get_name();
    
    nametable_.set_value(name, result);

    LOGINFO("paracl: interpreter: execute assign expression: \"{}\" = {}", name, result);

    return result;
}

int Interpreter::execute(const CombinedAssignExpr *combinedAssignExpr)
{
    const std::string& name = combinedAssignExpr->get_name();
    std::optional<int> varValue = nametable_.get_variable_value(name);
    
    if (not varValue.has_value())
        throw std::runtime_error("error: '" + name + "' was not declared in this scope\n");

    const Expression* expr = combinedAssignExpr->get_value();
    msg_assert(expr, "CombinedAssignExpr value is not Expression");

    int lhs = varValue.value();
    int rhs = execute(expr);
    int result = execute(lhs, rhs, combinedAssignExpr->get_op());
    
    nametable_.set_value(name, result);

    LOGINFO("paracl: interpreter: execute combined assign expression: \"{}\" = {}", name, result);

    return result;
}

int Interpreter::execute(int lhs, int rhs, binary_op_t binary_operator)
{
    switch (binary_operator)
    {
    case binary_op_t::ADD:
        return lhs + rhs;
    case binary_op_t::SUB:
        return lhs - rhs;
    case binary_op_t::MUL:
        return lhs * rhs;
    case binary_op_t::DIV:
        return lhs / rhs;
    case binary_op_t::REM:
        return lhs % rhs;
    case binary_op_t::ISAB:
        return lhs > rhs;
    case binary_op_t::ISABE:
        return lhs >= rhs;
    case binary_op_t::ISLS:
        return lhs < rhs;
    case binary_op_t::ISLSE:
        return lhs <= rhs;
    case binary_op_t::ISEQ:
        return lhs == rhs;
    case binary_op_t::ISNE:
        return lhs != rhs;
    case binary_op_t::AND:
        return lhs && rhs;
    case binary_op_t::OR:
        return lhs || rhs;
    default:
        builtin_unreachable_wrapper("here we parse only binary operation");
    }
    builtin_unreachable_wrapper("we must return in switch");
}

int Interpreter::execute(int rhs, unary_op_t unary_operator)
{
    switch (unary_operator)
    {
    case unary_op_t::PLUS:
        return +rhs;
    case unary_op_t::MINUS:
        return -rhs;
    case unary_op_t::NOT:
        return !rhs;
    default:
        builtin_unreachable_wrapper("here we parse only unary operation");
    }
    builtin_unreachable_wrapper("we must return in switch");
}

int Interpreter::execute(int lhs, int value, combined_assign_t combined_assign)
{
    switch (combined_assign)
    {
    case combined_assign_t::ADDASGN:
        lhs += value;
        break;
    case combined_assign_t::SUBASGN:
        lhs -= value;
        break;
    case combined_assign_t::MULASGN:
        lhs *= value;
        break;
    case combined_assign_t::DIVASGN:
        lhs /= value;
        break;
    case combined_assign_t::REMASGN:
        lhs %= value;
        break;
    default:
        builtin_unreachable_wrapper("here we parse only combined assign operations");
    }
    return lhs;
}

void Interpreter::execute(const AssignStmt *assign)
{
    const Expression* e = assign->get_value();
    msg_assert(e, "AssignStmt value is not Expression");

    int result = execute(e);
    const std::string& name = assign->get_name();
    
    nametable_.set_value(name, result);

    LOGINFO("paracl: interpreter: execute assign statement: \"{}\" = {}", name, result);
}

void Interpreter::execute(const CombinedAssignStmt *combined_assign_statement)
{
    const std::string& name = combined_assign_statement->get_name();
    std::optional<int> varValue = nametable_.get_variable_value(name);
    
    if (not varValue.has_value())
        throw std::runtime_error("error: '" + name +
                                 "' was not declared in this scope\n"
                                 "paracl: failed with exit code 1");

    const Expression* expr = combined_assign_statement->get_value();
    msg_assert(expr, "CombinedAssignStmt value is not Expression");

    int lhs = varValue.value();
    int rhs = execute(expr);
    int result = execute(lhs, rhs, combined_assign_statement->get_op());
    
    nametable_.set_value(name, result);

    LOGINFO("paracl: interpreter: execute combined assign statement: \"{}\" = {}", name, result);
}

void Interpreter::execute(const PrintStmt *print_statement)
{
    LOGINFO("paracl: interpreter: execute print statement");

    size_t argCount = print_statement->get_arg_count();
    for (size_t i = 0; i < argCount; ++i)
    {
        const Expression* arg = print_statement->get_arg(i);
        if (!arg) continue;
        
        if (auto string = dynamic_cast<const StringConstant *>(arg))
        {
            std::cout << string->get_value() << std::flush;
            continue;
        }
        
        if (auto expr = dynamic_cast<const Expression *>(arg))
        {
            int result = execute(expr);
            std::cout << result << std::flush;
            continue;
        }
        
        builtin_unreachable_wrapper("bad 'print_statement' args type");
    }

    std::cout << std::endl;
}

} /* namespace ParaCL */