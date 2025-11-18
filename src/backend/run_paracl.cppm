module;

#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <filesystem>
#include <stdexcept>

#include "lexer/lexer.hpp"
#include "parser/parser.hpp"
#include "parser.tab.hpp"

#include "global/global.hpp"
#include "global/custom_console_output.hpp"

extern int yyparse();
extern void set_current_paracl_file(const std::string&);

extern FILE* yyin;
extern ParaCL::ProgramAST program;

export module run_paracl;

import paracl_extension;
import options_parser;
import paracl_interpreter;
import parse_paracl_exit_code;

#if defined(GRAPHVIZ)
import ast_graph_dump;
#endif /* defined(GRAPHVIZ) */

namespace ParaCL
{
int  no_sources_action      ();
int  one_source_action      (const std::string& source);

export int run_paracl(const OptionsParsing::program_options_t& program_options)
{
    const std::vector<std::string> sources          = program_options.sources  ;
    const size_t                   sources_quantity = sources        .size   ();

    msg_bad_exit(sources_quantity <= 1, "now we work only with 1 input file :(");

    int paracil_exit_code = EXIT_SUCCESS;

    if (sources_quantity == 0)
        paracil_exit_code = no_sources_action();
    else if (sources_quantity == 1)
        paracil_exit_code =  one_source_action(sources[0]);
    else
        builtin_unreachable_wrapper("now we dont parse any situations");

    return parse_paracl_exit_code(program_options.program_name, paracil_exit_code);
}

int no_sources_action()
{
    set_current_paracl_file("stdin");
    
    yyin = stdin;
    int result =  yyparse();

    if (result != EXIT_SUCCESS)
        return result;


#if defined(GRAPHVIZ)
    try {
        ast_dump(program);
    } catch(const std::runtime_error& e) {
        std::cerr << ERROR_MSG("Dump failed:\n") << e.what() << "\n";
    } catch(...) {
        std::cerr << "Dump failed!\n";
    }
#endif /* defined(GRAPHVIZ) */

    interpet(program);

    return result;
}

int one_source_action(const std::string& source)
{
    set_current_paracl_file(source);

    FILE* input_file = fopen(source.c_str(), "rb");

    if (not input_file)
        throw std::runtime_error(RED BOLD "no such file: " RESET_CONSOLE_OUT WHITE + source);

    yyin = input_file;

    int result = yyparse();

    fclose(input_file);

    if (result != EXIT_SUCCESS)
        return result;

#if defined(GRAPHVIZ)
    try {
        ast_dump(program);
    } catch(const std::runtime_error& e) {
        std::cerr << ERROR_MSG("Dump failed:\n") << e.what() << "\n";
    } catch(...) {
        std::cerr << ERROR_MSG("Dump failed!\n");
    }
#endif /* defined(GRAPHVIZ) */

    interpet(program);

    return result;
}

} /* namespace ParaCL */
