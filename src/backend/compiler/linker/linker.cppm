module;

#include <cstdlib>
#include <filesystem>
#include <vector>
#include <sstream>
#include <stdexcept>

export module paracl_linker;

import options_parser;

namespace ParaCL
{

export class Linker
{
    private:
        std::ostringstream link_command_;

    public:
        Linker(const Options::program_options_t& program_options);
        void link_objects_to_executable() const;

};

Linker::Linker(const Options::program_options_t& program_options)
{
    link_command_ << "clang ";

    for (const std::filesystem::path& obj_file: program_options.object_files)
        link_command_ << obj_file.string() << " ";

    link_command_ << "-o " << program_options.executable_file;
}

void Linker::link_objects_to_executable() const
{
    const int sys_result =  std::system(link_command_.str().c_str());
    if (sys_result == EXIT_SUCCESS) return;

    throw std::runtime_error("failed link object files");
}

} /* namespace ParaCL */

