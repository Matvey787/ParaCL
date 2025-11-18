module;

//---------------------------------------------------------------------------------------------------------------

#include <string>
#include <filesystem>

#if defined(LOGGER)
#include "pineaplog.hpp"
#endif /* defined(USE_LOGGER) */

//---------------------------------------------------------------------------------------------------------------

export module paracl_extension;

//---------------------------------------------------------------------------------------------------------------

export namespace ParaCL
{
//---------------------------------------------------------------------------------------------------------------

const std::string paracl_extension = ".cl";

//---------------------------------------------------------------------------------------------------------------

bool is_paracl_file_name(const std::string& file)
{
    std::filesystem::path file_(file);
    return file_.extension().string() == paracl_extension;
}

//---------------------------------------------------------------------------------------------------------------

bool is_paracl_file_name(std::string_view file)
{
    std::filesystem::path file_(file);
    return file_.extension().string() == paracl_extension;
}

//---------------------------------------------------------------------------------------------------------------

bool is_paracl_file_name(const char* file)
{
    std::filesystem::path file_(file);
    return file_.extension().string() == paracl_extension;
}

//---------------------------------------------------------------------------------------------------------------

}; /* export namespace ParaCLExtension */

//---------------------------------------------------------------------------------------------------------------
