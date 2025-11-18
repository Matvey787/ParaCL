#pragma once

#include <string>

extern int         current_num_value;
extern std::string current_var_value;

extern std::string current_file;

int yylex();
