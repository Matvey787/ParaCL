module;

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <unordered_map>
#include <vector>

#include "global/global.hpp"
#include "parser/parser.hpp"

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
void execute_combined_assign(int &rhs, int value, token_t combined_assign);
int execute_unary_operation(int rhs, token_t unary_operator);

export void interpret(const ProgramAST &progAST)
{
    NameTable table;

    table.enter(); // global scope

    for (auto &stmt : progAST.statements)
        execute_statement(stmt.get(), table);

    table.leave();
}

void execute_statement(const Statement *stmt, NameTable &table)
{
    if (auto assign = dynamic_cast<const AssignStmt *>(stmt))
        return execute_assign_statement(assign, table);

    else if (auto combined_assign_statement = dynamic_cast<const CombinedAssingStmt *>(stmt))
        return execute_combined_assign_statement(combined_assign_statement, table);

    else if (auto print_statement = dynamic_cast<const PrintStmt *>(stmt))
        return execute_print_statement(print_statement, table);

    else if (auto while_stmt = dynamic_cast<const WhileStmt *>(stmt))
        return execute_while_statement(while_stmt, table);

    else if (auto block = dynamic_cast<const BlockStmt *>(stmt))
        return execute_block_statement(block, table);

    else if (auto condition = dynamic_cast<const ConditionStatement *>(stmt))
        return execute_condition_statement(condition, table);

    builtin_unreachable_wrapper("we must return in some else-if");
}

int execute_expression(const Expression *expr, NameTable &table)
{
    if (auto bin = dynamic_cast<const BinExpr *>(expr))
        return execute_binary_expression(bin, table);

    else if (auto un = dynamic_cast<const UnExpr *>(expr))
        return execute_unary_expression(un, table);

    else if (auto num = dynamic_cast<const NumExpr *>(expr))
        return execute_number_expression(num, table);

    else if (auto var = dynamic_cast<const VarExpr *>(expr))
        return execute_variable_expression(var, table);

    else if (auto in = dynamic_cast<const InputExpr *>(expr))
        return execute_input_expression(in, table);

    else if (auto assignExpr = dynamic_cast<const AssignExpr *>(expr))
        return execute_assign_expression(assignExpr, table);

    else if (auto combinedAssingExpr = dynamic_cast<const CombinedAssingExpr *>(expr))
        return execute_combined_assign_expression(combinedAssingExpr, table);

    builtin_unreachable_wrapper("we must return in some else-if");
}

void execute_while_statement(const WhileStmt *while_stmt, NameTable &table)
{
    const std::vector<std::unique_ptr<Statement>> &bodyStmts = while_stmt->body->statements;

    while (execute_expression(while_stmt->condition.get(), table))
        for (auto &s : bodyStmts)
            execute_statement(s.get(), table);
}

void execute_block_statement(const BlockStmt *block, NameTable &table)
{
    const std::vector<std::unique_ptr<Statement>> &blockStmts = block->statements;

    bool blockIsEmpty = blockStmts.empty();

    if (!blockIsEmpty)
        table.enter();

    for (auto &s : block->statements)
        execute_statement(s.get(), table);

    if (!blockIsEmpty)
        table.leave();
}

void execute_condition_statement(const ConditionStatement *condition, NameTable &table)
{
    auto if_stmt = condition->if_stmt.get();
    msg_assert(if_stmt, "in condition we always expect IF");

    bool flag = execute_expression(if_stmt->condition.get(), table);
    if (flag)
        return execute_statement(if_stmt->body.get(), table);

    for (auto &elif_statement : condition->elif_stmts)
    {
        flag = execute_expression(elif_statement->condition.get(), table);
        if (not flag)
            continue;
        return execute_statement(elif_statement->body.get(), table);
    }

    auto else_stmt = condition->else_stmt.get();
    if (not else_stmt)
        return; /* no ELSE */

    return execute_statement(else_stmt->body.get(), table);
}

