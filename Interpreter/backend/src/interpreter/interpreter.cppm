module;

#include <cstdlib>
#include <iostream>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <ostream>
#include <filesystem>
#include <utility>

#include <boost/json.hpp>

#include "create-basic-node.hpp"

#define LOGINFO(...)
#define LOGERR(...)

export module interpreter;

import nametable;
import thelast;

//-----------------------------------------------------------------------------

namespace last::node
{

using executable_expression                     = int(interpreter::nametable::Nametable&);
using executable_statement                      = void(interpreter::nametable::Nametable&);
using printable_string                          = std::string_view();
using executable_if_with_return_codition_status = bool(interpreter::nametable::Nametable&);

static_assert(not std::is_same_v<executable_expression, executable_statement>                       , "visit specializations must be different");
static_assert(not std::is_same_v<executable_expression, printable_string>                           , "visit specializations must be different");
static_assert(not std::is_same_v<executable_statement, printable_string>                            , "visit specializations must be different");
static_assert(not std::is_same_v<executable_if_with_return_codition_status, executable_statement>   , "visit specializations must be different");
static_assert(not std::is_same_v<executable_if_with_return_codition_status, executable_expression>  , "visit specializations must be different");
static_assert(not std::is_same_v<executable_if_with_return_codition_status, printable_string>       , "visit specializations must be different");

} /* namespace last::node */

//-----------------------------------------------------------------------------

namespace last::node
{

decltype(auto) execute_expsession(BasicNode const & node, interpreter::nametable::Nametable& nametable)
{
    return visit<int, interpreter::nametable::Nametable&>(node, nametable);
}

decltype(auto) execute_statement(BasicNode const & node, interpreter::nametable::Nametable& nametable)
{
    return visit<void, interpreter::nametable::Nametable&>(node, nametable);
}

decltype(auto) print_string(BasicNode const & node)
{
    return visit<std::string_view>(node);
}

decltype(auto) execute_if_with_return_codition_status(BasicNode const & node, interpreter::nametable::Nametable& nametable)
{
    return visit<bool, interpreter::nametable::Nametable&>(node, nametable);
}

} /* namespace last::node */

//-----------------------------------------------------------------------------

