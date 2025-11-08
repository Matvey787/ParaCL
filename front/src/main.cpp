#include <FlexLexer.h>
#include "parser.tab.hh"
#include <iostream>

using namespace Calc;

// Обёртка для лексера
class MyLexer : public yyFlexLexer {
public:
    using yyFlexLexer::yyFlexLexer;
    int yylex(Parser::semantic_type *yylval, Parser::location_type *yyloc = nullptr) {
        this->yylval_ptr = yylval; // передаем semantic_type
        return yyFlexLexer::yylex();
    }
};

MyLexer lexer;

// Глобальная функция yylex для Bison
int yylex(Parser::semantic_type *yylval, Parser::location_type *yyloc) {
    return lexer.yylex(yylval, yyloc);
}

int main() {
    Parser parser;
    parser.parse();
    return 0;
}
