module;

//---------------------------------------------------------------------------------------------------------------

#if defined(LOGGER)
#include "pineaplog.hpp"
#endif /* defined(LOGGER) */

#include <iostream>
#include <cstdlib>

#include "global/global.hpp"
#include "global/custom_console_output.hpp"

//---------------------------------------------------------------------------------------------------------------

export module parse_paracl_exit_code;

//---------------------------------------------------------------------------------------------------------------

export namespace ParaCL
{
//---------------------------------------------------------------------------------------------------------------

int parse_paracl_exit_code(const char* argv0, int paracl_exit_code)
{
    msg_assert(argv0, "argv[0] is nullptr. maybe you give not a argv[0]?");

    if (paracl_exit_code == EXIT_SUCCESS)
        return EXIT_SUCCESS;

    std::cerr << argv0 << ": " RED << "failed with exit code " << RESET_CONSOLE_OUT << paracl_exit_code << "\n";

    return paracl_exit_code;
}

//---------------------------------------------------------------------------------------------------------------

} /* export namespace ParaCL */

//---------------------------------------------------------------------------------------------------------------
