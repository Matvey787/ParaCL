/*
!!!!!!!!!!!!!!WARNINNG!!!!!!!!!!!!!!!!!!!!!!!!
==============USING GLOBAL VARIABLES======================
'optarg' and 'optind' - is a global variables from getopt.h
their declaration there:
extern char *optarg;
extern int optind;
*/
module;

//---------------------------------------------------------------------------------------------------------------

#include "global/custom_console_output.hpp"
#include "global/global.hpp"
#include "log/log_api.hpp"
#include <bits/unique_ptr.h>
#include <cassert>
#include <concepts>
#include <cstdlib>
#include <filesystem>
#include <getopt.h>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <unistd.h>
#include <vector>

//---------------------------------------------------------------------------------------------------------------

export module options;

//---------------------------------------------------------------------------------------------------------------

import paracl_info;
// export import options;

//---------------------------------------------------------------------------------------------------------------

namespace ParaCL
{
namespace options
{
//---------------------------------------------------------------------------------------------------------------

void set_getopt_args_default_values();

//---------------------------------------------------------------------------------------------------------------

export enum class optimize_level_t
{
    O0,
    O1,
    O2,
    O3,
};

//---------------------------------------------------------------------------------------------------------------

using file = std::filesystem::path;

//---------------------------------------------------------------------------------------------------------------

export template <typename Options>
concept IOptions = std::is_constructible_v<Options, int, char* []> and
    requires(const Options op, /*Options &&op_rref,*/ int argc, char *argv[], size_t i)
    {
        { op.optimize_level() } -> std::same_as<optimize_level_t>;
        { op.compile() } -> std::convertible_to<bool>;
        { op.input_files_quant() } -> std::convertible_to<size_t>;
        { op.executable() } -> std::convertible_to<const file &>;
        // { std::move(op_rref).executable() } -> std::convertible_to<file>; /* dont need in singleton */
        { op.sources() } -> std::convertible_to<const std::vector<file> &>;
        // { std::move(op_rref).sources() } -> std::convertible_to<std::vector<file>>; /* dont need in singleton */

        { op.i_source(i) } -> std::convertible_to<const file &>;
        { op.i_source_fast_unsafe(i) } -> std::convertible_to<const file &>;
        // { std::move(op_rref).i_source(i) } -> std::convertible_to<file>; /* dont need in singleton */
        // { std::move(op_rref).i_source_fast_unsafe(i) } noexcept -> std::convertible_to<file>; /* dont need in singleton
        // */

        { op.objects() } -> std::convertible_to<const std::vector<file> &>;
        // { std::move(op_rref).objects() } -> std::convertible_to<std::vector<file>>; /* dont need in singleton */
        { op.i_object(i) } -> std::convertible_to<const file &>;
        { op.i_object_fast_unsafe(i) } -> std::convertible_to<const file &>;
        // { std::move(op_rref).i_object(i) } -> std::convertible_to<file>; /* dont need in singleton */
        // { std::move(op_rref).i_object_fast_unsafe(i) } noexcept -> std::convertible_to<file>; /* dont need in singleton
        // */

        ON_GRAPHVIZ(
            { op.ast_dump() } -> std::convertible_to<bool>; { op.dot_file() } -> std::convertible_to<const file &>;
            // { std::move(op_rref).dot_file() } -> std::convertible_to<file>;  /* dont need in singleton */
        )
};


//---------------------------------------------------------------------------------------------------------------

export class Options
{
private:
    std::vector<file> sources_;
    std::vector<file> objects_;
    ON_GRAPHVIZ(file dot_file_;)
    file executable_file_{"a.out"};
    optimize_level_t optimize_level_ = optimize_level_t::O0;
    ON_GRAPHVIZ(bool ast_dump_ : 1 = false;)
    bool compile_ : 1 = false;

    [[noreturn]]
    void parse_flag_help() const;
    [[noreturn]]
    void parse_flag_version() const;
    ON_GRAPHVIZ(void parse_flag_ast_dump();)
    void parse_flag_compile();
    void parse_flag_output();
    void parse_flag_optimize();
    void parse_not_a_flag(const char *arg);
    [[noreturn]]
    void undefined_option(const char *option) const;
    void set_out_files();

public:
    explicit Options(int argc, char *argv[]);

    optimize_level_t optimize_level() const
    {
        return optimize_level_;
    }
    bool compile() const
    {
        return compile_;
    }
    const file &executable() const &
    {
        return executable_file_;
    }
    // file executable() const && { return executable_file_; } /* dont need in singleton */

