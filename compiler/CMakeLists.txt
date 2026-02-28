# =============================================================================
# front/CMakeLists.txt
# =============================================================================

cmake_minimum_required(VERSION 3.28)
project(ParaCL_Front LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Подключаем проверки инструментов
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/deps/check-bison.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/deps/check-flex.cmake)

# ====================== PATHS ======================
set(FRONT_SRC_DIR      ${CMAKE_CURRENT_SOURCE_DIR})

# ====================== BISON (Parser) ======================
set(PARSER_SRC_DIR          ${FRONT_SRC_DIR}/parser/src)
set(BISON_PARSER_CPP_OUT    parser.tab.cpp)
set(BISON_PARSER_HPP_OUT    parser.tab.hpp)
set(BISON_INPUT_FILE        ${PARSER_SRC_DIR}/parser.y)
set(BISON_OUTPUT_DIR        ${CMAKE_BINARY_DIR}/generated/bison)

add_custom_command(
    OUTPUT
        ${BISON_OUTPUT_DIR}/${BISON_PARSER_CPP_OUT}
        ${BISON_OUTPUT_DIR}/${BISON_PARSER_HPP_OUT}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${BISON_OUTPUT_DIR}
    COMMAND ${BISON_EXECUTABLE}
        -d
        -o ${BISON_OUTPUT_DIR}/${BISON_PARSER_CPP_OUT}
        ${BISON_INPUT_FILE}
    DEPENDS ${BISON_INPUT_FILE}
    COMMENT "Generating Bison parser (${BISON_PARSER_CPP_OUT})"
    VERBATIM
)

add_library(parser STATIC)

target_sources(parser
    PRIVATE
        ${BISON_OUTPUT_DIR}/${BISON_PARSER_CPP_OUT}
        ${PARSER_SRC_DIR}/parse_error.cpp
        ${PARSER_SRC_DIR}/check_variables.cpp
)

target_include_directories(parser
    PUBLIC
        ${BISON_OUTPUT_DIR}          # parser.tab.hpp
        ${PARSER_SRC_DIR}
)

# ====================== FLEX (Lexer) ======================
set(LEXER_SRC_DIR          ${FRONT_SRC_DIR}/lexer/src)
set(FLEX_LEXER_CPP_OUT     lexer.yy.cpp)
set(FLEX_INPUT_FILE        ${LEXER_SRC_DIR}/lexer.l)
set(FLEX_OUTPUT_DIR        ${CMAKE_BINARY_DIR}/generated/flex)

add_custom_command(
    OUTPUT
        ${FLEX_OUTPUT_DIR}/${FLEX_LEXER_CPP_OUT}
    COMMAND ${CMAKE_COMMAND} -E make_directory ${FLEX_OUTPUT_DIR}
    COMMAND ${FLEX_EXECUTABLE}
        -o ${FLEX_OUTPUT_DIR}/${FLEX_LEXER_CPP_OUT}
        ${FLEX_INPUT_FILE}
    DEPENDS
        ${FLEX_INPUT_FILE}
        ${BISON_OUTPUT_DIR}/${BISON_PARSER_HPP_OUT}   # lexer зависит от токенов парсера
    COMMENT "Generating Flex lexer (${FLEX_LEXER_CPP_OUT})"
    VERBATIM
)

add_library(lexer STATIC)

target_sources(lexer
    PRIVATE
        ${FLEX_OUTPUT_DIR}/${FLEX_LEXER_CPP_OUT}
)

target_include_directories(lexer
    PUBLIC
        ${FLEX_OUTPUT_DIR}                     # lexer.yy.cpp
        ${LEXER_SRC_DIR}/inc                   # lexer.hpp
        ${BISON_OUTPUT_DIR}                    # parser.tab.hpp
        ${PARSER_SRC_DIR}
)

# Связываем lexer с parser (нужен для yy::parser::token::...)
target_link_libraries(lexer PRIVATE parser)

# ====================== Main executable ======================
add_executable(paracl main.cpp)

target_link_libraries(paracl
    PRIVATE
        parser
        lexer
)

target_include_directories(paracl
    PRIVATE
        ${FRONT_SRC_DIR}
)
