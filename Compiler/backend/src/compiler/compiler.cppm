module;

#include <filesystem>
#include <sstream>
#include <cstdlib>
#include <iostream>

export module compiler;

import llvm_ir_translator;

namespace compiler
{
export void compile([[maybe_unused]] std::filesystem::path const & ast_json, std::filesystem::path const & executable)
{
    auto&& tmp_ir_file = std::filesystem::path{executable};
    tmp_ir_file.replace_extension(".ll");

    llvm_ir_translator::generate_llvm_ir(ast_json, tmp_ir_file);

    auto&& compile_commmand = std::ostringstream{};
    compile_commmand << "clang++ -O3 " << tmp_ir_file.string() << " -o " << executable.string() << " 2>/dev/null";

    auto&& compile_command_exit_code = std::system(compile_commmand.str().c_str());

    if (compile_command_exit_code == EXIT_SUCCESS) return;

    throw std::runtime_error("Failed generate '" + executable.string() + "' with exit code " + std::to_string(compile_command_exit_code));
}

} /* namespace compiler */
