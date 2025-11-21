#pragma once

#include <cstdint>
#include <string>

#include "parser.tab.hpp"

namespace ErrorHandler
{

struct ErrorParseOptions
{
    bool show_bad_token : 1 = false;
    bool show_error_context : 1 = false;
    bool show_posible_token : 1 = false;
};

void throwError(const yy::location &loc, const std::string &msg, const ErrorParseOptions &options = {});

} /* namespace ErrorHandler */
