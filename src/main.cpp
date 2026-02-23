#include <cstdlib>
#include <stdexcept>

#include "global/global.hpp"
#include "parser/parser_exceptions.hpp"

#include "log/log_api.hpp"

import options;
import runner;
import parse_paracl_exceptions;

ON_LOGGER(import spdlog_init;)
ON_DEBUG(import exceptoin_stacktrace;)

int main(int argc, char *argv[])
try
{
    ON_LOGGER(spdlog::init_spdlogger();)

    LOGINFO("paracl: start");

    ParaCL::options::Options::init(argc, argv);
    ParaCL::runner::Runner{ParaCL::options::Optoins::queryOptions()}.run();

    LOGINFO("paracl: exit success");
    return EXIT_SUCCESS;
}
#define INCLUDE_THIS_FILE_ONLY_AFTER_MAIN
#include "codegen/main_catch_blocks.in"
#undef INCLUDE_THIS_FILE_ONLY_AFTER_MAIN
