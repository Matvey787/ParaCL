#include "lexer.hpp"
#include "parser.tab.hpp"
#include <iostream>
#include <cstdio>

// Объявляем внешние переменные Bison/Flex
extern FILE* yyin;
extern int yyparse();

// Объявляем внешние переменные из лексера
extern int current_num_value;
extern std::string current_var_value;
extern int current_line;

int main(int argc, char* argv[]) {
    if (argc > 1) {
        FILE* input_file = fopen(argv[1], "r");
        if (input_file) {
            yyin = input_file;
        } else {
            std::cerr << "Cannot open file: " << argv[1] << std::endl;
            return 1;
        }
    } else {
        std::cout << "Enter your program (Ctrl+D to end):" << std::endl;
        yyin = stdin;
    }

    int result = yyparse();
    
    if (argc > 1) {
        fclose(yyin);
    }
    
    return result;
}
