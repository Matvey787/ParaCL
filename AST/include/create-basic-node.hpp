#pragma once

#define SPECIALIZE_CREATE(NodeT, ...)                                                                                       \
template <>                                                                                                                 \
inline last::node::BasicNode last::node::create(NodeT node)                                                                 \
{                                                                                                                           \
    return last::node::BasicNode::Actions<__VA_ARGS__>::create(std::move(node));                                            \
}

#define CREATE_SAME(...)                                                                                                    \
SPECIALIZE_CREATE(last::node::Scope           , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(last::node::Print           , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(last::node::Scan            , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(last::node::UnaryOperator   , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(last::node::BinaryOperator  , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(last::node::If              , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(last::node::Else            , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(last::node::Condition       , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(last::node::While           , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(last::node::NumberLiteral   , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(last::node::StringLiteral   , __VA_ARGS__)                                                                \
SPECIALIZE_CREATE(last::node::Variable        , __VA_ARGS__)
