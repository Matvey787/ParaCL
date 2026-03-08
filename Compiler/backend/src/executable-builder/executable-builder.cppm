module;

#include <filesystem>
#include <cstdlib>
#include <stdexcept>
#include <sstream>
#include <string>

export module executable_builder;

namespace compiler::executable_builder
{

export
void build_executable(std::filesystem::path const & llvm_ir_file, std::filesystem::path const & executable)
{
    if (llvm_ir_file.extension() != ".ll")
        throw std::invalid_argument(__PRETTY_FUNCTION__);

    auto&& compile_command = std::ostringstream{};
    compile_command << LLVM_IR_COMPILER << " -O3 " << llvm_ir_file.string() << "-o " << executable.string();

    auto&& compile_command_return_value = std::system(compile_command.str().c_str());

    if (compile_command_return_value == EXIT_SUCCESS)
        return;

    throw std::runtime_error("Failed compile '" + llvm_ir_file.string() + "' with exit code: " + std::to_string(compile_command_return_value));
}


} /* namespace compiler::executable_builder */

