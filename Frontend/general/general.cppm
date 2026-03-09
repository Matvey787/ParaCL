module;

import thelast;

#include "parser.tab.hpp"

#include <boost/json.hpp>
#include <cstdio>
#include <string_view>
// #include "spdlog/sinks/stdout_color_sinks.h"
// #include "spdlog/spdlog.h"

extern void set_current_paracl_file(std::string_view);
extern FILE* yyin;
extern last::AST program;

export module general;

export namespace ParaCL::general
{

last::AST generateAST(std::string_view inputFileName)
{

    FILE* inputFile = std::fopen(std::string(inputFileName).c_str(), "r");
    set_current_paracl_file(inputFileName);

    if (!inputFile) throw std::runtime_error("Can't open file: " + std::string(inputFileName));

    yyin = inputFile;

    yy::parser paracl_parser;
    
    int result = paracl_parser.parse();
    
    if (result != 0) throw std::runtime_error("Parsing errors occured.");

    std::fclose(inputFile);

    return program;
}




void init_logging()
{
    // auto console = spdlog::stdout_color_mt("console");

    // По умолчанию уровень INFO 
    // spdlog::set_default_logger(console);
    // spdlog::set_level(spdlog::level::info);
}

}; // namespace ParaCL::general
