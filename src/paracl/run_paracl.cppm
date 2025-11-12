module;

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <filesystem>

#include "global/global.hpp"
#include "global/custom_console_output.hpp"
// могли бы import paraCL; но спасибо нашему доблестному bison
#include "paraCL_crutch_for_parsery.hpp"
#include "parser.tab.hpp"
#include "lexer.hpp"

import paracl_extension;
import options_parser;

extern FILE* yyin;
extern int yyparse();

extern ParaCL::ProgramAST program;


export module run_paracl;


int  no_sources_action      ();
int  one_source_action      (const std::string& source);

[[noreturn]]
void failed_open_source_file(const std::string& source);


export namespace ParaCL
{

int run_paracl(const OptionsParsing::program_options_t& program_options)
{
    const std::vector<std::string> sources          = program_options.sources  ;
    const size_t                   sources_quantity = sources        .size   ();

    msg_bad_exit(sources_quantity <= 1, "now we work only with 1 input file :(");

    if (sources_quantity == 0)
        return no_sources_action();

    if (sources_quantity == 1)
        return one_source_action(sources[0]);

    builtin_unreachable_wrapper("now we dont parse any situations");
}


} /* export namespace ParaCL */




int no_sources_action()
{
    // std::cout << "Enter your program (Ctrl+D to finish):" << std::endl;

    yyin = stdin;
    int result =  yyparse();

    #ifdef GRAPHVIZ
        ParaCL::dump(program);
    #endif

    ParaCL::compile(program);

    return result;
}


int one_source_action(const std::string& source)
{
    FILE* input_file = fopen(source.c_str(), "r");

    if (not input_file)
        failed_open_source_file(source); // exit 1

    yyin = input_file;

    int result = yyparse();

    #ifdef GRAPHVIZ
        ParaCL::dump(program);
    #endif

    ParaCL::compile(program);
    
    fclose(input_file);

    return result;
}


[[noreturn]]
void failed_open_source_file(const std::string& source)
{
    std::cerr << RED BOLD "No such file: " RESET_CONSOLE_OUT WHITE << source << "\n";
    exit(EXIT_FAILURE);
}
