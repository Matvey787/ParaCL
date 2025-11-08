#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>

extern int current_num_value;
extern std::string current_var_value;
extern int current_line;

int yylex();
int get_current_line();

#endif // LEXER_HPP
