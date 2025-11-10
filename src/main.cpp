#if defined(LOGGER)
#include "pineaplog.hpp"
#endif /* defined(LOGGER) */

import options_parser;
import run_paracl;
import parse_paracl_exit_code;

//---------------------------------------------------------------------------------------------------------------

int main(int argc, char* argv[])
{
    const OptionsParsing::program_options_t program_options  = OptionsParsing::parse_program_options (argc           , argv            );
    const int                               paracl_exit_code = ParaCL        ::run_paracl            (program_options                  );
    return                                                     ParaCL        ::parse_paracl_exit_code(argv[0]        , paracl_exit_code);
}

//---------------------------------------------------------------------------------------------------------------
