module;

//---------------------------------------------------------------------------------------------------------------

#if defined(LOGGER)
#include "pineaplog.hpp"
#endif /* defined(LOGGER) */

#include <getopt.h>
#include <unistd.h>
#include <cstdlib>
#include <cassert>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include <stdexcept>

#include "global/global.hpp"
#include "global/custom_console_output.hpp"

//---------------------------------------------------------------------------------------------------------------

export module options_parser;

//---------------------------------------------------------------------------------------------------------------

import paracl_extension;

//---------------------------------------------------------------------------------------------------------------

void set_getopt_args_default_values();

//---------------------------------------------------------------------------------------------------------------


export namespace OptionsParsing
{
//---------------------------------------------------------------------------------------------------------------

struct program_options_t
{
    std::string              program_name = "";
    bool                     help : 1 = false;
    std::vector<std::string> sources;
};

//---------------------------------------------------------------------------------------------------------------
} /* namespace OptionsParsing */
//---------------------------------------------------------------------------------------------------------------

class options_parser
{
    public:
        OptionsParsing::program_options_t get_program_options() const;

        options_parser() = delete;
        options_parser(int argc, char* argv[]);

    private:
        OptionsParsing::program_options_t program_options_;

        void set_program_name         (const char* argv0);

        [[noreturn]]        
        void parse_flag_help          () const;
        [[noreturn]]
        void parse_flag_version       () const;

        void parse_not_a_flag         (const char* argument);

        [[noreturn]]
        void undefined_option         (const char* argument) const;
};

//---------------------------------------------------------------------------------------------------------------

export namespace OptionsParsing
{
//---------------------------------------------------------------------------------------------------------------

program_options_t parse_program_options(int argc, char* argv[])
{
    options_parser result = options_parser(argc, argv);
    return options_parser(argc, argv).get_program_options();
}

//---------------------------------------------------------------------------------------------------------------
} /* export namespace OptionsParsing */
//---------------------------------------------------------------------------------------------------------------

constexpr option long_options[] =
{
    {"help"   , no_argument, 0, 'h'},
    {"version", no_argument, 0, 'v'},
    {""       , 0          , 0,  0 }, /* just for safety */
};


//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// class public methods
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

options_parser::options_parser(int argc, char* argv[]) :
program_options_()
{
    set_program_name(argv[0]);

    static constexpr int undefined_option_key = -1;

    for (int options_iterator = 1; options_iterator < argc; options_iterator++)
    {
        int option = getopt_long(argc, argv, "hv", long_options, nullptr);
    
        switch (option)
        {
            case 'h'                 : parse_flag_help   ();                        continue;
            case 'v'                 : parse_flag_version();                        continue;
            case undefined_option_key: parse_not_a_flag  (argv[options_iterator]); continue;
            default                  : undefined_option  (argv[options_iterator]); continue;
        }
    }

    set_getopt_args_default_values();
}

//---------------------------------------------------------------------------------------------------------------

OptionsParsing::program_options_t options_parser::get_program_options() const
{
    return program_options_;
}

// class private methods
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

void options_parser::set_program_name(const char* argv0)
{
    msg_assert(argv0, "argv[0] is nullptr? are you sure, that you give here arg[0]?");
    program_options_.program_name = std::string(argv0);
}

//---------------------------------------------------------------------------------------------------------------

[[noreturn]]
void options_parser::parse_flag_help() const
{
    std::cout <<
    BOLD
    "There are all flags and parametrs:"
    RESET_CONSOLE_OUT
    << R"(
    -h --help
    -v --version

        *.cl
            to give source file, just write *.cl file in command.

        -h, --help
            these flags show information about all flags and their functions.
    
        -v, --version
            use it to show paracl version, that installed on your device
    
    So, that was all, what I know about flags in this program.
    Good luck, I love you )"
    << HEART << std::endl;

    exit(EXIT_SUCCESS); // good exit :)
}

//---------------------------------------------------------------------------------------------------------------

[[noreturn]]
void options_parser::parse_flag_version() const
{
#if not defined(PARACL_VERSION)
#error "ParaCL version is unknowed."
#endif /* not defined(PARACL_VERSION) */
    std::cout << "ParaCL " PARACL_VERSION << std::endl;


    exit(EXIT_SUCCESS); // good exit :)
}

//---------------------------------------------------------------------------------------------------------------

void options_parser::parse_not_a_flag(const char* argument)
{
    if (ParaCL::is_paracl_file_name(argument))
        return program_options_.sources.push_back(std::string(argument));   

    throw std::invalid_argument("bad source file: unexpected extentsion: '" WHITE + std::string(argument) + RESET_CONSOLE_OUT "', expect " + ParaCL::paracl_extension);
}

//---------------------------------------------------------------------------------------------------------------

[[noreturn]]
void options_parser::undefined_option(const char* argument) const
{
    assert(argument);

    throw std::invalid_argument("Undefined option: '" + std::string(argument) + "'");
}

//---------------------------------------------------------------------------------------------------------------

void set_getopt_args_default_values()
{
    optarg = nullptr;
    optind = 1;
}

//---------------------------------------------------------------------------------------------------------------