namespace last::node::visit_specializations
{

//-----------------------------------------------------------------------------
// SCAN
//-----------------------------------------------------------------------------
template <>
int visit(Scan const& node, interpreter::nametable::Nametable& nametable)
{
    int value;
    std::cin >> value;
    LOGINFO("paracl: interpreter: scan value: {}", value);
    return value;
}

//-----------------------------------------------------------------------------

template <>
void visit(Scan const& node, interpreter::nametable::Nametable& nametable)
{
    (void) visit<Scan, int, interpreter::nametable::Nametable&>(node, nametable);
}

//-----------------------------------------------------------------------------
// VARIABLE
//-----------------------------------------------------------------------------
template <>
int visit(Variable const& node, interpreter::nametable::Nametable& nametable)
{
    auto&& var_name = node.name();
    auto&& value = nametable.get_variable_value(var_name);

    LOGINFO("paracl: interpreter: get variable '{}' = {}", var_name, value);
    return value;
}

template <>
void visit(Variable const& node, interpreter::nametable::Nametable& nametable)
{
    (void) visit<Variable, int, interpreter::nametable::Nametable&>(node, nametable);
}

//-----------------------------------------------------------------------------
// NUMBER LITERAL
//-----------------------------------------------------------------------------
template <>
int visit(NumberLiteral const& node, interpreter::nametable::Nametable& nametable)
{
    LOGINFO("paracl: interpreter: number literal: {}", node.value());
    return node.value();
}

template <>
void visit(NumberLiteral const& node, interpreter::nametable::Nametable& nametable)
{
    (void) visit<NumberLiteral, int, interpreter::nametable::Nametable&>(node, nametable);
}

//-----------------------------------------------------------------------------
// STRING LITERAL
//-----------------------------------------------------------------------------
template <>
std::string_view visit(StringLiteral const& node)
{
    LOGINFO("paracl: interpreter: string literal: \"{}\"", node.value());
    return node.value();
}

//-----------------------------------------------------------------------------
// UNARY OPERATOR
//-----------------------------------------------------------------------------
template <>
int visit(UnaryOperator const& node, interpreter::nametable::Nametable& nametable)
{
    auto&& arg_value = execute_expsession(node.arg(), nametable);
    
    switch (node.type())
    {
        case UnaryOperator::MINUS: return -arg_value;
        case UnaryOperator::PLUS:  return +arg_value;
        case UnaryOperator::NOT:   return !arg_value;
        default: __builtin_unreachable();
    }
}

template <>
void visit(UnaryOperator const& node, interpreter::nametable::Nametable& nametable)
{
    (void) visit<UnaryOperator, int, interpreter::nametable::Nametable&>(node, nametable);
}

//-----------------------------------------------------------------------------
// BINARY OPERATOR
//-----------------------------------------------------------------------------
template <>
int visit(BinaryOperator const& node, interpreter::nametable::Nametable& nametable)
{
    /* not geting left, cause it can be a not init variable */
    /* not geting right, cause left must be execute earlier */

    if (node.type() == BinaryOperator::ASGN)
    {
        auto&& variable = static_cast<Variable const &>(node.larg());
        auto&& right = execute_expsession(node.rarg(), nametable);
        nametable.set_value(variable.name(), right);
        return right;
    }

    auto&& left  = execute_expsession(node.larg(), nametable);
    auto&& right = execute_expsession(node.rarg(), nametable);

    switch (node.type())
    {
        case BinaryOperator::AND:     return left && right;
        case BinaryOperator::OR:      return left || right;
        case BinaryOperator::ADD:     return left +  right;
        case BinaryOperator::SUB:     return left -  right;
        case BinaryOperator::MUL:     return left *  right;
        case BinaryOperator::DIV:     return left /  right;
        case BinaryOperator::REM:     return left %  right;
        case BinaryOperator::ISAB:    return left >  right;
        case BinaryOperator::ISABE:   return left >= right;
        case BinaryOperator::ISLS:    return left <  right;
        case BinaryOperator::ISLSE:   return left <= right;
        case BinaryOperator::ISEQ:    return left == right;
        case BinaryOperator::ISNE:    return left != right;
        default: break;
    }

    /* left is a variable value, if we`re here */ 

    switch (node.type())
    {
        case BinaryOperator::ADDASGN: left += right; break;
        case BinaryOperator::SUBASGN: left -= right; break;
        case BinaryOperator::MULASGN: left *= right; break;
        case BinaryOperator::DIVASGN: left /= right; break;
        case BinaryOperator::REMASGN: left %= right; break;
        default: __builtin_unreachable();
    }

    auto&& variable = static_cast<Variable const &>(node.larg());
    nametable.set_value(variable.name(), left);
    return left;
}

template <>
void visit(BinaryOperator const& node, interpreter::nametable::Nametable& nametable)
{
    (void) visit<BinaryOperator, int, interpreter::nametable::Nametable&>(node, nametable);
}

//-----------------------------------------------------------------------------
// WHILE
//-----------------------------------------------------------------------------
template <>
void visit(While const& node, interpreter::nametable::Nametable& nametable)
{
    LOGINFO("paracl: interpreter: execute WHILE statement");

    while (execute_expsession(node.condition(), nametable))
        execute_statement(node.body(), nametable);
}

//-----------------------------------------------------------------------------
// IF
//-----------------------------------------------------------------------------
template <>
void visit(If const& node, interpreter::nametable::Nametable& nametable)
{
    LOGINFO("paracl: interpreter: execute IF statement");

    if (not execute_expsession(node.condition(), nametable)) return;
    execute_statement(node.body(), nametable);
}

//-----------------------------------------------------------------------------

template <>
bool visit(If const& node, interpreter::nametable::Nametable& nametable)
{
    LOGINFO("paracl: interpreter: execute IF statement");
    if (not execute_expsession(node.condition(), nametable)) return false;
    execute_statement(node.body(), nametable);
    return true;
}

//-----------------------------------------------------------------------------
// ELSE
//-----------------------------------------------------------------------------
template <>
void visit(Else const& node, interpreter::nametable::Nametable& nametable)
{
    LOGINFO("paracl: interpreter: execute ELSE statement");
    execute_statement(node.body(), nametable);
}

//-----------------------------------------------------------------------------
// CONDITION
//-----------------------------------------------------------------------------
template <>
void visit(Condition const& node, interpreter::nametable::Nametable& nametable)
{
    LOGINFO("paracl: interpreter: execute CONDITION statement");
    
    // Проверяем все if-ы по порядку
    for (auto&& if_node : node.get_ifs())
        if (execute_if_with_return_codition_status(if_node, nametable)) return;

    if (not node.has_else()) return;
    execute_statement(node.get_else(), nametable);
}

//-----------------------------------------------------------------------------
// PRINT
//-----------------------------------------------------------------------------

template <>
void visit(Print const& node, interpreter::nametable::Nametable& nametable)
{
    LOGINFO("paracl: interpreter: execute print statement");

    for (auto&& arg : node)
    {
        if (arg.support<printable_string>())
            std::cout << print_string(arg);
        else
            std::cout << execute_expsession(arg, nametable);
    }
    std::cout << std::endl;
}

//-----------------------------------------------------------------------------
// SCOPE
//-----------------------------------------------------------------------------

template <>
void visit(Scope const& node, interpreter::nametable::Nametable& nametable)
{
    LOGINFO("paracl: interpreter: execute scope statement");

    nametable.new_scope();

    for (auto&& arg : node)
        execute_statement(arg, nametable);

    nametable.leave_scope();
}

//-----------------------------------------------------------------------------
} /* namespace last::node::visit_specializations */
//-----------------------------------------------------------------------------

