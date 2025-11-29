module;

#include <cstdlib>
#include <stdexcept>

#include "parser.tab.hpp"
#include "parser/parser.hpp"

#include "global/custom_console_output.hpp"
#include "global/global.hpp"

extern void set_current_paracl_file(const std::string &);

extern FILE *yyin;
extern ParaCL::ProgramAST program;


#if defined(GRAPHVIZ)
#include <iostream>
#endif /* defined(GRAPHVIZ) */

export module paracl_toolchain;

import options_parser;
import paracl_interpreter;
import paracl_llvm_ir_translator;
import paracl_linker;
ON_GRAPHVIZ(import ast_graph_dump;)

namespace ParaCL
{

export class Toolchain
{
private:
    const Options::program_options_t& program_options_;
    mutable ProgramAST ast_;

    void read_ast(size_t source_num) const;
    void compile() const;
    void interpret() const;

    void create_objects_files() const;
    void link_objects_to_executable() const;

    ON_GRAPHVIZ(
    void ast_dump() const;
    )

public:
    Toolchain(const Options::program_options_t& program_options);
    void run_paracl() const;

};

Toolchain::Toolchain(const Options::program_options_t& program_options) :
program_options_(program_options)
{}

void Toolchain::run_paracl() const
{
    ON_GRAPHVIZ(
    if (program_options_.ast_dump)
        ast_dump();
    ) /* ON_GRAPHVIZ */

    (program_options_.compile) ?
        compile() : interpret();
}

void Toolchain::interpret() const
{
    if (program_options_.sources.size() != 1)
        throw std::invalid_argument("for interpreter expect only 1 input file");

    read_ast(0); /* set ast_ */

    Interpreter interpreter(ast_);
    interpreter.interpret();
}

void Toolchain::compile() const
{
    create_objects_files();
    link_objects_to_executable();
}

void Toolchain::create_objects_files() const
{
    for (size_t it = 0, ite = program_options_.sources.size(); it != ite; ++it)
    {
        LLVMIRBuilder llvm_builder(program_options_, it);
        llvm_builder.generate_ir(ast_);
        llvm_builder.compile_ir();
    }
}

void Toolchain::link_objects_to_executable() const
{
    Linker linker(program_options_);
    linker.link_objects_to_executable();
}

void Toolchain::read_ast(size_t source_num) const
{
    msg_assert(source_num < program_options_.sources.size(), "try to get no existence source: " << source_num);

    std::string source = program_options_.sources[source_num].string();
    set_current_paracl_file(source);

    yyin = fopen(source.c_str(), "rb");

    if (not yyin)
        throw std::invalid_argument(RED BOLD "no such file: " RESET_CONSOLE_OUT WHITE + source);

    yy::parser parser;
    int result = parser.parse();

    if (result != EXIT_SUCCESS)
        throw std::runtime_error("failed parsie '" + source + "' with exit code: " + std::to_string(result));

    /* program - is a global variable */
    ast_ = std::move(program);
}

ON_GRAPHVIZ(
void Toolchain::ast_dump() const
{   
    if (program_options_.ast_dump) try {
        ::ParaCL::ast_dump(ast_, program_options_.dot_file.string());
    } catch (const std::runtime_error &e) {
        std::cerr << ERROR_MSG("graphviz ast dump failed:\n") << e.what() << "\n";
    } catch (...) {
        std::cerr << ERROR_MSG("graphviz ast dump failed with unknow exception.\n");
    }
}
) /* ON_GRAPHVIZ */

} /* namespace ParaCL */
