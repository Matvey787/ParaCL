
#pragma once

#if not defined(THELAST_READ_AST_NO_INCLUDES)
#include <string_view>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <boost/json.hpp>
#endif /* not defined(THELAST_READ_AST_NO_INCLUDES) */


namespace last
{
namespace node::__detail
{

BinaryOperator::BinaryOperatorT string_to_bin_op(std::string_view op)
{
    using OpT = BinaryOperator::BinaryOperatorT;

    if (op == "+")   return OpT::ADD;     if (op == "-")   return OpT::SUB;
    if (op == "*")   return OpT::MUL;     if (op == "/")   return OpT::DIV;
    if (op == "%")   return OpT::REM;     if (op == "&&")  return OpT::AND;
    if (op == "||")  return OpT::OR;      if (op == ">")   return OpT::ISAB;
    if (op == ">=")  return OpT::ISABE;   if (op == "<")   return OpT::ISLS;
    if (op == "<=")  return OpT::ISLSE;   if (op == "==")  return OpT::ISEQ;
    if (op == "!=")  return OpT::ISNE;    if (op == "=")   return OpT::ASGN;
    if (op == "+=")  return OpT::ADDASGN; if (op == "-=")  return OpT::SUBASGN;
    if (op == "*=")  return OpT::MULASGN; if (op == "/=")  return OpT::DIVASGN;
    if (op == "%=")  return OpT::REMASGN;

    throw std::runtime_error("Unknown binary operator: " + std::string(op));
}

UnaryOperator::UnaryOperatorT string_to_un_op(std::string_view op)
{
    using OpT = UnaryOperator::UnaryOperatorT;
    if (op == "-") return OpT::MINUS;
    if (op == "+") return OpT::PLUS;
    if (op == "!") return OpT::NOT;

    throw std::runtime_error("Unknown unary operator: " + std::string(op));
}

BasicNode node_from_json(const boost::json::value& jv)
{
    auto&& obj = jv.as_object();
    auto&& kind = obj.at("kind").as_string();

    if (kind == traits::get_node_info<NumberLiteral, traits::NAME>())
    {
        auto&& value = static_cast<int>(obj.at(traits::get_node_info<NumberLiteral, traits::FIELD, 0>()).as_int64());
        auto&& node = NumberLiteral{std::move(value)};
        return create(std::move(node));
    }
    if (kind == traits::get_node_info<StringLiteral, traits::NAME>())
    {
        auto&& value = static_cast<std::string>(obj.at(traits::get_node_info<NumberLiteral, traits::FIELD, 0>()).as_string());
        auto&& node = StringLiteral{std::move(value)};
        return create(std::move(node));
    }
    if (kind == traits::get_node_info<Variable, traits::NAME>())
    {
        auto&& value = static_cast<std::string>(obj.at(traits::get_node_info<Variable, traits::FIELD, 0>()).as_string());
        auto&& node = Variable{std::move(value)};
        return create(std::move(node));
    }
    if (kind == traits::get_node_info<Scan, traits::NAME>())
    {
        auto&& node = Scan{};
        return create(std::move(node));
    }
    if (kind == traits::get_node_info<Print, traits::NAME>())
    {
        auto&& args = std::vector<BasicNode>{};
        for (auto&& arg_jv : obj.at(traits::get_node_info<Print, traits::FIELD, 0>()).as_array())
            args.push_back(node_from_json(arg_jv));
        auto&& node = Print{std::move(args)};
        return create(std::move(node));
    }
    if (kind == traits::get_node_info<UnaryOperator, traits::NAME>())
    {
        auto&& type = string_to_un_op(obj.at(traits::get_node_info<UnaryOperator, traits::FIELD, 0>()).as_string());
        auto&& arg = node_from_json(obj.at(traits::get_node_info<UnaryOperator, traits::FIELD, 1>()));
        auto&& node = UnaryOperator{type, std::move(arg)};
        return create(std::move(node));
    }
    if (kind == traits::get_node_info<BinaryOperator, traits::NAME>())
    {
        auto&& type = string_to_bin_op(obj.at(traits::get_node_info<BinaryOperator, traits::FIELD, 0>()).as_string());
        auto&& left = node_from_json(obj.at(traits::get_node_info<BinaryOperator, traits::FIELD, 1>()));
        auto&& right = node_from_json(obj.at(traits::get_node_info<BinaryOperator, traits::FIELD, 2>()));
        auto&& node = BinaryOperator{type, std::move(left), std::move(right)};
        return create(std::move(node));
    }
    if (kind == traits::get_node_info<While, traits::NAME>())
    {
        auto&& condition = node_from_json(obj.at(traits::get_node_info<While, traits::FIELD, 0>()));
        auto&& body = node_from_json(obj.at(traits::get_node_info<While, traits::FIELD, 1>()));
        auto&& node = While{std::move(condition), std::move(body)};
        return create(std::move(node));
    }
    if (kind == traits::get_node_info<If, traits::NAME>())
    {
        auto&& condition = node_from_json(obj.at(traits::get_node_info<If, traits::FIELD, 0>()));
        auto&& body = node_from_json(obj.at(traits::get_node_info<If, traits::FIELD, 1>()));
        auto&& node = If{std::move(condition), std::move(body)};
        return create(std::move(node));
    }
    if (kind == traits::get_node_info<Else, traits::NAME>())
    {
        auto&& body = node_from_json(obj.at(traits::get_node_info<Else, traits::FIELD, 0>()));
        auto&& node = Else{std::move(body)};
        return create(std::move(node));
    }
    if (kind == traits::get_node_info<Scope, traits::NAME>())
    {
        auto&& statements = std::vector<BasicNode>{};
        for (auto&& stmt_jv : obj.at(traits::get_node_info<Scope, traits::FIELD, 0>()).as_array())
            statements.push_back(node_from_json(stmt_jv));
        auto&& node = Scope{std::move(statements)};
        return create(std::move(node));
    }
    if (kind == traits::get_node_info<Condition, traits::NAME>())
    {
        auto&& node = Condition{};
        for (auto&& if_jv : obj.at(traits::get_node_info<Condition, traits::FIELD, 0>()).as_array())
            node.add_condition(node_from_json(if_jv));

        if (obj.contains(traits::get_node_info<Condition, traits::FIELD, 1>()))
        {
            auto&& else_node = node_from_json(obj.at(traits::get_node_info<Condition, traits::FIELD, 1>()));
            node.set_else(std::move(else_node));
        }

        return create(std::move(node));
    }

    throw std::runtime_error("Unsupported node kind during deserialization: " + std::string(kind));
}

} /* namespace node::__detail */

namespace __detail
{

AST read(std::string_view jsonData)
{
    auto&& jv = boost::json::parse(jsonData);
    auto&& jo = jv.as_object();

    auto&& kind = "kind";

    if (jo.at(kind).as_string() != "AST")
        throw std::runtime_error("Root JSON element is not a AST");

    auto&& root = node::__detail::node_from_json(jo.at("root"));

    return AST{std::move(root)};
}

} /* namespace __detail */

AST read(std::filesystem::path const & ast_json)
{
    auto&& in = std::ifstream{ast_json};

    if (in.fail())
        throw std::runtime_error("No such file: " + ast_json.string() + ".\nFailed read ast from json format.");

    auto&& buffer = std::ostringstream{};
    buffer << in.rdbuf();
    auto&& jsonData = buffer.str();

    return __detail::read(jsonData);
}

} /* namespace last */