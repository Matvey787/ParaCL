module;

#include <cstdlib>
#include <filesystem>
#include <sstream>
#include <stdexcept>
#include <vector>

export module paracl_linker;

import options_parser;

namespace ParaCL
{
namespace backend
{
namespace toolchain
{
namespace linker
{

export class Linker final
{
  private:
    std::ostringstream link_command_;

  public:
    Linker(const Options::program_options_t &options_parser);
    void link_objects_to_executable() const;
};

Linker::Linker(const Options::program_options_t &options_parser)
{
    link_command_ << "clang ";

    for (const std::filesystem::path &obj_file : options_parser.object_files)
        link_command_ << obj_file.string() << " ";

    link_command_ << "-o " << options_parser.executable_file;
}

void Linker::link_objects_to_executable() const
{
    const int sys_result = std::system(link_command_.str().c_str());
    if (sys_result == EXIT_SUCCESS)
        return;

    throw std::runtime_error("failed link object files");
}

} /* namespace linker */
} /* namespace toolchain */
} /* namespace backend */
} /* namespace ParaCL */
