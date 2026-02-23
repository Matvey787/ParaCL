module;

#include <iostream>
#include <stdexcept>

#include "global/custom_console_output.hpp"
#include "global/global.hpp"
#include "parser/parse_error.hpp"
#include "parser/parser_exceptions.hpp"

export module parse_paracl_exceptions;

namespace ParaCL
{
namespace parse_exceptions
{

void paracl_error_msg_begin();

export void exception_info(const std::invalid_argument &e)
{
    LOGERR("paracl: exit failure: bad argument");
    paracl_error_msg_begin();
    std::cerr << "invalid argument: " << e.what() << "\n";
}

export void exception_info(const std::runtime_error &e)
{
    LOGERR("paracl: exit failure: run time error");
    paracl_error_msg_begin();
    std::cerr << "runtime error: " << e.what() << "\n";
}

export void exception_info(const ErrorHandler::parse_grammar_error &e)
{
    LOGERR("paracl: exit failure: parse grammar error");
    paracl_error_msg_begin();
    std::cerr << "syntax error: " << e.what() << "\n";
}

export void exception_info(const ErrorHandler::undeclarated_variable_error &e)
{
    LOGERR("paracl: exit failure: parse grammar error");
    paracl_error_msg_begin();
    std::cerr << "use undeclareted variable: " << e.what() << "\n";
}

export void exception_info(const std::exception &e)
{
    LOGERR("paracl: exit failure: std::exception catched");
    paracl_error_msg_begin();
    std::cerr << e.what() << "\n";
}

export void exception_info(void)
{
    LOGERR("paracl: exit failure: undefined exception");
    paracl_error_msg_begin();
    std::cerr << "undefined error.\n";
}

void paracl_error_msg_begin()
{
    std::cerr << WHITE BOLD "\nparacl: " RED "error: " RESET_CONSOLE_OUT;
}

} /* namespace parse_exceptions */
} /* namespace ParaCL */
