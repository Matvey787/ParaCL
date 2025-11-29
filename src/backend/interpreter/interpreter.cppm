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

export class Interpreter
{
private:
    const ProgramAST& ast_;
    NameTable nametable_;

    /* Statement execution functions */
    void execute_statement(const Statement *stmt);
    void execute_assign_statement(const AssignStmt *assign);
    void execute_combined_assign_statement(const CombinedAssingStmt *combined_assign_statement);
    void execute_print_statement(const PrintStmt *print_statement);
    void execute_while_statement(const WhileStmt *while_stmt);
    void execute_block_statement(const BlockStmt *block);
    void execute_condition_statement(const ConditionStatement *condition);

    /* Expression execution functions */
    int execute_expression(const Expression *expr);
    int execute_binary_expression(const BinExpr *bin);
    int execute_unary_expression(const UnExpr *un);
    int execute_number_expression(const NumExpr *num);
    int execute_variable_expression(const VarExpr *var);
    int execute_input_expression([[maybe_unused]] const InputExpr *in);
    int execute_assign_expression(const AssignExpr *assignExpr);
    int execute_combined_assign_expression(const CombinedAssingExpr *combinedAssingExpr);

    int execute_binary_operation(int lhs, int rhs, token_t binary_operator);
    int execute_combined_assign(int rhs, int value, token_t combined_assign);
    int execute_unary_operation(int rhs, token_t unary_operator);

public:

    Interpreter(const ProgramAST& ast);
   ~Interpreter();

    Interpreter(const Interpreter&) = delete; 
    Interpreter& operator = (const Interpreter&) = delete; 
    Interpreter(Interpreter&&) = delete; 
    Interpreter& operator = (Interpreter&&) = delete; 

    void interpret();

};

Interpreter::Interpreter(const ProgramAST& ast) :
ast_(ast),
nametable_()
{
    LOGINFO("paracl: interpreter: ctor");

    /* create global scope */
    nametable_.new_scope();
}

Interpreter::~Interpreter()
{
    LOGINFO("paracl: interpreter: dtor");

    /* leave global scope */
    nametable_.leave_scope();
}

void Interpreter::interpret()
{
    LOGINFO("paracl: interpreter: start interpret");

    for (const auto &stmt : ast_.statements)
        execute_statement(stmt.get());

    LOGINFO("paracl: interpreter: interpret completed");
}

void Interpreter::execute_statement(const Statement *stmt)
{
    if (auto assign = dynamic_cast<const AssignStmt *>(stmt))
        return execute_assign_statement(assign);

    if (auto combined_assign_statement = dynamic_cast<const CombinedAssingStmt *>(stmt))
        return execute_combined_assign_statement(combined_assign_statement);

    if (auto print_statement = dynamic_cast<const PrintStmt *>(stmt))
        return execute_print_statement(print_statement);

    if (auto while_stmt = dynamic_cast<const WhileStmt *>(stmt))
        return execute_while_statement(while_stmt);

    if (auto block = dynamic_cast<const BlockStmt *>(stmt))
        return execute_block_statement(block);

    if (auto condition = dynamic_cast<const ConditionStatement *>(stmt))
        return execute_condition_statement(condition);

    builtin_unreachable_wrapper("we must return in some else-if");
}

int Interpreter::execute_expression(const Expression *expr)
{
    if (auto bin = dynamic_cast<const BinExpr *>(expr))
        return execute_binary_expression(bin);

    if (auto un = dynamic_cast<const UnExpr *>(expr))
        return execute_unary_expression(un);

    if (auto num = dynamic_cast<const NumExpr *>(expr))
        return execute_number_expression(num);

    if (auto var = dynamic_cast<const VarExpr *>(expr))
        return execute_variable_expression(var);

    if (auto in = dynamic_cast<const InputExpr *>(expr))
        return execute_input_expression(in);

    if (auto assignExpr = dynamic_cast<const AssignExpr *>(expr))
        return execute_assign_expression(assignExpr);

    if (auto combinedAssingExpr = dynamic_cast<const CombinedAssingExpr *>(expr))
        return execute_combined_assign_expression(combinedAssingExpr);

    builtin_unreachable_wrapper("we must return in some else-if");
}

void Interpreter::execute_while_statement(const WhileStmt *while_stmt)
{
    LOGINFO("paracl: interpreter: execute WHILE statement");

    const std::vector<std::unique_ptr<Statement>> &bodyStmts = while_stmt->body->statements;

    while (execute_expression(while_stmt->condition.get()))
    {
        for (auto &s : bodyStmts)
            execute_statement(s.get());
    }

    LOGINFO("paracl: interpreter: leave_scope WHILE statement");
}

void Interpreter::execute_block_statement(const BlockStmt *block)
{
    LOGINFO("paracl: interpreter: execute block statement");

    const std::vector<std::unique_ptr<Statement>> &blockStmts = block->statements;

    bool blockIsEmpty = blockStmts.empty();

    if (not blockIsEmpty)
        nametable_.new_scope();

    for (auto &s : block->statements)
        execute_statement(s.get());

    if (not blockIsEmpty)
        nametable_.leave_scope();
}

