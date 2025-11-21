#if defined(LOGGER)
#include "pineaplog.hpp"
#endif /* defined(LOGGER) */

#include <iostream>
#include <stdexcept>

import options_parser;
import run_paracl;
import parse_paracl_exceptions;

int main(int argc, char *argv[])
try
{
    const OptionsParsing::program_options_t program_options = OptionsParsing::parse_program_options(argc, argv);
    ParaCL::run_paracl(program_options);
    return EXIT_SUCCESS;
}
catch (const std::invalid_argument &e)
{
    ParaCL::invalid_argument(e);
    return EXIT_FAILURE;
}
catch (const std::runtime_error &e)
{
    ParaCL::runtime_error(e);
    return EXIT_FAILURE;
}