int execute_binary_expression(const BinExpr *bin, NameTable &table)
{
    auto leftExpr = dynamic_cast<const Expression *>(bin->left.get());
    auto rightExpr = dynamic_cast<const Expression *>(bin->right.get());

    if (!leftExpr || !rightExpr)
        throw std::runtime_error("BinExpr children are not Expression");

    int leftResult = execute_expression(leftExpr, table);
    int rightResult = execute_expression(rightExpr, table);

    return execute_binary_operation(leftResult, rightResult, bin->op);
}

int execute_unary_expression(const UnExpr *un, NameTable &table)
{
    auto child_expr = dynamic_cast<const Expression *>(un->operand.get());
    int child_value = execute_expression(child_expr, table);
    return execute_unary_operation(child_value, un->op);
}

int execute_number_expression(const NumExpr *num, [[maybe_unused]] NameTable &table)
{
    return num->value;
}

int execute_variable_expression(const VarExpr *var, NameTable &table)
{
    auto *varValue = table.lookup(var->name);
    if (not varValue)
        throw std::runtime_error("'" + var->name + "' was not declared in this scope\n");

    return varValue->value;
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
    if (!e)
        throw std::runtime_error("BinExpr children are not Expression");

    auto result = execute_expression(e, table);

    table.declare(assignExpr->name, result);

    return result;
}

int execute_combined_assign_expression(const CombinedAssingExpr *combinedAssingExpr, NameTable &table)
{
    auto *varValue = table.lookup(combinedAssingExpr->name);
    if (!varValue)
    {
        std::cerr << "error: '" << combinedAssingExpr->name << "' was not declared in this scope\n"
                  << "paracl: failed with exit code 1";
        exit(EXIT_FAILURE);
    }

    auto e = dynamic_cast<const Expression *>(combinedAssingExpr->value.get());
    if (!e)
        throw std::runtime_error("BinExpr children are not Expression");

    auto result = execute_expression(e, table);

    execute_combined_assign(varValue->value, result, combinedAssingExpr->op);

    return varValue->value;
}

// Existing functions (остаются без изменений)
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
        return !rhs;
    default:
        builtin_unreachable_wrapper("here we parse only unary operation");
    }
    builtin_unreachable_wrapper("we must return in switch");
}

void execute_combined_assign(int &rhs, int value, token_t combined_assign)
{
    switch (combined_assign)
    {
    case token_t::ADDASGN:
        rhs += value;
        return;
    case token_t::SUBASGN:
        rhs -= value;
        return;
    case token_t::MULASGN:
        rhs *= value;
        return;
    case token_t::DIVASGN:
        rhs /= value;
        return;
    case token_t::REMASGN:
        rhs %= value;
        return;
    default:
        builtin_unreachable_wrapper("here we parse only combined assign operations");
    }
    builtin_unreachable_wrapper("we must return in switch");
}

void execute_assign_statement(const AssignStmt *assign, NameTable &table)
{
    auto e = dynamic_cast<const Expression *>(assign->value.get());
    if (!e)
        throw std::runtime_error("BinExpr children are not Expression");

    auto result = execute_expression(e, table);

    table.declare(assign->name, result);
}

void execute_combined_assign_statement(const CombinedAssingStmt *combined_assign_statement, NameTable &table)
{
    auto *varValue = table.lookup(combined_assign_statement->name);
    if (!varValue)
        throw std::runtime_error("error: '" + combined_assign_statement->name +
                                 "' was not declared in this scope\n"
                                 "paracl: failed with exit code 1");

    auto e = dynamic_cast<const Expression *>(combined_assign_statement->value.get());
    if (!e)
        throw std::runtime_error("BinExpr children are not Expression");

    auto result = execute_expression(e, table);

    execute_combined_assign(varValue->value, result, combined_assign_statement->op);
}

void execute_print_statement(const PrintStmt *print_statement, NameTable &table)
{
    for (auto &arg : print_statement->args)
    {
        if (auto string = dynamic_cast<const StringConstant *>(arg.get()))
        {
            std::cout << string->value << std::flush;
            continue;
        }
        else if (auto expr = dynamic_cast<const Expression *>(arg.get()))
        {
            auto result = execute_expression(expr, table);
            std::cout << result << std::flush;
            continue;
        }

        throw std::runtime_error("bad 'print_statement' args type");
    }

    std::cout << std::endl;
    return;
}

} /* namespace ParaCL */