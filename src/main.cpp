#if defined(LOGGER)
#include "pineaplog.hpp"
#endif /* defined(LOGGER) */

#include <iostream>
#include <stdexcept>

import options_parser;
import run_paracl;
import parse_paracl_exit_code;

//---------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    try {
        const OptionsParsing::program_options_t program_options = OptionsParsing::parse_program_options(argc, argv);
        return ParaCL::run_paracl(program_options);
    } catch (const std::invalid_argument& e) {
        std::cerr << ParaCL::paracl_failed_message_before << e.what() << "\n";
    } catch (const std::runtime_error& e) {
        std::cerr << ParaCL::paracl_failed_message_before << e.what() << "\n";
    }

    return ParaCL::parse_paracl_exit_code(argv[0], EXIT_FAILURE);
}

//---------------------------------------------------------------------------------------------------------------
