#if not defined(PARACL_FRONT)
#error "Please define 'PARACL_FRONT' for this unit."
#endif /* not defined(PARACL_FRONT) */

#if not defined(PARACL_COMPILER)
#error "Please define 'PARACL_COMPILER' for this unit."
#endif /* not defined(PARACL_COMPILER) */

#include <sstream>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <filesystem>
#include <string>
#include <cstdlib>

int main(int argc, char* argv[]) try
{
    if (argc <= 1 or argc == 3 or argc >= 5)
        throw std::invalid_argument("Usage:\n" + std::string(argv[0]) + " <source>.cl [-o executable]");

    auto&& executable = (argc == 4) ? std::filesystem::path{argv[3]} : std::filesystem::path{"a.out"};
    std::filesystem::path tmp_ast_json = executable;
    tmp_ast_json.replace_extension(".ast.json");

    auto&& source = std::filesystem::path{argv[1]};
    auto&& frontend_command = std::ostringstream{};

    frontend_command << PARACL_FRONT " " << source.string() << " -o " << tmp_ast_json.string();

    auto&& frontend_exit_code = std::system(frontend_command.str().c_str());
    if (frontend_exit_code != EXIT_SUCCESS)
        throw std::runtime_error("Fronted failed with exit code " + std::to_string(frontend_exit_code));

    auto&& compiler_command = std::ostringstream{};
    compiler_command << PARACL_COMPILER " " << tmp_ast_json.string() << " -o " << executable.string();

    auto&& compiler_exit_code = std::system(compiler_command.str().c_str());
    if (compiler_exit_code != EXIT_SUCCESS)
        throw std::runtime_error("Compilation failed with exit code " + std::to_string(compiler_exit_code));

    return EXIT_SUCCESS;
}
catch (std::exception const & e)
{
    std::cerr << "Exception catched: " << e.what() << "\n";
    return EXIT_FAILURE;
}
catch(...)
{
    std::cerr << "Undefined exception.\n";
    return EXIT_FAILURE;
}
