module;

#include <cstddef>

#define STRINGIFY(X) #X

export module node_traits;

export import ast_nodes;

export
namespace last::node::traits
{

//--------------------------------------------------------------------------------------------------------------------------------------

enum NodeInfo
{
    NAME,
    FIELDS,
    FIELD,
    OPERATOR_NAME,
};

//--------------------------------------------------------------------------------------------------------------------------------------

template <typename NodeT, NodeInfo n, int id = 0>
struct NodeTraits
{
    static_assert(false, "using unspecialized NodeTraits");
};

//--------------------------------------------------------------------------------------------------------------------------------------

template <typename NodeT, NodeInfo n, int id = 0>
constexpr typename NodeTraits<NodeT, n, id>::type get_node_info()
{ return NodeTraits<NodeT, n, id>::value; }

//--------------------------------------------------------------------------------------------------------------------------------------
// SCOPE
//--------------------------------------------------------------------------------------------------------------------------------------

template <>
struct NodeTraits<Scope, NodeInfo::NAME>
{
    using type = const char *;
    static constexpr type value = STRINGIFY(Scope);
};

template <>
struct NodeTraits<Scope, NodeInfo::FIELDS>
{
    using type = size_t;
    static constexpr type value = 1;
};

template <>
struct NodeTraits<Scope, NodeInfo::FIELD, 0>
{
    using type = const char *;
    static constexpr type value = "args";
};

//--------------------------------------------------------------------------------------------------------------------------------------
// PRINT
//--------------------------------------------------------------------------------------------------------------------------------------

template <>
struct NodeTraits<Print, NodeInfo::NAME>
{
    using type = const char *;
    static constexpr type value = STRINGIFY(Print);
};

template <>
struct NodeTraits<Print, NodeInfo::FIELDS>
{
    using type = size_t;
    static constexpr type value = 1;
};

template <>
struct NodeTraits<Print, NodeInfo::FIELD, 0>
{
    using type = const char *;
    static constexpr type value = "args";
};

//--------------------------------------------------------------------------------------------------------------------------------------
// SCAN
//--------------------------------------------------------------------------------------------------------------------------------------

template <>
struct NodeTraits<Scan, NodeInfo::NAME>
{
    using type = const char *;
    static constexpr type value = STRINGIFY(Scan);
};

template <>
struct NodeTraits<Scan, NodeInfo::FIELDS>
{
    using type = size_t;
    static constexpr type value = 0;
};

//--------------------------------------------------------------------------------------------------------------------------------------
// VARIABLE
//--------------------------------------------------------------------------------------------------------------------------------------

template <>
struct NodeTraits<Variable, NodeInfo::NAME>
{
    using type = const char *;
    static constexpr type value = STRINGIFY(Variable);
};

template <>
struct NodeTraits<Variable, NodeInfo::FIELDS>
{
    using type = size_t;
    static constexpr type value = 1;
};

template <>
struct NodeTraits<Variable, NodeInfo::FIELD, 0>
{
    using type = const char *;
    static constexpr type value = "name";
};

//--------------------------------------------------------------------------------------------------------------------------------------
// NUMBER LITERAL
//--------------------------------------------------------------------------------------------------------------------------------------

template <>
struct NodeTraits<NumberLiteral, NodeInfo::NAME>
{
    using type = const char *;
    static constexpr type value = STRINGIFY(NumberLiteral);
};

template <>
struct NodeTraits<NumberLiteral, NodeInfo::FIELDS>
{
    using type = size_t;
    static constexpr type value = 1;
};

template <>
struct NodeTraits<NumberLiteral, NodeInfo::FIELD, 0>
{
    using type = const char *;
    static constexpr type value = "value";
};

//--------------------------------------------------------------------------------------------------------------------------------------
// STRING LITERAL
//--------------------------------------------------------------------------------------------------------------------------------------

template <>
struct NodeTraits<StringLiteral, NodeInfo::NAME>
{
    using type = const char *;
    static constexpr type value = STRINGIFY(StringLiteral);
};

template <>
struct NodeTraits<StringLiteral, NodeInfo::FIELDS>
{
    using type = size_t;
    static constexpr type value = 1;
};

template <>
struct NodeTraits<StringLiteral, NodeInfo::FIELD, 0>
{
    using type = const char *;
    static constexpr type value = "value";
};

//--------------------------------------------------------------------------------------------------------------------------------------
// UNARY OPERATOR_NAME
//--------------------------------------------------------------------------------------------------------------------------------------

template <>
struct NodeTraits<UnaryOperator, NodeInfo::NAME>
{
    using type = const char *;
    static constexpr type value = STRINGIFY(UnaryOperator);
};

template <>
struct NodeTraits<UnaryOperator, NodeInfo::FIELDS>
{
    using type = size_t;
    static constexpr type value = 2;
};

template <>
struct NodeTraits<UnaryOperator, NodeInfo::FIELD, 0>
{
    using type = const char *;
    static constexpr type value = "type";
};

template <>
struct NodeTraits<UnaryOperator, NodeInfo::FIELD, 1>
{
    using type = const char *;
    static constexpr type value = "arg";
};

template <>
struct NodeTraits<UnaryOperator, NodeInfo::OPERATOR_NAME, UnaryOperator::PLUS>
{
    using type = const char*; 
    static constexpr type value = "+";
};

template <>
struct NodeTraits<UnaryOperator, NodeInfo::OPERATOR_NAME, UnaryOperator::MINUS>
{
    using type = const char*; 
    static constexpr type value = "-";
};

template <>
struct NodeTraits<UnaryOperator, NodeInfo::OPERATOR_NAME, UnaryOperator::NOT>
{
    using type = const char*; 
    static constexpr type value = "!";
};


//--------------------------------------------------------------------------------------------------------------------------------------
// BINARY OPERATOR_NAME
//--------------------------------------------------------------------------------------------------------------------------------------

template <>
struct NodeTraits<BinaryOperator, NodeInfo::NAME>
{
    using type = const char *;
    static constexpr type value = STRINGIFY(BinaryOperator);
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::FIELDS>
{
    using type = size_t;
    static constexpr type value = 3;
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::FIELD, 0>
{
    using type = const char *;
    static constexpr type value = "type";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::FIELD, 1>
{
    using type = const char *;
    static constexpr type value = "left";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::FIELD, 2>
{
    using type = const char *;
    static constexpr type value = "right";
};


template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::AND>
{
    using type = const char*; 
    static constexpr type value = "&&";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::OR>
{
    using type = const char*; 
    static constexpr type value = "||";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::ADD>
{
    using type = const char*; 
    static constexpr type value = "+";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::SUB>
{
    using type = const char*; 
    static constexpr type value = "-";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::MUL>
{
    using type = const char*; 
    static constexpr type value = "*";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::DIV>
{
    using type = const char*; 
    static constexpr type value = "/";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::REM>
{
    using type = const char*; 
    static constexpr type value = "%";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::ISAB>
{
    using type = const char*; 
    static constexpr type value = ">";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::ISABE>
{
    using type = const char*; 
    static constexpr type value = ">=";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::ISLS>
{
    using type = const char*; 
    static constexpr type value = "<";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::ISLSE>
{
    using type = const char*; 
    static constexpr type value = "<=";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::ISEQ>
{
    using type = const char*; 
    static constexpr type value = "==";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::ISNE>
{
    using type = const char*;
    static constexpr const char* value = "!=";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::ASGN>
{
    using type = const char*;
    static constexpr const char* value = "=";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::ADDASGN>
{
    using type = const char*; 
    static constexpr type value = "+=";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::SUBASGN>
{
    using type = const char*; 
    static constexpr type value = "-=";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::MULASGN>
{
    using type = const char*; 
    static constexpr type value = "*=";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::DIVASGN>
{
    using type = const char*; 
    static constexpr type value = "/=";
};

template <>
struct NodeTraits<BinaryOperator, NodeInfo::OPERATOR_NAME, BinaryOperator::REMASGN>
{
    using type = const char*; 
    static constexpr type value = "%=";
};


//--------------------------------------------------------------------------------------------------------------------------------------
// WHILE
//--------------------------------------------------------------------------------------------------------------------------------------

template <>
struct NodeTraits<While, NodeInfo::NAME>
{
    using type = const char *;
    static constexpr type value = STRINGIFY(WHile);
};

template <>
struct NodeTraits<While, NodeInfo::FIELDS>
{
    using type = size_t;
    static constexpr type value = 2;
};

template <>
struct NodeTraits<While, NodeInfo::FIELD, 0>
{
    using type = const char *;
    static constexpr type value = "condition";
};

template <>
struct NodeTraits<While, NodeInfo::FIELD, 1>
{
    using type = const char *;
    static constexpr type value = "body";
};

//--------------------------------------------------------------------------------------------------------------------------------------
// IF
//--------------------------------------------------------------------------------------------------------------------------------------

template <>
struct NodeTraits<If, NodeInfo::NAME>
{
    using type = const char *;
    static constexpr type value = STRINGIFY(If);
};

template <>
struct NodeTraits<If, NodeInfo::FIELDS>
{
    using type = size_t;
    static constexpr type value = 2;
};

template <>
struct NodeTraits<If, NodeInfo::FIELD, 0>
{
    using type = const char *;
    static constexpr type value = "condition";
};

template <>
struct NodeTraits<If, NodeInfo::FIELD, 1>
{
    using type = const char *;
    static constexpr type value = "body";
};

//--------------------------------------------------------------------------------------------------------------------------------------
// ELSE
//--------------------------------------------------------------------------------------------------------------------------------------

template <>
struct NodeTraits<Else, NodeInfo::NAME>
{
    using type = const char *;
    static constexpr type value = STRINGIFY(Else);
};

template <>
struct NodeTraits<Else, NodeInfo::FIELDS>
{
    using type = size_t;
    static constexpr type value = 1;
};

template <>
struct NodeTraits<Else, NodeInfo::FIELD, 0>
{
    using type = const char *;
    static constexpr type value = "body";
};

//--------------------------------------------------------------------------------------------------------------------------------------
// CONDITION
//--------------------------------------------------------------------------------------------------------------------------------------

template <>
struct NodeTraits<Condition, NodeInfo::NAME>
{
    using type = const char *;
    static constexpr type value = STRINGIFY(Condition);
};

template <>
struct NodeTraits<Condition, NodeInfo::FIELDS>
{
    using type = size_t;
    static constexpr type value = 2;
};

template <>
struct NodeTraits<Condition, NodeInfo::FIELD, 0>
{
    using type = const char*;
    static constexpr type value = "if";
};

template <>
struct NodeTraits<Condition, NodeInfo::FIELD, 1>
{
    using type = const char*;
    static constexpr type value = "else";
};

//--------------------------------------------------------------------------------------------------------------------------------------
} /* namespace last::node::traits */
//--------------------------------------------------------------------------------------------------------------------------------------
