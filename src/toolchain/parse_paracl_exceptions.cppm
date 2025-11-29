module;

#include "parser/parse_error.hpp"
#include "parser/parser_exceptions.hpp"
#include <iostream>
#include <stdexcept>

#include "global/custom_console_output.hpp"

export module parse_paracl_exceptions;

namespace ParaCL
{

void paracl_error_msg_begin();

export void invalid_argument(const std::invalid_argument &e)
{
    paracl_error_msg_begin();
    std::cerr << "invalid argument: " << e.what() << "\n";
}

export void runtime_error(const std::runtime_error &e)
{
    paracl_error_msg_begin();
    std::cerr << "runtime error: " << e.what() << "\n";
}

export void parse_grammar_error(const ErrorHandler::parse_grammar_error &e)
{
    paracl_error_msg_begin();
    std::cerr << "syntax error: " << e.what() << "\n";
}

export void undeclarated_variable_error(const ErrorHandler::undeclarated_variable_error &e)
{
    paracl_error_msg_begin();
    std::cerr << "use undeclareted variable: " << e.what() << "\n";
}

export void undefined_error()
{
    paracl_error_msg_begin();
    std::cerr << "undefined error.\n";
}

void paracl_error_msg_begin()
{
    std::cerr << WHITE BOLD "paracl: " RED "error: " RESET_CONSOLE_OUT;
}

} /* namespace ParaCL */
