list(LENGTH AUTHORS AUTHORS_COUNT)

set(AUTHORS_LIST_CPP "\t\t")
foreach(AUTHOR_ENTRY IN LISTS AUTHORS)
    string(REPLACE "|" ";" TEMP_LIST "${AUTHOR_ENTRY}")
    
    list(GET TEMP_LIST 0 NAME)
    list(GET TEMP_LIST 1 GITLINK)
    string(APPEND AUTHORS_LIST_CPP "Author{\"${NAME}\", \"${GITLINK}\"},\n\t\t")
endforeach()

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/projInfo.hpp.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/projInfo.hpp"
    @ONLY
)

configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/projInfo.hpp.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/projInfo.hpp"
    @ONLY
)