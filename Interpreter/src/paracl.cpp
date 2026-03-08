#if not defined(PARACL_FRONT)
#error "Please define 'PARACL_FRONT' for this unit."
#endif /* not defined(PARACL_FRONT) */

#if not defined(PARACL_INTERPRETER)
#error "Please define 'PARACL_INTERPRETER' for this unit."
#endif /* not defined(PARACL_INTERPRETER) */

#include <sstream>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <filesystem>
#include <string>
#include <cstdlib>

int main(int argc, char* argv[]) try
{
    if (argc != 2)
        throw std::invalid_argument("Usage:\n" + std::string(argv[0]) + " <source>.cl [-o executable]");

    auto&& source = std::filesystem::path{argv[1]};
    auto&& frontend_command = std::ostringstream{};
    frontend_command << PARACL_FRONT " " << source.string() << " -o " << source.replace_extension(".ast.json");

    auto&& frontend_exit_code = std::system(frontend_command.str().c_str());
    if (frontend_exit_code != EXIT_SUCCESS)
        throw std::runtime_error("Fronted failed with exit code " + std::to_string(frontend_exit_code));

    auto&& intepreter_command = std::ostringstream{};
    intepreter_command << PARACL_INTERPRETER " " << source.string();

    auto&& intepreter_exit_code = std::system(intepreter_command.str().c_str());
    if (intepreter_exit_code != EXIT_SUCCESS)
        throw std::runtime_error("Intepretation failed with exit code " + std::to_string(intepreter_exit_code));

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