    size_t input_files_quant() const
    {
        return sources_.size();
    }

    const file &i_source(size_t i) const &
    {
        if (i > sources_.size())
            throw std::out_of_range("try to get no existence source");
        return sources_[i];
    }
    const file &i_source_fast_unsafe(size_t i) const &
    {
        return sources_[i];
    }
    // file i_source(size_t i) const && { if (i > sources_.size()) throw std::runtime_error("try to get no existence
    // source"); return sources_[i]; } /* dont need in singleton */ file i_source_fast_unsafe(size_t i) && noexcept
    // const { return sources_[i]; } /* dont need in singleton */

    const file &i_object(size_t i) const &
    {
        if (i > objects_.size())
            throw std::out_of_range("try to get no existence object");
        return objects_[i];
    }
    const file &i_object_fast_unsafe(size_t i) const &
    {
        return objects_[i];
    }
    // file i_object(size_t i) const && { if (i > sources_.size()) throw std::runtime_error("try to get no existence
    // source"); return objects_[i]; } /* dont need in singleton */ file i_object_fast_unsafe(size_t i) && noexcept
    // const { return objects_[i]; } /* dont need in singleton */

    const std::vector<file> &sources() const
    {
        return sources_;
    }
    const std::vector<file> &objects() const &
    {
        return objects_;
    }
    // std::vector<file> sources() const && { return sources_; } /* dont need in singleton */
    // std::vector<file> objects() const && { return objects_; } /* dont need in singleton */