SPECIALIZE_CREATE(last::node::Scan           , last::node::executable_statement  , last::node::executable_expression                      )
SPECIALIZE_CREATE(last::node::Variable       , last::node::executable_statement  , last::node::executable_expression                      )
SPECIALIZE_CREATE(last::node::NumberLiteral  , last::node::executable_statement  , last::node::executable_expression                      )
SPECIALIZE_CREATE(last::node::UnaryOperator  , last::node::executable_statement  , last::node::executable_expression                      )
SPECIALIZE_CREATE(last::node::BinaryOperator , last::node::executable_statement  , last::node::executable_expression                      )
SPECIALIZE_CREATE(last::node::If             , last::node::executable_statement  , last::node::executable_if_with_return_codition_status  )
SPECIALIZE_CREATE(last::node::Print          , last::node::executable_statement                                                           )
SPECIALIZE_CREATE(last::node::While          , last::node::executable_statement                                                           )
SPECIALIZE_CREATE(last::node::Else           , last::node::executable_statement                                                           )
SPECIALIZE_CREATE(last::node::Condition      , last::node::executable_statement                                                           )
SPECIALIZE_CREATE(last::node::Scope          , last::node::executable_statement                                                           )
SPECIALIZE_CREATE(last::node::StringLiteral  , last::node::printable_string                                                               )

//-----------------------------------------------------------------------------

#define THELAST_READ_AST_NO_INCLUDES
#include "read-ast.hpp"
#undef THELAST_READ_AST_NO_INCLUDES

//-----------------------------------------------------------------------------
 
namespace interpreter
{

using namespace last::node;

export
void interpret(std::filesystem::path const & ast_txt)
{
    LOGINFO("paracl: interpreter: start");

    auto&& ast = last::read(ast_txt);

    nametable::Nametable nametable;
    nametable.new_scope(); /* global scope */

    execute_statement(ast.root(), nametable);

    LOGINFO("paracl: interpreter: end");
}

} /* namespace ParaCL::interpreter */

//-----------------------------------------------------------------------------