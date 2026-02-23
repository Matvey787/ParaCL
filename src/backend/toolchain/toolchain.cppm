module;

#include <concepts>
#include <cstdlib>
#include <filesystem>
#include <stdexcept>
#include <type_traits>

#include "parser.tab.hpp"
#include "parser/parser.hpp"

#include "global/custom_console_output.hpp"
#include "global/global.hpp"

extern void set_current_paracl_file(std::string_view);

extern FILE *yyin;
extern ParaCL::ProgramAST program;

#if defined(GRAPHVIZ)
#include <iostream>
#endif /* defined(GRAPHVIZ) */

export module toolchain;

export import compiler_options;
import paracl_llvm_ir_translator;
import paracl_linker;
import ast_builder;

ON_GRAPHVIZ(import ast_graph_dump;)
ON_DEBUG(import exceptoin_stacktrace;)

namespace ParaCL
{
namespace backend
{
namespace toolchain
{

export template <typename Toolchain>
concept IToolchain =
    std::is_constructible<Toolchain, const options::compiler::CompilerOptions &> && requires(Toolchain t) {
        { t.create_executable() } -> std::same_as<void>;
    };

export class Toolchain
{
  private:
    const options::compiler::CompilerOptions &program_options_;
    mutable ProgramAST ast_;

    void read_ast(const std::filesystem::path &source) const;
    void compile() const;

    void create_objects_files() const;
    void link_objects_to_executable() const;

    ON_GRAPHVIZ(void ast_dump() const;)

  public:
    Toolchain(const options::compiler::CompilerOptions &options);
    void create_executable() const;
};

Toolchain::Toolchain(const options::compiler::CompilerOptions &options) : program_options_(options)
{
}

void Toolchain::create_executable() const
{
    create_objects_files();
    link_objects_to_executable();
}

void Toolchain::create_objects_files() const
{
    for (size_t it = 0, ite = program_options_.sources.size(); it != ite; ++it)
    {
        const std::filesystem::path &source = program_options_.sources[it];
        set_current_paracl_file(source.string());
        read_ast(source);

        ON_GRAPHVIZ(if (program_options_.ast_dump) ast_dump();) /* ON_GRAPHVIZ */

        LLVMIRBuilder llvm_builder(program_options_, it);
        llvm_builder.generate_ir(ast_);
        llvm_builder.compile_ir();
    }
}

tvoid Toolchain::link_objects_to_executable() const
{
    Linker linker(program_options_);
    linker.link_objects_to_executable();
}

void Toolchain::read_ast(const std::filesystem::path &source) const
{
    /* yyin is global bison variable */
    yyin = std::fopen(source.string().c_str(), "rb");

    if (not yyin)
        throw std::invalid_argument(RED BOLD "no such file: " RESET_CONSOLE_OUT WHITE + source.string());

    // yy::parser parser;
    int result = yy::parser().parse();

    if (result != EXIT_SUCCESS)
        throw std::runtime_error("failed parse '" + source.string() + "' with exit code: " + std::to_string(result));

    /* program - is a global bison-user variable */
    ast_ = std::move(program);
}

} /* namespace toolchain */

} /* namespace backend */
} /* namespace ParaCL */
