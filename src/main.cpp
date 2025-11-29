#include <cstdlib>
#include <stdexcept>

#include "log/log_api.hpp"
#include "parser/parser_exceptions.hpp"

import options_parser;
import paracl_toolchain;
import parse_paracl_exceptions;

#if defined(LOGGER)
import spdlog_init;
#endif /* defined(LOGGER)*/
#include <iostream>

int main(int argc, char *argv[])
try
{    
    ON_LOGGER(spdlog::init_spdlogger();)
    LOGINFO("paracl: start");

    const Options::program_options_t program_options = Options::OptionsParser(argc, argv).get_program_options();

    ParaCL::Toolchain(program_options).run_paracl();

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
catch (const ErrorHandler::parse_grammar_error &e)
{
    LOGERR("paracl: exit failure: parse grammar error");
    ParaCL::parse_grammar_error(e);
    return EXIT_FAILURE;
}
/* FIXME: add throw for ErrorHandler::undeclareted_variale_error */
catch (const ErrorHandler::undeclarated_variable_error &e)
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