    ON_GRAPHVIZ(
    bool ast_dump() const { return ast_dump_; } const file &dot_file() const & { return dot_file_; }
    // file dot_file() const && { return dotf_file_; }  /* dont need in singleton */
    )
};

//---------------------------------------------------------------------------------------------------------------

static_assert(IOptions<Options>, "class Options must realized IOptionsParser interface, "
                                 "because now it`s the only one realizatoin of this interaface, "
                                 "and it`s using in other modules.");

//---------------------------------------------------------------------------------------------------------------

enum flag_key
{
    help = 'h',
    version = 'v',
    compile = 'c',
    output = 'o',
    optimize = 'O',
    show_ir = 'i',
    ON_GRAPHVIZ(ast_dump = 'd', ) undefined_option_key = '?',
    end_of_parsing = -1
};

//---------------------------------------------------------------------------------------------------------------

constexpr option long_options[] = {
    {"help", no_argument, 0, help},
    {"version", no_argument, 0, version},
    {"Optimize", required_argument, 0, optimize},
    ON_GRAPHVIZ({"ast-dump", required_argument, 0, ast_dump}, ){"compile", no_argument, 0, compile},
    {"output", required_argument, 0, output},
    {"show-ir", no_argument, 0, show_ir},
    {0, 0, 0, 0}};

//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// class public methods
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

Options::Options(int argc, char *argv[])
{
    LOGINFO("paracl: options parser: begin parse options");

    LOGINFO("argc = {}", argc);

    ON_LOGGER(for (int i = 0; i < argc; ++i))
    LOGINFO("argv[{}] = \"{}\"", i, argv[i] ? argv[i] : "NULL");

    int option;
    while ((option = getopt_long(argc, argv, "hvd:co:i:", long_options, nullptr)) != end_of_parsing)
    {
        LOGINFO("paracl: options parser: processing option '{}', optarg = '{}'", static_cast<char>(option),
                optarg ? optarg : "NULL");

        switch (option)
        {
        case flag_key::help:
            parse_flag_help();
            break;
        case flag_key::version:
            parse_flag_version();
            break;
        case flag_key::compile:
            parse_flag_compile();
            break;
        case flag_key::optimize:
            parse_flag_optimize();
            break;
        case flag_key::output:
            parse_flag_output();
            break;
            ON_GRAPHVIZ(case flag_key::ast_dump : parse_flag_ast_dump(); break;) /* ON_GRAPHVIZ */
        case flag_key::undefined_option_key:
            throw std::invalid_argument("invalid option");
        default:
            undefined_option(argv[optind - 1]);
            break;
        }
    }

    for (int i = optind; i < argc; i++)
        parse_not_a_flag(argv[i]);

    set_getopt_args_default_values();

    if (sources_.empty())
        throw std::invalid_argument("no source files specified");

    set_out_files();

    LOGINFO("paracl: options parser: parse options completed");
}

// class private methods
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

[[noreturn]]
void Options::parse_flag_help() const
{
    LOGINFO("paracl: options parser: find '--help' flag");

    std::cout << BOLD "There are all flags and parametrs:" RESET_CONSOLE_OUT << R"(
    -h --help
    -v --version
    -d --ast-dump=
    -c --compile
    -o --output=
    -O --Optimize=

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

    -O, --Optimize=
        Optimize level in compiler. Exists 4 values: 0, 1, 2, 3
    
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
void Options::parse_flag_version() const
{
    LOGINFO("paracl: options parser: find '--version' flag");

    const ParaCL::ParaCLInfo &paracl_info = ParaCL::ParaCLInfo::queryParaCLInfo();

    std::cout << "ParaCL\n"
                 "Version     : "
              << paracl_info.version
              << "\n"
                 "Build at    : "
              << paracl_info.build_date
              << "\n"
                 "Build type  : "
              << paracl_info.build_type
              << "\n"
                 "Build with  : "
              << paracl_info.compiler
              << "\n"
                 "Commit hash : "
              << paracl_info.git_commit
              << "\n"
                 "Architecture: "
              << paracl_info.architecture << std::endl;

    LOGINFO("paracl: exit success");
    exit(EXIT_SUCCESS);
}

//---------------------------------------------------------------------------------------------------------------

ON_GRAPHVIZ(void Options::parse_flag_ast_dump() {
    LOGINFO("paracl: options parser: --ast-dump={}", optarg);
    msg_assert(optarg, "Expected filename for --ast-dump");
    ast_dump_ = true;
    dot_file_ = optarg;
}) /* ON_GRAPHVIZ */

//---------------------------------------------------------------------------------------------------------------

void Options::parse_flag_compile()
{
    LOGINFO("paracl: options parser: --compile");
    compile_ = true;
}

//---------------------------------------------------------------------------------------------------------------

void Options::parse_flag_output()
{
    msg_assert(optarg, "Expected filename for --output");
    LOGINFO("paracl: options parser: --output={}", optarg);
    executable_file_ = optarg;
}

//---------------------------------------------------------------------------------------------------------------

void Options::parse_flag_optimize()
{
    msg_assert(optarg, "Expected ompimize level for -O");
    LOGINFO("paracl: options parser: -O{}", optarg);

    const std::string optimize_level_str = optarg;

    if (optimize_level_str == "0")
        optimize_level_ = optimize_level_t::O0;
    else if (optimize_level_str == "1")
        optimize_level_ = optimize_level_t::O1;
    else if (optimize_level_str == "2")
        optimize_level_ = optimize_level_t::O2;
    else if (optimize_level_str == "3")
        optimize_level_ = optimize_level_t::O3;
    else
        throw std::invalid_argument("bad optimization level: -O" + optimize_level_str);
}

//---------------------------------------------------------------------------------------------------------------

void Options::parse_not_a_flag(const char *arg)
{
    LOGINFO("paracl: options parser: find object file: \"{}\"", arg);
    LOGINFO("paracl: options parser: parse not a flag: \"{}\"", arg);

    file file(arg);
    std::string extension(file.extension().string());

    std::string_view paracl_extension = ParaCLInfo::queryParaCLInfo().extension;

    if (extension == paracl_extension)
    {
        LOGINFO("paracl: options parser: find source file: \"{}\"", arg);
        sources_.push_back(std::string(arg));
        return;
    }

    if (extension == ".o")
    {
        objects_.push_back(file);
        return;
    }

    LOGERR("paracl: options parser: unexpected file: \"{}\"", arg);
    throw std::invalid_argument("bad source file: unexpected extension: '" + std::string(arg) +
                                "', expected '.cl' - for paracl src, '.ll' - for llvm ir, or '.o' - for object file");
}

//---------------------------------------------------------------------------------------------------------------

[[noreturn]]
void Options::undefined_option(const char *option) const
{
    LOGERR("paracl: options parser: undefined option \"{}\"", option);
    throw std::invalid_argument("Undefined option: '" + std::string(option) + "'");
}

//---------------------------------------------------------------------------------------------------------------

void Options::set_out_files()
{
    LOGINFO("paracl: options parser: set not-seted obj files default values");

    const size_t sources_size = sources_.size();
    const size_t object_files_size = objects_.size();

    if (object_files_size > sources_size)
        throw std::invalid_argument("there are more objects files, then sources files");

    objects_.resize(sources_size);

    for (size_t it = objects_.size(), ite = sources_size; it < ite; ++it)
    {
        file &object_file = objects_[it];

        object_file = sources_[it];
        object_file.replace_extension(".o");
    }
}

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

} /* namespace options */
} /* namespace ParaCL */

//---------------------------------------------------------------------------------------------------------------
