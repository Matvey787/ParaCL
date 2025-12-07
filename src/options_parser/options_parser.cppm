/*
'optarg' and 'optind' - is a global variables from getopt.h
their declaration there:
extern char *optarg;
extern int optind;
*/
module;

//---------------------------------------------------------------------------------------------------------------

#include <cassert>
#include <cstdlib>
#include <filesystem>
#include <getopt.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <unistd.h>
#include <vector>

#include "global/custom_console_output.hpp"
#include "global/global.hpp"

#include "log/log_api.hpp"

// не бейте, я просто развлекаюсь
extern const char *__progname;

//---------------------------------------------------------------------------------------------------------------

export module options_parser;

//---------------------------------------------------------------------------------------------------------------

import paracl_info;

//---------------------------------------------------------------------------------------------------------------

void set_getopt_args_default_values();

//---------------------------------------------------------------------------------------------------------------

namespace Options
{
//---------------------------------------------------------------------------------------------------------------

export struct program_options_t
{
    std::string program_name;
    std::vector<std::filesystem::path> sources;

    std::filesystem::path executable_file = "a.out";
    std::vector<std::filesystem::path> llvm_ir_files;
    std::vector<std::filesystem::path> object_files;

    ON_GRAPHVIZ(std::filesystem::path dot_file; bool ast_dump : 1 = false;) /* ON_GRAPHVIZ */

    bool compile : 1 = false;
};

//---------------------------------------------------------------------------------------------------------------

export class OptionsParser
{
  public:
    Options::program_options_t get_program_options() const;

    OptionsParser() = delete;
    OptionsParser(int argc, char *argv[]);

  private:
    Options::program_options_t program_options_;

    void set_program_name();

    [[noreturn]]
    void parse_flag_help() const;
    [[noreturn]]
    void parse_flag_version() const;

    ON_GRAPHVIZ(void parse_flag_ast_dump();) /* ON_GRAPHVIZ */

    void parse_flag_compile();
    void parse_flag_output();

    void parse_not_a_flag(const char *arg);

    [[noreturn]]
    void undefined_option(const char *option) const;

    void set_out_files();
};

//---------------------------------------------------------------------------------------------------------------

enum flag_key
{
    help = 'h',
    version = 'v',
    compile = 'c',
    output = 'o',
    ON_GRAPHVIZ(ast_dump = 'd', ) undefined_option_key = '?',
    end_of_parsing = -1
};

//---------------------------------------------------------------------------------------------------------------

constexpr option long_options[] = {
    {"help", no_argument, 0, help},
    {"version", no_argument, 0, version},
    ON_GRAPHVIZ({"ast-dump", required_argument, 0, ast_dump}, ){"compile", no_argument, 0, compile},
    {"output", required_argument, 0, output},
    {0, 0, 0, 0}};

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// class public methods
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

OptionsParser::OptionsParser(int argc, char *argv[]) : program_options_()
{
    LOGINFO("paracl: options parser: begin parse options");

    LOGINFO("argc = {}", argc);
    for (int i = 0; i < argc; ++i)
        LOGINFO("argv[{}] = \"{}\"", i, argv[i] ? argv[i] : "NULL");

    set_program_name();

    int option;
    while ((option = getopt_long(argc, argv, "hvd:co:", long_options, nullptr)) != end_of_parsing)
    {
        LOGINFO("paracl: options parser: processing option '{}', optarg = '{}'", static_cast<char>(option),
                optarg ? optarg : "NULL");

        switch (option)
        {
        case help:
            parse_flag_help();
            break;
        case version:
            parse_flag_version();
            break;
        case compile:
            parse_flag_compile();
            break;
        case output:
            parse_flag_output();
            break;
            ON_GRAPHVIZ(case ast_dump : parse_flag_ast_dump(); break;) /* ON_GRAPHVIZ */
        case undefined_option_key:
            throw std::invalid_argument("invalid option");
        default:
            undefined_option(argv[optind - 1]);
            break;
        }
    }

    for (int i = optind; i < argc; i++)
        parse_not_a_flag(argv[i]);

    set_getopt_args_default_values();

    if (program_options_.sources.empty())
        throw std::invalid_argument("no source files specified");

    set_out_files();

    LOGINFO("paracl: options parser: parse options completed");
}

//---------------------------------------------------------------------------------------------------------------

Options::program_options_t OptionsParser::get_program_options() const
{
    return program_options_;
}

// class private methods
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

void OptionsParser::set_program_name()
{
    msg_assert(__progname, "__progname is nullptr? are you sure, that you give here arg[0]?");
    LOGINFO("set current program name: \"{}\"", __progname);
    program_options_.program_name = std::string(__progname);
}

//---------------------------------------------------------------------------------------------------------------

[[noreturn]]
void OptionsParser::parse_flag_help() const
{
    LOGINFO("paracl: options parser: find '--help' flag");

    std::cout << BOLD "There are all flags and parametrs:" RESET_CONSOLE_OUT << R"(
    -h --help
    -v --version
    -d --ast-dump=
    -c --compile
    -o --output=

    *.cl - source files

    -h, --help
        Show this help message

    -v, --version
        Show version information

    -c, --compile
        Compile to executable

    -o, --output=FILE
        Specify output filename

    -d, --ast-dump=FILE
        Dump AST to .dot file for Graphviz

    Examples:
        paracl -c -o program source.cl
        paracl --ast-dump=ast.dot source.cl
        paracl -d ast.dot -c -o program source.cl
)" << std::endl;

