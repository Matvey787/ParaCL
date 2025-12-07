#include "parser/parser_exceptions.hpp"
#include <string>

namespace ErrorHandler
{

parse_grammar_error::parse_grammar_error(std::string_view msg) : msg_(msg)
{
}

const char *parse_grammar_error::what() const noexcept
{
    return msg_.c_str();
}

undeclarated_variable_error::undeclarated_variable_error(std::string_view msg) : msg_(msg)
{
}

const char *undeclarated_variable_error::what() const noexcept
{
    return msg_.c_str();
}

} // namespace ErrorHandler
