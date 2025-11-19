#pragma once

#include <string>
#include <cstdint>
#include "parser.tab.hpp"

namespace ErrorHandler
{

struct ErrorParseOptions
{
    bool show_bad_token     : 1 = false;
    bool show_error_context : 1 = false;
    bool show_posible_token : 1 = false;
    // bool show_error_place   : 1 = true; по моему бред, мы это всегда показываем
};

void
throwError(const yy::location& loc,
           const std::string& msg,
           const ErrorParseOptions& options = {});

} // namespace ErrorHandler
