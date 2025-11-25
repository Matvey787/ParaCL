#include <cstdlib>
#include <stdexcept>

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
    LOGINFO("paracl: parse program options");
    const OptionsParsing::program_options_t program_options = OptionsParsing::parse_program_options(argc, argv);
    LOGINFO("paracl: parse program options completed");
    LOGINFO("paracl: run");
    ParaCL::run_paracl(program_options);
    LOGINFO("paracl: exit success");
    return EXIT_SUCCESS;
}
catch (const std::invalid_argument &e)
{
    ParaCL::invalid_argument(e);
    LOGERR("paracl: exit failure: bad argument");
    return EXIT_FAILURE;
}
catch (const std::runtime_error &e)
{
    ParaCL::runtime_error(e);
    LOGERR("paracl: exit failure: run time error");
    return EXIT_FAILURE;
}
catch (...)
{
    ParaCL::undefined_error();
    LOGERR("paracl: exit failure: undefined exceprion");
    return EXIT_FAILURE;
}
