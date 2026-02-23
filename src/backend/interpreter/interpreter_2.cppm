module;

#include <cstdlib>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

#include "global/global.hpp"
#include "parser/parser.hpp"

#include "log/log_api.hpp"

export module interpreter;

import interpreter_name_table;
export import interpreter_optoins;

import ast_builder;

namespace ParaCL
{
namespace backend
{
namespace interpreter
{

using ParaCL::options::interpreter::InterpreterOptions;

export template <typename Interpreter>
concept IInterpreter =
    std::is_consructible<Interpreter, const InterpreterOptions &> && requires(Interpreter interpreter) {
        { interpreter.interpret() } -> std::same_as<void>;
    };

class basic_node;

namespace execute_overload_set
{
void execute(const basic_node *, [[maybe_unused]] int &return_value)
{
}
} /* namespace execute_overload_set */


#define SPECIALIZE_VISIT(NodeT) \
template <> void visit(ast::AST<void, nametable::INametable>::NodeT node, nametable::INametable nametable_, [[maybe_unused]] int& return_value)

SPECIALIZE_VISIT(Print)
{
    LOGINFO("paracl: interpreter: execute print statement");

    for (auto const &arg : node.args())
    {
        visit(arg, nametable_, return_value);
        std::cout << return_value;
    }

    std::cout << std::endl;
    return;   
}



export template <nametable::INametable Nametable = nametable::Nametable,
                 frontend::ast_builder::IASTBuilder ASTBuilder = frontend::ast_builder::ASTBuilder>
class Interpreter final
{
  private:
    Nametable nametable_;

  public:
    Interpreter(const InterpreterOptions &opt);

