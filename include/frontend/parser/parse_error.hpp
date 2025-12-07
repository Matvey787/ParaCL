#pragma once

#include <exception>
#include <stdexcept>
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

void throwError(const yy::location &loc, std::string_view msg, const ErrorParseOptions &options = {});

} /* namespace ErrorHandler */
