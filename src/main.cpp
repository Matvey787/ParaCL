#include <cstdlib>
#include <stdexcept>

#include "parser/parser_exceptions.hpp"
#include "log/log_api.hpp"

import options_parser;
import run_paracl;
import parse_paracl_exceptions;

#if defined(LOGGER)
import spdlog_init;
#endif /* defined(LOGGER)*/

// #define SPDINFO(...) spdlog::info("[{} { ]")

int main(int argc, char *argv[])
try
{
    ON_LOGGER(spdlog::init_spdlogger();)
    LOGINFO("paracl: start");
    const OptionsParsing::program_options_t program_options = OptionsParsing::parse_program_options(argc, argv);
    LOGINFO("paracl: run");
    ParaCL::run_paracl(program_options);
    LOGINFO("paracl: exit success");
    return EXIT_SUCCESS;
}
catch (const std::invalid_argument &e)
{
    LOGERR("paracl: exit failure: bad argument");
    ParaCL::invalid_argument(e);
    return EXIT_FAILURE;
}
catch (const std::runtime_error &e)
{
    LOGERR("paracl: exit failure: run time error");
    ParaCL::runtime_error(e);
    return EXIT_FAILURE;
}
/* FIXME: add throw for ErrorHandler::parse_grammar_error */
catch (const ErrorHandler::parse_grammar_error& e)
{
    LOGERR("paracl: exit failure: parse grammar error");
    ParaCL::parse_grammar_error(e);
    return EXIT_FAILURE;
}
/* FIXME: add throw for ErrorHandler::undeclareted_variale_error */
catch (const ErrorHandler::undeclarated_variable_error& e)
{
    LOGERR("paracl: exit failure: parse grammar error");
    ParaCL::undeclarated_variable_error(e);
    return EXIT_FAILURE;
}
catch (...)
{
    LOGERR("paracl: exit failure: undefined exception");
    ParaCL::undefined_error();
    return EXIT_FAILURE;
}
