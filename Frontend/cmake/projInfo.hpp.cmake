#pragma once

#include <string_view>
#include <array>

namespace ParaCL::general::projInfo
{
    class Author
    {
        public:
        
        std::string_view name;
        std::string_view gitLink;
    };

    constexpr std::string_view ParaCLVersion = "@PROJECT_VERSION@";

    constexpr std::array<Author, @AUTHORS_COUNT@> authors = {
@AUTHORS_LIST_CPP@};
}