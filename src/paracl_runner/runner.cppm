module;

#include <concepts>
#include <type_traits>

export module runner;

export import options;
export import ast_builder;
export import toolchain;
export import interpreter;

namespace ParaCL
{
namespace runner
{

export template <typename Runner>
concept IRunner = std::is_constructible<Runner, const options::Options &> && requires(Runner r) {
    { r.run() } -> std::same_as<void>;
};

using backend::interpreter::IInterpreter;
using backend::toolchain ::IToolchain;
using frontend::ast_builder::IASTBuilder;

export template <IASTBuilder ASTBuilder = frontend::ast_builder::ASTBuilder,
                 IInterpreter Interpreter = backend::interpreter::Interpreter,
                 IToolchain Toolchain = backend::toolchain::Toolchain>
class Runner final
{
  private:
    const options::Options &program_options_;
    ProgramAST ast_;
    void read_ast(const file &source)
    {
        ast_ = ASTBuilder(source).read_ast();
    }

    void run_interpreter() const
    {
        const options::interpreter::InterpreterOptions opt{program_options_};
        Interpreter{opt}.interpret();
    }

    void run_toolchain() const
    {
        const options::compiler::CompilerOptions opt{program_options_};
        Toolchain{opt}.create_executable();
    }

    Runner(const options::Options &program_options) : program_options_(program_options)
    {
    }

    void select_run_way_and_run()
    {
        (program_options_.compile) ? (run_toolchain()) : (run_interpreter());
    }

  public:
    void run()
    {
    }
};

void Runner::run()

} /* namespace runner */
} /* namespace ParaCL */
