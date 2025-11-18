module;

//---------------------------------------------------------------------------------------------------------------

#if defined(LOGGER)
#include "pineaplog.hpp"
#endif /* defined(LOGGER) */

#include <iostream>
#include <cstdlib>
#include <string>

#include "global/custom_console_output.hpp"

//---------------------------------------------------------------------------------------------------------------

export module parse_paracl_exit_code;

//---------------------------------------------------------------------------------------------------------------

export namespace ParaCL
{

//---------------------------------------------------------------------------------------------------------------

const std::string paracl_failed_message_before = "paracl: error: ";

//---------------------------------------------------------------------------------------------------------------

int parse_paracl_exit_code(const std::string& argv0, int paracl_exit_code)
{
    if (paracl_exit_code == EXIT_SUCCESS)
        return EXIT_SUCCESS;

    std::cerr << argv0 << ": " RED << "failed with exit code " << RESET_CONSOLE_OUT << paracl_exit_code << "\n";

    return paracl_exit_code;
}

//---------------------------------------------------------------------------------------------------------------

} /* export namespace ParaCL */

//---------------------------------------------------------------------------------------------------------------
