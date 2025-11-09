# Сборка без юнит тестов
# cmake -S . -B build && cmake --build build

# Сборка с юнит тестами
# cmake -S . -B build -DBUILD_TESTING=ON && cmake --build build

# Сборка только юнит тестов
# cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTING=ON -DBUILD_EXECUTABLE=OFF && cmake --build build

# Запуск
# ./build/...

# Очистка
# cmake --build build --target clean

# Запуск ctest
# ctest --test-dir build -V

# Запуск Google tests
# ./build/geo_tests

# Запуск Google tests с фильтром
# ./build/geo_tests --gtest_filter="...*"

cmake_minimum_required(VERSION 3.17)
project(paraCL LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CXX_STANDARD "cxx_std_20")

set(EXEC_FILE_NAME "paracl")

include( FetchContent )

set(CMAKE_SETTINGS_DIR ${PROJECT_SOURCE_DIR}/cmake)
include(${CMAKE_SETTINGS_DIR}/debug.cmake)

find_program(FLEX_EXECUTABLE flex)
find_program(BISON_EXECUTABLE bison)

if (NOT FLEX_EXECUTABLE)
    message(STATUS "Fetching flex...")
    FetchContent_Declare(
        flex
        URL https://github.com/westes/flex/archive/refs/tags/v2.6.4.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(flex)
    find_package(flex REQUIRED)

else ()
    message(STATUS "Using system flex: ${FLEX_EXECUTABLE}")
endif()

if (NOT BISON_EXECUTABLE)
    message(STATUS "Fetching flex...")
    FetchContent_Declare (
        bison
        URL https://github.com/akimd/bison/archive/refs/tags/v3.8.2.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(bison)
    find_package(bison REQUIRED)

else ()
    message(STATUS "Using system bison: ${BISON_EXECUTABLE}")
endif()

add_custom_command(
    OUTPUT
        ${CMAKE_BINARY_DIR}/lexer.yy.cpp
    COMMAND
        ${FLEX_EXECUTABLE}
        -o ${CMAKE_BINARY_DIR}/lexer.yy.cpp
        ${CMAKE_SOURCE_DIR}/src/front/src/lexer.l
    DEPENDS
        ${CMAKE_SOURCE_DIR}/src/front/src/lexer.l
        ${CMAKE_BINARY_DIR}/parser.tab.hpp
    COMMENT
        "Generate lexer.yy.cpp with flex..."
)

add_custom_command(
    OUTPUT
        ${CMAKE_BINARY_DIR}/parser.tab.cpp
        ${CMAKE_BINARY_DIR}/parser.tab.hpp
    COMMAND
        ${BISON_EXECUTABLE}
        -o ${CMAKE_BINARY_DIR}/parser.tab.cpp
        ${CMAKE_SOURCE_DIR}/src/back/src/parser.y -d
    DEPENDS
        ${CMAKE_SOURCE_DIR}/src/back/src/parser.y
    COMMENT
        "Generate parser.tab.cpp(hpp) with bison..."
)

add_custom_target(
    generate_lexer
    DEPENDS ${CMAKE_BINARY_DIR}/lexer.yy.cpp
)

add_custom_target(
    generate_parser
    DEPENDS 
        ${CMAKE_BINARY_DIR}/parser.tab.cpp 
        ${CMAKE_BINARY_DIR}/parser.tab.hpp
)


set(SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/src/main.cpp
)

set(INCLUDE_DIRECTORIES
    ${CMAKE_SOURCE_DIR}/src/front/inc
    ${CMAKE_BINARY_DIR}
)

set(SANITIZER "-fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr")

set(DEBUG_FLAGS "-Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=81920 -Wstack-usage=81920 -pie -fPIE -Werror=vla")

option(BUILD_EXECUTABLE "Build executable" ON)
option(BUILD_TESTING "Build tests" OFF)

option(SANITIZE "Enable compiler sanitizers" OFF)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type selected, defaulting to Debug")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Choose the type of build.")
endif()

if(SANITIZE)

    message(STATUS "Sanitizers enabled")
    list(APPEND DEBUG_FLAGS ${SANITIZER})

endif()

# ----------------------------------------------------------------------------------------

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

if(BUILD_EXECUTABLE)
    add_executable( ${EXEC_FILE_NAME} ${SOURCE_FILES} )
    target_hard_debug_options(${EXEC_FILE_NAME})

    target_sources(${EXEC_FILE_NAME} PRIVATE 
        ${CMAKE_BINARY_DIR}/parser.tab.cpp
        ${CMAKE_BINARY_DIR}/lexer.yy.cpp
    )
    target_include_directories( ${EXEC_FILE_NAME} PRIVATE ${INCLUDE_DIRECTORIES} )
    
    target_compile_features( ${EXEC_FILE_NAME} PRIVATE ${CXX_STANDARD} )

    add_dependencies( ${EXEC_FILE_NAME} generate_parser generate_lexer )

    if (CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_options( ${EXEC_FILE_NAME} PRIVATE ${DEBUG_FLAGS} )
    endif()

endif()