void Interpreter::execute_condition_statement(const ConditionStatement *condition)
{
    LOGINFO("paracl: interpreter: execute condition statement");

    auto if_stmt = condition->if_stmt.get();
    msg_assert(if_stmt, "in condition we always expect IF");

    bool flag = execute_expression(if_stmt->condition.get());
    LOGINFO("paracl: interpreter: condition in IF: {}", flag ? "true" : "false");
    if (flag)
        return execute_statement(if_stmt->body.get());

    for (auto &elif_statement : condition->elif_stmts)
    {
        flag = execute_expression(elif_statement->condition.get());
        LOGINFO("paracl: interpreter: condition in ELSE-IF: {}", flag ? "true" : "false");

        if (not flag)
            continue;

        return execute_statement(elif_statement->body.get());
    }

    LOGINFO("paracl: interpreter: no ELSE in condition");

    auto else_stmt = condition->else_stmt.get();
    if (not else_stmt)
        return; /* no ELSE */

    LOGINFO("paracl: interpreter: condition ELSE");

    return execute_statement(else_stmt->body.get());
}

int Interpreter::execute_binary_expression(const BinExpr *bin)
{
    auto leftExpr = dynamic_cast<const Expression *>(bin->left.get());
    auto rightExpr = dynamic_cast<const Expression *>(bin->right.get());

    msg_assert(leftExpr, "left bin expr child is not expr");
    msg_assert(rightExpr, "rihgt bin expr child is not expr");

    int leftResult = execute_expression(leftExpr);
    int rightResult = execute_expression(rightExpr);
    int result = execute_binary_operation(leftResult, rightResult, bin->op);

    LOGINFO("paracl: interpreter: execute binary expresion: {}", result);

    return result;
}

int Interpreter::execute_unary_expression(const UnExpr *un)
{
    auto child_expr = dynamic_cast<const Expression *>(un->operand.get());
    int child_value = execute_expression(child_expr);
    int result = execute_unary_operation(child_value, un->op);
    LOGINFO("paracl: interpreter: execute unary expresion: {}", result);
    return result;
}

int Interpreter::execute_number_expression(const NumExpr *num)
{
    LOGINFO("paracl: interpreter: get number value: {}", num->value);
    return num->value;
}

int Interpreter::execute_variable_expression(const VarExpr *var)
{
    std::optional<NameValue> varValue = nametable_.get_variable_value(var->name);
    if (not varValue.has_value())
        throw std::runtime_error("'" + var->name + "' was not declared in this scope\n");

    LOGINFO("paracl: interpreter: get variable value: \"{}\" = {}", var->name, varValue.value().value());

    return varValue.value().value();
}

int Interpreter::execute_input_expression([[maybe_unused]] const InputExpr *in)
{
    int value = 0;
    std::cin >> value;
    return value;
}

int Interpreter::execute_assign_expression(const AssignExpr *assignExpr)
{
    auto e = dynamic_cast<const Expression *>(assignExpr->value.get());
    msg_assert(e, "BinExpr children are not Expression");

    auto result = execute_expression(e);

    NameValue new_value{result};
    nametable_.set_value(assignExpr->name, new_value);

    LOGINFO("paracl: interpreter: execute assign expression: \"{}\" = {}", assignExpr->name, result);

    return result;
}

int Interpreter::execute_combined_assign_expression(const CombinedAssingExpr *combinedAssingExpr)
{
    std::optional<NameValue> varValue = nametable_.get_variable_value(combinedAssingExpr->name);
    int value = varValue.value().value();
    if (not varValue.has_value())
        throw std::runtime_error("error: '" + combinedAssingExpr->name + "' was not declared in this scope\n");

    auto expr = dynamic_cast<const Expression *>(combinedAssingExpr->value.get());
    msg_assert(expr, "BinExpr children are not Expression");

    auto result = execute_expression(expr);

    value = execute_combined_assign(value, result, combinedAssingExpr->op);
    NameValue new_value{value};
    nametable_.set_value(combinedAssingExpr->name, new_value);

    LOGINFO("paracl: interpreter: execute combined assign expression: \"{}\" = {}", combinedAssingExpr->name, value);

    return value;
}

int Interpreter::execute_binary_operation(int lhs, int rhs, token_t binary_operator)
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

int Interpreter::execute_unary_operation(int rhs, token_t unary_operator)
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

int Interpreter::execute_combined_assign(int rhs, int value, token_t combined_assign)
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

void Interpreter::execute_assign_statement(const AssignStmt *assign)
{
    auto e = dynamic_cast<const Expression *>(assign->value.get());
    msg_assert(e, "BinExpr children are not Expression");

    int result = execute_expression(e);
    nametable_.set_value(assign->name, NameValue{result});

    LOGINFO("paracl: interpreter: execute assign statement: \"{}\" = {}", assign->name, result);
}

void Interpreter::execute_combined_assign_statement(const CombinedAssingStmt *combined_assign_statement)
{
    std::optional<NameValue> varValue = nametable_.get_variable_value(combined_assign_statement->name);
    int value = varValue.value().value();
    if (not varValue.has_value())
        throw std::runtime_error("error: '" + combined_assign_statement->name +
                                 "' was not declared in this scope\n"
                                 "paracl: failed with exit code 1");

    auto expr = dynamic_cast<const Expression *>(combined_assign_statement->value.get());
    msg_assert(expr, "BinExpr children are not Expression");

    auto result = execute_expression(expr);

    value = execute_combined_assign(value, result, combined_assign_statement->op);
    NameValue new_value{value};
    nametable_.set_value(combined_assign_statement->name, new_value);

    LOGINFO("paracl: interpreter: execute combined assign statement: \"{}\" = {}", combined_assign_statement->name,
            value);
}

void Interpreter::execute_print_statement(const PrintStmt *print_statement)
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
            auto result = execute_expression(expr);
            std::cout << result << std::flush;
            continue;
        }
        builtin_unreachable_wrapper("bad 'print_statement' args type");
    }

    std::cout << std::endl;
    return;
}

} /* namespace ParaCL */