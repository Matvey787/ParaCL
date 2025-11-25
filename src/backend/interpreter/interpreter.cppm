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

import name_table;

namespace ParaCL
{

/* Statement execution functions */
void execute_statement(const Statement *stmt, NameTable &table);
void execute_assign_statement(const AssignStmt *assign, NameTable &table);
void execute_combined_assign_statement(const CombinedAssingStmt *combined_assign_statement, NameTable &table);
void execute_print_statement(const PrintStmt *print_statement, NameTable &table);
void execute_while_statement(const WhileStmt *while_stmt, NameTable &table);
void execute_block_statement(const BlockStmt *block, NameTable &table);
void execute_condition_statement(const ConditionStatement *condition, NameTable &table);

/* Expression execution functions */
int execute_expression(const Expression *expr, NameTable &table);
int execute_binary_expression(const BinExpr *bin, NameTable &table);
int execute_unary_expression(const UnExpr *un, NameTable &table);
int execute_number_expression(const NumExpr *num, [[maybe_unused]] NameTable &table);
int execute_variable_expression(const VarExpr *var, NameTable &table);
int execute_input_expression([[maybe_unused]] const InputExpr *in, [[maybe_unused]] NameTable &table);
int execute_assign_expression(const AssignExpr *assignExpr, NameTable &table);
int execute_combined_assign_expression(const CombinedAssingExpr *combinedAssingExpr, NameTable &table);

int execute_binary_operation(int lhs, int rhs, token_t binary_operator);
int execute_combined_assign(int rhs, int value, token_t combined_assign);
int execute_unary_operation(int rhs, token_t unary_operator);

export void interpret(const ProgramAST &progAST)
{
    LOGINFO("paracl: interpreter: start iterpret");

    NameTable table;

    table.enter(); // global scope

    for (auto &stmt : progAST.statements)
        execute_statement(stmt.get(), table);

    table.leave();

    LOGINFO("paracl: interpreter: interpret completed");
}

void execute_statement(const Statement *stmt, NameTable &table)
{
    if (auto assign = dynamic_cast<const AssignStmt *>(stmt))
        return execute_assign_statement(assign, table);

    if (auto combined_assign_statement = dynamic_cast<const CombinedAssingStmt *>(stmt))
        return execute_combined_assign_statement(combined_assign_statement, table);

    if (auto print_statement = dynamic_cast<const PrintStmt *>(stmt))
        return execute_print_statement(print_statement, table);

    if (auto while_stmt = dynamic_cast<const WhileStmt *>(stmt))
        return execute_while_statement(while_stmt, table);

    if (auto block = dynamic_cast<const BlockStmt *>(stmt))
        return execute_block_statement(block, table);

    if (auto condition = dynamic_cast<const ConditionStatement *>(stmt))
        return execute_condition_statement(condition, table);

    builtin_unreachable_wrapper("we must return in some else-if");
}

int execute_expression(const Expression *expr, NameTable &table)
{
    if (auto bin = dynamic_cast<const BinExpr *>(expr))
        return execute_binary_expression(bin, table);

    if (auto un = dynamic_cast<const UnExpr *>(expr))
        return execute_unary_expression(un, table);

    if (auto num = dynamic_cast<const NumExpr *>(expr))
        return execute_number_expression(num, table);

    if (auto var = dynamic_cast<const VarExpr *>(expr))
        return execute_variable_expression(var, table);

    if (auto in = dynamic_cast<const InputExpr *>(expr))
        return execute_input_expression(in, table);

    if (auto assignExpr = dynamic_cast<const AssignExpr *>(expr))
        return execute_assign_expression(assignExpr, table);

    if (auto combinedAssingExpr = dynamic_cast<const CombinedAssingExpr *>(expr))
        return execute_combined_assign_expression(combinedAssingExpr, table);

    builtin_unreachable_wrapper("we must return in some else-if");
}

void execute_while_statement(const WhileStmt *while_stmt, NameTable &table)
{
    LOGINFO("paracl: interpreter: execute WHILE statement");

    const std::vector<std::unique_ptr<Statement>> &bodyStmts = while_stmt->body->statements;

    while (execute_expression(while_stmt->condition.get(), table))
    {
        for (auto &s : bodyStmts)
            execute_statement(s.get(), table);
    }

    LOGINFO("paracl: interpreter: leave WHILE statement");
}

void execute_block_statement(const BlockStmt *block, NameTable &table)
{
    LOGINFO("paracl: interpreter: execute block statement");

    const std::vector<std::unique_ptr<Statement>> &blockStmts = block->statements;

    bool blockIsEmpty = blockStmts.empty();

    if (not blockIsEmpty)
        table.enter();

    for (auto &s : block->statements)
        execute_statement(s.get(), table);

    if (not blockIsEmpty)
        table.leave();
}

void execute_condition_statement(const ConditionStatement *condition, NameTable &table)
{
    LOGINFO("paracl: interpreter: execute condition statement");

    auto if_stmt = condition->if_stmt.get();
    msg_assert(if_stmt, "in condition we always expect IF");

    bool flag = execute_expression(if_stmt->condition.get(), table);
    LOGINFO("paracl: interpreter: condition in IF: {}", flag ? "true" : "false");
    if (flag)
        return execute_statement(if_stmt->body.get(), table);

    for (auto &elif_statement : condition->elif_stmts)
    {
        flag = execute_expression(elif_statement->condition.get(), table);
        LOGINFO("paracl: interpreter: condition in ELSE-IF: {}", flag ? "true" : "false");

        if (not flag)
            continue;

        return execute_statement(elif_statement->body.get(), table);
    }

    LOGINFO("paracl: interpreter: no ELSE in condition");

    auto else_stmt = condition->else_stmt.get();
    if (not else_stmt)
        return; /* no ELSE */

    LOGINFO("paracl: interpreter: condition ELSE");

    return execute_statement(else_stmt->body.get(), table);
}

int execute_binary_expression(const BinExpr *bin, NameTable &table)
{
    auto leftExpr = dynamic_cast<const Expression *>(bin->left.get());
    auto rightExpr = dynamic_cast<const Expression *>(bin->right.get());

    msg_assert(leftExpr, "left bin expr child is not expr");
    msg_assert(rightExpr, "rihgt bin expr child is not expr");

    int leftResult = execute_expression(leftExpr, table);
    int rightResult = execute_expression(rightExpr, table);
    int result = execute_binary_operation(leftResult, rightResult, bin->op);

    LOGINFO("paracl: interpreter: execute binary expresion: {}", result);

    return result;
}

int execute_unary_expression(const UnExpr *un, NameTable &table)
{
    auto child_expr = dynamic_cast<const Expression *>(un->operand.get());
    int child_value = execute_expression(child_expr, table);
    int result = execute_unary_operation(child_value, un->op);
    LOGINFO("paracl: interpreter: execute unary expresion: {}", result);
    return result;
}

int execute_number_expression(const NumExpr *num, [[maybe_unused]] NameTable &table)
{
    LOGINFO("paracl: interpreter: get number value: {}", num->value);
    return num->value;
}

int execute_variable_expression(const VarExpr *var, NameTable &table)
{
    std::optional<NameValue> varValue = table.get_variable_value(var->name);
    if (not varValue.has_value())
        throw std::runtime_error("'" + var->name + "' was not declared in this scope\n");

    LOGINFO("paracl: interpreter: get variable value: \"{}\" = {}", var->name, varValue.value().value);

    return varValue.value().value;
}

int execute_input_expression([[maybe_unused]] const InputExpr *in, [[maybe_unused]] NameTable &table)
{
    int value = 0;
    std::cin >> value;
    return value;
}

int execute_assign_expression(const AssignExpr *assignExpr, NameTable &table)
{
    auto e = dynamic_cast<const Expression *>(assignExpr->value.get());
    msg_assert(e, "BinExpr children are not Expression");

    auto result = execute_expression(e, table);

    NameValue new_value{result};
    table.set_value(assignExpr->name, new_value);

    LOGINFO("paracl: interpreter: execute assign expression: \"{}\" = {}", assignExpr->name, result);

    return result;
}

int execute_combined_assign_expression(const CombinedAssingExpr *combinedAssingExpr, NameTable &table)
{
    std::optional<NameValue> varValue = table.get_variable_value(combinedAssingExpr->name);
    int value = varValue.value().value;
    if (not varValue.has_value())
        throw std::runtime_error("error: '" + combinedAssingExpr->name + "' was not declared in this scope\n");

    auto expr = dynamic_cast<const Expression *>(combinedAssingExpr->value.get());
    msg_assert(expr, "BinExpr children are not Expression");

    auto result = execute_expression(expr, table);

    value = execute_combined_assign(value, result, combinedAssingExpr->op);
    NameValue new_value{value};
    table.set_value(combinedAssingExpr->name, new_value);

    LOGINFO("paracl: interpreter: execute combined assign expression: \"{}\" = {}", combinedAssingExpr->name, value);

    return value;
}

int execute_binary_operation(int lhs, int rhs, token_t binary_operator)
{
    switch (binary_operator)
    {
    case token_t::ADD:
        return lhs + rhs;
    case token_t::SUB:
        return lhs - rhs;
    case token_t::MUL:
        return lhs * rhs;
    case token_t::DIV:
        return lhs / rhs;
    case token_t::REM:
        return lhs % rhs;
    case token_t::ISAB:
        return lhs > rhs;
    case token_t::ISABE:
        return lhs >= rhs;
    case token_t::ISLS:
        return lhs < rhs;
    case token_t::ISLSE:
        return lhs <= rhs;
    case token_t::ISEQ:
        return lhs == rhs;
    case token_t::ISNE:
        return lhs != rhs;
    case token_t::AND:
        return lhs && rhs;
    case token_t::OR:
        return lhs || rhs;
    default:
        builtin_unreachable_wrapper("here we parse only binary operation");
    }
    builtin_unreachable_wrapper("we must return in switch");
}

int execute_unary_operation(int rhs, token_t unary_operator)
{
    switch (unary_operator)
    {
    case token_t::ADD:
        return +rhs;
    case token_t::SUB:
        return -rhs;
    case token_t::NOT:
        return not rhs;
    default:
        builtin_unreachable_wrapper("here we parse only unary operation");
    }
    builtin_unreachable_wrapper("we must return in switch");
}

int execute_combined_assign(int rhs, int value, token_t combined_assign)
{
    switch (combined_assign)
    {
    case token_t::ADDASGN:
        rhs += value;
        break;
    case token_t::SUBASGN:
        rhs -= value;
        break;
    case token_t::MULASGN:
        rhs *= value;
        break;
    case token_t::DIVASGN:
        rhs /= value;
        break;
    case token_t::REMASGN:
        rhs %= value;
        break;
    default:
        builtin_unreachable_wrapper("here we parse only combined assign operations");
    }
    return rhs;
}

void execute_assign_statement(const AssignStmt *assign, NameTable &table)
{
    auto e = dynamic_cast<const Expression *>(assign->value.get());
    msg_assert(e, "BinExpr children are not Expression");

    int result = execute_expression(e, table);
    table.set_value(assign->name, NameValue{result});

    LOGINFO("paracl: interpreter: execute assign statement: \"{}\" = {}", assign->name, result);
}

void execute_combined_assign_statement(const CombinedAssingStmt *combined_assign_statement, NameTable &table)
{
    std::optional<NameValue> varValue = table.get_variable_value(combined_assign_statement->name);
    int value = varValue.value().value;
    if (not varValue.has_value())
        throw std::runtime_error("error: '" + combined_assign_statement->name +
                                 "' was not declared in this scope\n"
                                 "paracl: failed with exit code 1");

    auto expr = dynamic_cast<const Expression *>(combined_assign_statement->value.get());
    msg_assert(expr, "BinExpr children are not Expression");

    auto result = execute_expression(expr, table);

    value = execute_combined_assign(value, result, combined_assign_statement->op);
    NameValue new_value{value};
    table.set_value(combined_assign_statement->name, new_value);

    LOGINFO("paracl: interpreter: execute combined assign statement: \"{}\" = {}", combined_assign_statement->name,
            value);
}

void execute_print_statement(const PrintStmt *print_statement, NameTable &table)
{
    LOGINFO("paracl: interpreter: execute print statement");

    for (auto &arg : print_statement->args)
    {
        if (auto string = dynamic_cast<const StringConstant *>(arg.get()))
        {
            std::cout << string->value << std::flush;
            continue;
        }
        if (auto expr = dynamic_cast<const Expression *>(arg.get()))
        {
            auto result = execute_expression(expr, table);
            std::cout << result << std::flush;
            continue;
        }
        builtin_unreachable_wrapper("bad 'print_statement' args type");
    }

    std::cout << std::endl;
    return;
}

} /* namespace ParaCL */