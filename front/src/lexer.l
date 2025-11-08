%option c++ noyywrap
%{
#include "parser.tab.hh"
#include <cstdlib>
#include <iostream>

using namespace Calc; // пространство имён парсера
%}

%%

[0-9]+      { yylval->as<int>() = std::stoi(yytext); return NUMBER; }
[ \t\n]+    ;   // пробелы игнорируем
"+"         { return PLUS; }

.           { return yytext[0]; }

%%