    void interpret();
};

Interpreter<Nametable, ASTBuilder>::Interpreter(const InterpreterOptions &opt)
{
    LOGINFO("paracl: interpreter: ctor");

    if (opt.sources.size() == 0)
        throw std::invalid_argument("no input files given to interpreter");

    if (opt.sources.size() > 1)
        throw std::invalid_argument("now interpreter supported only one file programs");

    const std::filsystem::path &source = opt.sources[0];

    ast_ = ASTBUilder{source}.ast();

    /* create global scope */
    nametable_.new_scope();
}

void Interpreter<Nametable, ASTBuilder>::interpret()
{
    LOGINFO("paracl: interpreter: start interpret");

    for (const auto &stmt : ast_.statements)
        execute(stmt.get());

    ast::visit(ast_.root, nametable_);

    LOGINFO("paracl: interpreter: interpret completed");
}

void Interpreter<Nametable, ASTBuilder>::execute(const Statement *stmt)
{
    if (auto assign = static_cast<const AssignStmt *>(stmt))
        return execute(assign);

    if (auto combined_assign_statement = static_cast<const CombinedAssingStmt *>(stmt))
        return execute(combined_assign_statement);

    if (auto print_statement = static_cast<const PrintStmt *>(stmt))
        return execute(print_statement);

    if (auto while_stmt = static_cast<const WhileStmt *>(stmt))
        return execute(while_stmt);

    if (auto block = static_cast<const BlockStmt *>(stmt))
        return execute(block);

    if (auto condition = static_cast<const ConditionStatement *>(stmt))
        return execute(condition);

    builtin_unreachable_wrapper("we must return in some else-if");
}

int Interpreter<Nametable, ASTBuilder>::execute(const Expression *expr)
{
    if (auto bin = static_cast<const BinExpr *>(expr))
        return execute(bin);

    if (auto un = static_cast<const UnExpr *>(expr))
        return execute(un);

    if (auto num = static_cast<const NumExpr *>(expr))
        return execute(num);

    if (auto var = static_cast<const VarExpr *>(expr))
        return execute(var);

    if (auto in = static_cast<const InputExpr *>(expr))
        return execute(in);

    if (auto assignExpr = static_cast<const AssignExpr *>(expr))
        return execute(assignExpr);

    if (auto combinedAssingExpr = static_cast<const CombinedAssingExpr *>(expr))
        return execute(combinedAssingExpr);

    builtin_unreachable_wrapper("we must return in some else-if");
}

void Interpreter<Nametable, ASTBuilder>::execute(const WhileStmt *while_stmt)
{
    LOGINFO("paracl: interpreter: execute WHILE statement");

    const std::vector<std::unique_ptr<Statement>> &bodyStmts = while_stmt->body->statements;

    while (execute(while_stmt->condition.get()))
    {
        for (auto &s : bodyStmts)
            execute(s.get());
    }

    LOGINFO("paracl: interpreter: leave_scope WHILE statement");
}

void Interpreter<Nametable, ASTBuilder>::execute(const BlockStmt *block)
{
    LOGINFO("paracl: interpreter: execute block statement");

    const std::vector<std::unique_ptr<Statement>> &blockStmts = block->statements;

    bool blockIsEmpty = blockStmts.empty();

    if (not blockIsEmpty)
        nametable_.new_scope();

    for (auto &s : block->statements)
        execute(s.get());

    if (not blockIsEmpty)
        nametable_.leave_scope();
}

void Interpreter<Nametable, ASTBuilder>::execute(const ConditionStatement *condition)
{
    LOGINFO("paracl: interpreter: execute condition statement");

    auto if_stmt = condition->if_stmt.get();
    msg_assert(if_stmt, "in condition we always expect IF");

    bool flag = execute(if_stmt->condition.get());
    LOGINFO("paracl: interpreter: condition in IF: {}", flag ? "true" : "false");
    if (flag)
        return execute(if_stmt->body.get());

    for (auto &elif_statement : condition->elif_stmts)
    {
        flag = execute(elif_statement->condition.get());
        LOGINFO("paracl: interpreter: condition in ELSE-IF: {}", flag ? "true" : "false");

        if (not flag)
            continue;

        return execute(elif_statement->body.get());
    }

    LOGINFO("paracl: interpreter: no ELSE in condition");

    auto *else_stmt = condition->else_stmt.get();

    if (not else_stmt)
        return; /* no ELSE */

    LOGINFO("paracl: interpreter: condition ELSE");

    return execute(else_stmt->body.get());
}

int Interpreter<Nametable, ASTBuilder>::execute(const BinExpr *bin)
{
    auto leftExpr = static_cast<const Expression *>(bin->left.get());
    auto rightExpr = static_cast<const Expression *>(bin->right.get());

    msg_assert(leftExpr, "left bin expr child is not expr");
    msg_assert(rightExpr, "rihgt bin expr child is not expr");

    int leftResult = execute(leftExpr);
    int rightResult = execute(rightExpr);
    int result = execute(leftResult, rightResult, bin->op());

    LOGINFO("paracl: interpreter: execute binary expresion: {}", result);

    return result;
}

int Interpreter<Nametable, ASTBuilder>::execute(const UnExpr *un)
{
    auto child_expr = static_cast<const Expression *>(un->operand.get());
    int child_value = execute(child_expr);
    int result = execute(child_value, un->op());
    LOGINFO("paracl: interpreter: execute unary expresion: {}", result);
    return result;
}

int Interpreter<Nametable, ASTBuilder>::execute(const NumExpr *num)
{
    LOGINFO("paracl: interpreter: get number value: {}", num->value);
    return num->value;
}

int Interpreter<Nametable, ASTBuilder>::execute(const VarExpr *var)
{
    std::optional<int> varValue = nametable_.get_variable_value(var->name);
    if (not varValue.has_value())
        throw std::runtime_error("'" + var->name + "' was not declared in this scope\n");

    LOGINFO("paracl: interpreter: get variable value: \"{}\" = {}", var->name, varValue.value());

    return varValue.value();
}

int Interpreter<Nametable, ASTBuilder>::execute([[maybe_unused]] const InputExpr *in)
{
    int value = 0;
    std::cin >> value;
    return value;
}

int Interpreter<Nametable, ASTBuilder>::execute(const AssignExpr *assignExpr)
{
    auto e = static_cast<const Expression *>(assignExpr->value.get());
    msg_assert(e, "BinExpr children are not Expression");

    int result = execute(e);

    nametable_.set_value(assignExpr->name, result);

    LOGINFO("paracl: interpreter: execute assign expression: \"{}\" = {}", assignExpr->name, result);

    return result;
}

int Interpreter<Nametable, ASTBuilder>::execute(const CombinedAssingExpr *combinedAssingExpr)
{
    std::optional<int> varValue = nametable_.get_variable_value(combinedAssingExpr->name);
    int value = varValue.value();
    if (not varValue.has_value())
        throw std::runtime_error("error: '" + combinedAssingExpr->name + "' was not declared in this scope\n");

    auto expr = static_cast<const Expression *>(combinedAssingExpr->value.get());
    msg_assert(expr, "BinExpr children are not Expression");

    int result = execute(expr);

    value = execute(value, result, combinedAssingExpr->op());
    nametable_.set_value(combinedAssingExpr->name, value);

    LOGINFO("paracl: interpreter: execute combined assign expression: \"{}\" = {}", combinedAssingExpr->name, value);

    return value;
}

int Interpreter<Nametable, ASTBuilder>::execute(int lhs, int rhs, binary_op_t binary_operator)
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

int Interpreter<Nametable, ASTBuilder>::execute(int rhs, unary_op_t unary_operator)
{
    switch (unary_operator)
    {
    case unary_op_t::PLUS:
        return +rhs;
    case unary_op_t::MINUS:
        return -rhs;
    case unary_op_t::NOT:
        return not rhs;
    default:
        builtin_unreachable_wrapper("here we parse only unary operation");
    }
    builtin_unreachable_wrapper("we must return in switch");
}

int Interpreter<Nametable, ASTBuilder>::execute(int rhs, int value, combined_assign_t combined_assign)
{
    switch (combined_assign)
    {
    case combined_assign_t::ADDASGN:
        rhs += value;
        break;
    case combined_assign_t::SUBASGN:
        rhs -= value;
        break;
    case combined_assign_t::MULASGN:
        rhs *= value;
        break;
    case combined_assign_t::DIVASGN:
        rhs /= value;
        break;
    case combined_assign_t::REMASGN:
        rhs %= value;
        break;
    default:
        builtin_unreachable_wrapper("here we parse only combined assign operations");
    }
    return rhs;
}

void Interpreter<Nametable, ASTBuilder>::execute(const AssignStmt *assign)
{
    auto e = static_cast<const Expression *>(assign->value.get());
    msg_assert(e, "BinExpr children are not Expression");

    int result = execute(e);
    nametable_.set_value(assign->name, result);

    LOGINFO("paracl: interpreter: execute assign statement: \"{}\" = {}", assign->name, result);
}

void Interpreter<Nametable, ASTBuilder>::execute(const CombinedAssingStmt *combined_assign_statement)
{
    std::optional<int> varValue = nametable_.get_variable_value(combined_assign_statement->name);
    int value = varValue.value();
    if (not varValue.has_value())
        throw std::runtime_error("error: '" + combined_assign_statement->name +
                                 "' was not declared in this scope\n"
                                 "paracl: failed with exit code 1");

    auto expr = static_cast<const Expression *>(combined_assign_statement->value.get());
    msg_assert(expr, "BinExpr children are not Expression");

    int result = execute(expr);

    value = execute(value, result, combined_assign_statement->op());
    nametable_.set_value(combined_assign_statement->name, value);

    LOGINFO("paracl: interpreter: execute combined assign statement: \"{}\" = {}", combined_assign_statement->name,
            value);
}

void Interpreter<Nametable, ASTBuilder>::execute(const PrintStmt *print_statement)
{
    LOGINFO("paracl: interpreter: execute print statement");

    for (auto &arg : print_statement->args)
    {
        if (auto string = static_cast<const StringConstant *>(arg.get()))
        {
            std::cout << string->value << std::flush;
            continue;
        }
        if (auto expr = static_cast<const Expression *>(arg.get()))
        {
            int result = execute(expr);
            std::cout << result << std::flush;
            continue;
        }
        builtin_unreachable_wrapper("bad 'print_statement' args type");
    }

    std::cout << std::endl;
    return;
}

} /* namespace interpreter */
} /* namespace backend */
} /* namespace ParaCL */