    LOGINFO("paracl: exit success");
    exit(EXIT_SUCCESS);
}

//---------------------------------------------------------------------------------------------------------------

[[noreturn]]
void OptionsParser::parse_flag_version() const
{
    LOGINFO("paracl: options parser: find '--version' flag");

    std::cout << "ParaCL\n"
                 "Version     : "
              << ParaCL::paracl_info.version
              << "\n"
                 "Build at    : "
              << ParaCL::paracl_info.build_date
              << "\n"
                 "Build type  : "
              << ParaCL::paracl_info.build_type
              << "\n"
                 "Build with  : "
              << ParaCL::paracl_info.compiler
              << "\n"
                 "Commit hash : "
              << ParaCL::paracl_info.git_commit
              << "\n"
                 "Architecture: "
              << ParaCL::paracl_info.architecture << std::endl;

    LOGINFO("paracl: exit success");
    exit(EXIT_SUCCESS);
}

//---------------------------------------------------------------------------------------------------------------

ON_GRAPHVIZ(void OptionsParser::parse_flag_ast_dump() {
    LOGINFO("paracl: options parser: --ast-dump={}", optarg);
    msg_assert(optarg, "Expected filename for --ast-dump");
    program_options_.ast_dump = true;
    program_options_.dot_file = optarg;
}) /* ON_GRAPHVIZ */

//---------------------------------------------------------------------------------------------------------------

void OptionsParser::parse_flag_compile()
{
    LOGINFO("paracl: options parser: --compile");
    program_options_.compile = true;
}

//---------------------------------------------------------------------------------------------------------------

void OptionsParser::parse_flag_output()
{
    msg_assert(optarg, "Expected filename for --output");
    LOGINFO("paracl: options parser: --output={}", optarg);
    program_options_.executable_file = optarg;
}

//---------------------------------------------------------------------------------------------------------------

void OptionsParser::parse_not_a_flag(const char *arg)
{
    LOGINFO("paracl: options parser: parse not a flag: \"{}\"", arg);

    std::filesystem::path file(arg);
    std::string extension(file.extension().string());

    if (extension == ".cl")
    {
        LOGINFO("paracl: options parser: find source file: \"{}\"", arg);
        program_options_.sources.push_back(std::string(arg));
        return;
    }

    if (extension == ".o" or extension == ".obj")
    {
        LOGINFO("paracl: options parser: find object file: \"{}\"", arg);
        program_options_.object_files.push_back(file);
        return;
    }

    LOGERR("paracl: options parser: unexpected file: \"{}\"", arg);
    throw std::invalid_argument("bad source file: unexpected extension: '" + std::string(arg) +
                                "', expected '.cl' - for paracl src, '.ll' - for llvm ir, or '.o' - for object file");
}

//---------------------------------------------------------------------------------------------------------------

[[noreturn]]
void OptionsParser::undefined_option(const char *option) const
{
    LOGERR("paracl: options parser: undefined option \"{}\"", option);
    throw std::invalid_argument("Undefined option: '" + std::string(option) + "'");
}

//---------------------------------------------------------------------------------------------------------------

void OptionsParser::set_out_files()
{
    LOGINFO("paracl: options parser: set llvm ir and obj files correct value");

    const size_t sources_size = program_options_.sources.size();
    const size_t object_files_size = program_options_.object_files.size();

    if (object_files_size > sources_size)
        throw std::invalid_argument("there are more llvm ir files, then sources files");

    program_options_.object_files.resize(sources_size);
    program_options_.llvm_ir_files.resize(sources_size);

    for (size_t it = 0, ite = sources_size; it < ite; ++it)
    {
        std::filesystem::path &object_file = program_options_.object_files[it];
        std::filesystem::path &llvm_ir_file = program_options_.llvm_ir_files[it];

        if (object_file.string() == "")
        {
            object_file = program_options_.sources[it];
            object_file.replace_extension(".o");
        }

        llvm_ir_file = object_file;
        llvm_ir_file.replace_extension(".ll");
    }
}

//---------------------------------------------------------------------------------------------------------------

} /* namespace Options */

//---------------------------------------------------------------------------------------------------------------

/*
this need if more then 1 call of optoins_parser.
oparg, optind - is extern in getopt.h, so we need to make it dafault with ourself
*/
void set_getopt_args_default_values()
{
    optarg = nullptr; // no args expect before begin parsing options (no options => no arg :) )
    optind = 1;       // skip argv[0] (name of executable file)
}

//---------------------------------------------------------------------------------------------------------------
