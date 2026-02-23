# 3.28 need for support of C++ modules (now (9 Janurary 2026) it`s does`t work for Unix Makefiles)
cmake_minimum_required(VERSION 3.28)
project(ParaCL
    LANGUAGES C CXX
    VERSION 3.0.0
)

# =================================================================================================

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =================================================================================================
# set default settings

# in default we build project in release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

option(BUILD_TESTING "Build tests" OFF)
option(LOGGER "logger" OFF)
option(GRAPHVIZ "Enable graphviz visualization" OFF)

string(TIMESTAMP CURRENT_TIME "%Y-%m-%d %H:%M:%S")

# =================================================================================================
# set some global varibles

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INC_DIR ${PROJECT_SOURCE_DIR}/include)

set(BACKEND_SRC_DIR ${SRC_DIR}/backend)
set(FRONTEND_SRC_DIR ${SRC_DIR}/frontend)

set(FRONTEND_INC_DIR ${INC_DIR}/frontend)

set(DEBUG_DIR ${PROJECT_SOURCE_DIR}/debug)

set(CODEGEN_DIR ${INC_DIR}/codegen)

# =================================================================================================

set(CMAKE_SETTINGS_DIR ${PROJECT_SOURCE_DIR}/cmake)

include(${CMAKE_SETTINGS_DIR}/debug.cmake)
include(${CMAKE_SETTINGS_DIR}/logger.cmake)

# =================================================================================================
# check depecndencies and install it if not found

set(CMAKE_DEPS_DIR ${CMAKE_SETTINGS_DIR}/dep)

include(${CMAKE_DEPS_DIR}/check-flex.cmake)
include(${CMAKE_DEPS_DIR}/check-bison.cmake)
include(${CMAKE_DEPS_DIR}/check-optarg.cmake)
include(${CMAKE_DEPS_DIR}/check-llvm.cmake)

# =================================================================================================
# crete paracl info library
set(PARACL_INFO_DIR ${SRC_DIR}/paracl_info)
set(PARACL_INFO_SRC
    ${PARACL_INFO_DIR}/paracl_info.cppm.in
)

set(PARACL_INFO_BUILD_DIR ${CMAKE_BINARY_DIR}/paracl_info)
set(PARACL_INFO_BUILD_SRC ${PARACL_INFO_BUILD_DIR}/paracl_info.cppm)

# get git commit hash
execute_process(
    COMMAND git log -1 --format=%h
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_COMMIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

# create module with paracl info
configure_file(
    ${PARACL_INFO_SRC}
    ${PARACL_INFO_BUILD_SRC}
    @ONLY
)

set(PARACL_INFO_LIB paracl_info)
add_library(${PARACL_INFO_LIB})

target_sources(${PARACL_INFO_LIB}
    PUBLIC
        FILE_SET
            CXX_MODULES
        TYPE
            CXX_MODULES
        FILES
            ${PARACL_INFO_BUILD_SRC}
)

target_hard_debug_options(${PARACL_INFO_LIB})

# =================================================================================================
# crete stacktrace showing library

set(STACKTRACE_DIR ${DEBUG_DIR}/stacktrace)
set(STACKTRACE_SRC
    ${STACKTRACE_DIR}/exceptions-stacktrace.cppm
)

set(STACKTRACE_LIB stacktrace)
add_library(${STACKTRACE_LIB})

target_sources(${STACKTRACE_LIB}
    PUBLIC
        FILE_SET
            CXX_MODULES
        TYPE
            CXX_MODULES
        FILES
            ${STACKTRACE_SRC}
)

target_hard_debug_options(${STACKTRACE_LIB})

function(target_stacktrace target)
    target_link_libraries(${target} PRIVATE ${STACKTRACE_LIB})
endfunction(target_stacktrace target)

# =================================================================================================
# set some options variables

set(OPTIONS_SRC_DIR ${SRC_DIR}/options)

# =================================================================================================
# add options parser library

set(OPTIONS_LIB options)
add_library(${OPTIONS_LIB})

set(OPTIONS_SRC
    ${OPTIONS_SRC_DIR}/options.cppm
)

target_sources(${OPTIONS_LIB}
    PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
            ${OPTIONS_SRC}   
)

target_include_directories(${OPTIONS_LIB}
    PRIVATE
        ${INC_DIR}
)

target_link_libraries(${OPTIONS_LIB}
    PRIVATE
        ${PARACL_INFO_LIB}    
)

target_hard_debug_options(${OPTIONS_LIB})
target_add_logger(${OPTIONS_LIB})
target_compile_definitions(${OPTIONS_LIB} PRIVATE $<$<BOOL:${GRAPHVIZ}>:GRAPHVIZ>)

# =================================================================================================
# add interpreter options library

set(INTERPRETER_OPTIONS_LIB interpreter_options)
add_library(${INTERPRETER_OPTIONS_LIB})

set(INTERPRETER_OPTIONS_SRC_DIR ${OPTIONS_SRC_DIR})
set(INTERPRETER_OPTIONS_SRC
    ${INTERPRETER_OPTIONS_SRC_DIR}/interpreter_options.cppm
)

target_sources(${INTERPRETER_OPTIONS_LIB}
    PUBLIC
        FILE_SET
            CXX_MODULES
        TYPE
            CXX_MODULES
        FILES
            ${INTERPRETER_OPTIONS_SRC}
)

target_include_directories(${INTERPRETER_OPTIONS_LIB}
    PRIVATE
        ${INC_DIR}
)

target_link_libraries(${INTERPRETER_OPTIONS_LIB}
    PUBLIC
        ${OPTIONS_LIB}
)

target_hard_debug_options(${INTERPRETER_OPTIONS_LIB})
target_add_logger(${INTERPRETER_OPTIONS_LIB})
target_compile_definitions(${OPTIONS_LIB} PRIVATE $<$<BOOL:${GRAPHVIZ}>:GRAPHVIZ>)

# =================================================================================================
# add compiler options library

set(COMPILER_OPTIONS_LIB compiler_options)
add_library(${COMPILER_OPTIONS_LIB})

set(COMPILER_OPTIONS_SRC_DIR ${OPTIONS_SRC_DIR})
set(COMPILER_OPTIONS_SRC
    ${COMPILER_OPTIONS_SRC_DIR}/compiler_options.cppm
)

target_sources(${COMPILER_OPTIONS_LIB}
    PUBLIC
        FILE_SET
            CXX_MODULES
        TYPE
            CXX_MODULES
        FILES
            ${COMPILER_OPTIONS_SRC}
)

target_include_directories(${COMPILER_OPTIONS_LIB}
    PRIVATE
        ${INC_DIR}
)

target_link_libraries(${COMPILER_OPTIONS_LIB}
    PUBLIC
        ${OPTIONS_LIB}
)

target_hard_debug_options(${COMPILER_OPTIONS_LIB})
target_add_logger(${COMPILER_OPTIONS_LIB})
target_compile_definitions(${OPTIONS_LIB} PRIVATE $<$<BOOL:${GRAPHVIZ}>:GRAPHVIZ>)

# =================================================================================================
# set some bison and flex variables

set( BISON_OUT_DIR        ${CMAKE_BINARY_DIR}/bison-out )
set( BISON_PARSER_SRC_DIR ${BISON_OUT_DIR}              )
set( BISON_PARSER_CPP     parser.tab.cpp                )
set( BISON_PARSER_HPP     parser.tab.hpp                )
set( BISON_SRC_DIR        ${FRONTEND_SRC_DIR}/parser    )
set( BISON_SRC            ${BISON_SRC_DIR}/parser.y     )

set( FLEX_OUT_DIR         ${CMAKE_BINARY_DIR}/flex-out  )
set( LEXER_SRC_DIR        ${FLEX_OUT_DIR}               )
set( LEXER_CPP            lexer.yy.cpp                  )
set( FLEX_SRC_DIR         ${FRONTEND_SRC_DIR}/lexer     )
set( FLEX_SRC             ${FLEX_SRC_DIR}/lexer.l       )
set( FLEX_INC_DIR         ${FRONTEND_INC_DIR}/lexer      )

# =================================================================================================
# bison generating command

add_custom_command(
    OUTPUT
        ${BISON_PARSER_SRC_DIR}/${BISON_PARSER_CPP}
        ${BISON_PARSER_SRC_DIR}/${BISON_PARSER_HPP}
    COMMAND
        ${CMAKE_COMMAND} -E make_directory ${BISON_OUT_DIR}
    COMMAND
        ${BISON_EXECUTABLE} -o ${BISON_OUT_DIR}/${BISON_PARSER_CPP} ${BISON_SRC} -d
    DEPENDS
        ${BISON_SRC}
    COMMENT
        "Generate `${BISON_PARSER_CPP}' and `${BISON_PARSER_HPP}' with bison..."
    COMMENT
        "${BISON_EXECUTABLE} -o ${BISON_OUT_DIR}/${BISON_PARSER_CPP} ${BISON_SRC} -d"
)

set(PARSER_LIB parser)
add_library(${PARSER_LIB} STATIC)

target_sources(${PARSER_LIB}
    PRIVATE
        ${BISON_PARSER_SRC_DIR}/${BISON_PARSER_CPP}
        ${BISON_SRC_DIR}/parse_error.cpp
        ${BISON_SRC_DIR}/check_variables.cpp
        ${BISON_SRC_DIR}/parser_exceptions.cpp
)

target_include_directories(${PARSER_LIB}
    PRIVATE
        ${FRONTEND_INC_DIR}
        ${BISON_PARSER_SRC_DIR}/
        ${INC_DIR}/
)

target_debug_definitions(${PARSER_LIB})
target_hard_debug_sanitizers(${PARSER_LIB})
target_add_logger(${PARSER_LIB})

# =================================================================================================
# flex generating command

add_custom_command(
    OUTPUT
        ${LEXER_SRC_DIR}/${LEXER_CPP}
    COMMAND
        ${CMAKE_COMMAND} -E make_directory ${FLEX_OUT_DIR}
    COMMAND 
        ${FLEX_EXECUTABLE} -o ${FLEX_OUT_DIR}/${LEXER_CPP} ${FLEX_SRC}
    DEPENDS
        ${FLEX_SRC}
        ${BISON_PARSER_SRC_DIR}/${BISON_PARSER_HPP}
    COMMENT
        "Generate ${LEXER_CPP} with flex..."
    COMMENT
        "${FLEX_EXECUTABLE} -o ${FLEX_OUT_DIR}/${LEXER_CPP} ${FLEX_SRC}"
)

set(LEXER_LIB lexer)
add_library(${LEXER_LIB} STATIC)

target_sources(${LEXER_LIB}
    PRIVATE
        ${LEXER_SRC_DIR}/${LEXER_CPP}
)

target_include_directories(${LEXER_LIB}
    PRIVATE
        ${FRONTEND_INC_DIR}
        ${BISON_PARSER_SRC_DIR}/
        ${INC_DIR}
)

target_debug_definitions(${LEXER_LIB})
target_hard_debug_sanitizers(${LEXER_LIB})
target_add_logger(${LEXER_LIB})

# =================================================================================================
# node lib (ast = abstract syntax tree)

set(NODES_LIB nodes)
add_library(${NODES_LIB})

set(NODES_SRC_DIR ${SRC_DIR}/ast)
set(NODES_SRC
    ${NODES_SRC_DIR}/nodes.cppm
    ${NODES_SRC_DIR}/node-type-erasure.cppm
)

target_sources(${NODES_LIB}
    PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
            ${NODES_SRC}
)

target_hard_debug_options(${NODES_LIB})
target_hard_debug_sanitizers(${NODES_LIB})
target_add_logger(${NODES_LIB})

# =================================================================================================
# ast writer (ast = abstract syntax tree)

set(AST_WRITER_LIB ast-writer)
add_library(${AST_WRITER_LIB})

set(AST_WRITER_SRC_DIR ${SRC_DIR}/ast)
set(AST_WRITER_SRC
    ${AST_WRITER_SRC_DIR}/write.cppm
)

target_sources(${AST_WRITER_LIB}
    PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
            ${AST_WRITER_SRC}
)

target_link_libraries(${AST_WRITER_LIB}
    PUBLIC
        ${NODES_LIB}
)


target_include_directories(${AST_WRITER_LIB}
    PRIVATE
        ${AST_WRITER_SRC_DIR}
)

target_hard_debug_options(${AST_WRITER_LIB})
target_hard_debug_sanitizers(${AST_WRITER_LIB})
target_add_logger(${AST_WRITER_LIB})

# =================================================================================================
# ast lib (ast = abstract syntax tree)

set(AST_LIB ast)
add_library(${AST_LIB})

set(AST_SRC_DIR ${SRC_DIR}/ast)
set(AST_SRC
    ${AST_SRC_DIR}/ast.cppm
)

target_sources(${AST_LIB}
    PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
            ${AST_SRC}
)

target_link_libraries(${AST_LIB}
    PUBLIC
        ${NODES_LIB}
        ${AST_WRITER_LIB}
)

target_hard_debug_options(${AST_LIB})
target_hard_debug_sanitizers(${AST_LIB})
target_add_logger(${AST_LIB})

# =================================================================================================


add_executable(ast-test
${AST_SRC_DIR}/test.cpp    
)

target_link_libraries(ast-test
PRIVATE
    ${AST_LIB}
    # ${AST_WRITER_LIB}
)

# =================================================================================================
# ast builder lib

set(AST_BUILDER_LIB ast_builder)
add_library(${AST_BUILDER_LIB})

set(AST_BUILDER_SRC_DIR ${FRONTEND_SRC_DIR}/ast_builder)
set(AST_BUILDER_SRC
    ${AST_BUILDER_SRC_DIR}/ast_builder.cppm
)

target_sources(${AST_BUILDER_LIB}
    PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
            ${AST_BUILDER_SRC}
)

target_link_libraries(${AST_BUILDER_LIB}
    PUBLIC
        ${AST_LIB}
)

target_hard_debug_options(${AST_BUILDER_LIB})
target_hard_debug_sanitizers(${AST_BUILDER_LIB})
target_add_logger(${AST_BUILDER_LIB})

# # =================================================================================================
# # hir lib (hir = high-level ir)

# set(HIR_LIB high_level_ir)
# add_library(${HIR_LIB})

# set(HIR_SRC_DIR ${SRC_DIR}/hir)
# set(HIR_SRC
#     ${HIR_SRC_DIR}/hir.cppm
# )

# target_sources(${HIR_LIB}
#     PUBLIC
#         FILE_SET CXX_MODULES
#         TYPE CXX_MODULES
#         FILES
#             ${HIR_SRC}
# )

# target_hard_debug_options(${HIR_LIB})
# target_hard_debug_sanitizers(${HIR_LIB})
# target_add_logger(${HIR_LIB})

# # =================================================================================================
# # hir builder lib (hir = high-level ir)

# set(HIR_BUILDER_LIB hir_builder)
# add_library(${HIR_BUILDER_LIB})

# set(HIR_BUILDER_SRC_DIR ${FRONTEND_SRC_DIR}/hir_builder)
# set(HIR_BUILDER_SRC
#     ${HIR_BUILDER_SRC_DIR}/hir_builder.cppm
# )

# target_sources(${HIR_BUILDER_LIB}
#     PUBLIC
#         FILE_SET CXX_MODULES
#         TYPE CXX_MODULES
#         FILES
#             ${HIR_BUILDER_SRC}
# )

# target_link_libraries(${HIR_BUILDER_LIB}
#     PUBLIC
#         ${HIR_LIB}
#         ${AST_LIB}
# )

# target_hard_debug_options(${HIR_BUILDER_LIB})
# target_hard_debug_sanitizers(${HIR_BUILDER_LIB})
# target_add_logger(${HIR_BUILDER_LIB})

# =================================================================================================
# frontend library

set(FRONTEND_LIB frontend)
add_library(${FRONTEND_LIB} INTERFACE)

target_link_libraries(${FRONTEND_LIB}
    INTERFACE
        ${AST_BUILDER_LIB}
        # ${HIR_BUILDER_LIB}
)

# =================================================================================================
# set some interpreter variable

set(INTERPRETOR_SRC_DIR ${BACKEND_SRC_DIR}/interpreter)

# =================================================================================================
# paracl intepretor nametable

set(INTERPRETOR_NAMETABLE_LIB interpreter_nametable)
add_library(${INTERPRETOR_NAMETABLE_LIB})

set(INTERPRETOR_NAMETABLE_SRC_DIR ${BACKEND_SRC_DIR}/interpreter)
set(INTERPRETOR_NAMETABLE_SRC
    ${INTERPRETOR_NAMETABLE_SRC_DIR}/nametable.cppm
)

target_sources(${INTERPRETOR_NAMETABLE_LIB}
    PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
            ${INTERPRETOR_NAMETABLE_SRC}
)

target_hard_debug_options(${INTERPRETOR_NAMETABLE_LIB})
target_hard_debug_sanitizers(${INTERPRETOR_NAMETABLE_LIB})
target_add_logger(${INTERPRETOR_NAMETABLE_LIB})

# =================================================================================================
# paracl intepretor

set(INTERPRETER_LIB interpreter)
add_library(${INTERPRETER_LIB})

set(INTERPRETOR_SRC
    ${INTERPRETOR_SRC_DIR}/interpreter.cppm
)

target_sources(${INTERPRETER_LIB}
    PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
            ${INTERPRETOR_SRC}
)

target_link_libraries(${INTERPRETER_LIB}
    PUBLIC
        ${INTERPRETER_OPTIONS_LIB}
    PRIVATE
        ${AST_BUILDER_LIB}
        ${INTERPRETOR_NAMETABLE_LIB}
)

# TODO:
target_include_directories(${INTERPRETER_LIB}
    PRIVATE
        ${INC_DIR}
        ${FRONTEND_INC_DIR}
)

target_hard_debug_options(${INTERPRETER_LIB})
target_hard_debug_sanitizers(${INTERPRETER_LIB})
target_add_logger(${INTERPRETER_LIB})

# =================================================================================================
# set some toolchain lib vairables

set(TOOLCHAIN_SRC_DIR ${BACKEND_SRC_DIR}/toolchain)
set(COMPILER_SRC_DIR ${TOOLCHAIN_SRC_DIR}/compiler)

# =================================================================================================
# llvm ir translator nametable lib

set(OBJECTS_BUILDER_NAMETABLE_LIB llvm_ir_translator_nametable)
add_library(${OBJECTS_BUILDER_NAMETABLE_LIB})

set(OBJECTS_BUILDER_NAMETABLE_SRC
    ${COMPILER_SRC_DIR}/nametable.cppm
)

target_sources(${OBJECTS_BUILDER_NAMETABLE_LIB}
    PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
            ${OBJECTS_BUILDER_NAMETABLE_SRC}
)

# TODO:

target_include_directories(${OBJECTS_BUILDER_NAMETABLE_LIB}
    PRIVATE
        ${LLVM_INCLUDE_DIRS}
        ${INC_DIR}
        ${FRONTEND_INC_DIR}
        ${DEBUG_DIR}
)

target_link_libraries(${OBJECTS_BUILDER_NAMETABLE_LIB}
    PUBLIC
        ${COMPILER_OPTIONS_LIB}
    PRIVATE
        LLVM
        ${PARACL_LLVM_TRAN}
)

target_add_logger(${OBJECTS_BUILDER_NAMETABLE_LIB})
target_debug_definitions(${OBJECTS_BUILDER_NAMETABLE_LIB})
target_hard_debug_sanitizers(${OBJECTS_BUILDER_NAMETABLE_LIB})

# =================================================================================================
# llvm ir translator

set(OBJECTS_BUILDER_LIB llvm_ir_translator)
add_library(${OBJECTS_BUILDER_LIB})

set(OBJECTS_BUILDER_SRC_DIR ${TOOLCHAIN_SRC_DIR}/compiler)
set(OBJECTS_BUILDER_SRC
    ${OBJECTS_BUILDER_SRC_DIR}/objects_builder.cppm
    ${OBJECTS_BUILDER_SRC_DIR}/libc_stadart_functions.cppm
)

target_sources(${OBJECTS_BUILDER_LIB}
    PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
            ${OBJECTS_BUILDER_SRC}
)

target_include_directories(${OBJECTS_BUILDER_LIB}
    PRIVATE
        ${LLVM_INCLUDE_DIRS}
        ${INC_DIR}
        ${FRONTEND_INC_DIR}
        ${DEBUG_DIR}
)

target_link_libraries(${OBJECTS_BUILDER_LIB}
    PUBLIC
        ${COMPILER_OPTIONS_LIB}
    PRIVATE
        LLVM
        ${OBJECTS_BUILDER_NAMETABLE_LIB}
)

target_add_logger(${OBJECTS_BUILDER_LIB})
target_debug_definitions(${OBJECTS_BUILDER_LIB})
target_hard_debug_sanitizers(${OBJECTS_BUILDER_LIB})

# =================================================================================================
# linker lib

set(LINKER_LIB linker)
add_library(${LINKER_LIB})

set(LINKER_LIB_SRC_DIR ${TOOLCHAIN_SRC_DIR}/linker)
set(LINKER_LIB_SRC
    ${LINKER_LIB_SRC_DIR}/linker.cppm
)

target_sources(${LINKER_LIB}
    PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
            ${LINKER_LIB_SRC}
)

target_include_directories(${LINKER_LIB}
    PRIVATE
        ${INC_DIR}
        ${DEBUG_DIR}
)

target_link_libraries(${LINKER_LIB}
    PUBLIC
        ${COMPILER_OPTIONS_LIB}
)

target_add_logger(${LINKER_LIB})
target_debug_definitions(${LINKER_LIB})
target_hard_debug_sanitizers(${LINKER_LIB})

# =================================================================================================
# toolchain lib

set(TOOLCHAIN_LIB toolchain)
add_library(${TOOLCHAIN_LIB})

set(TOOLCHAIN_SRC
    ${TOOLCHAIN_SRC_DIR}/toolchain.cppm
)

target_compile_definitions(${TOOLCHAIN_LIB} PRIVATE $<$<BOOL:${GRAPHVIZ}>:GRAPHVIZ>)

target_sources(${TOOLCHAIN_LIB}
    PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
            ${TOOLCHAIN_SRC}
)

target_include_directories(${TOOLCHAIN_LIB}
    PRIVATE
        ${DEBUG_DIR}
        ${INC_DIR}
        ${FRONTEND_DIR}
        ${LEXER_SRC_DIR}
        ${BISON_OUT_DIR}
        ${FRONTEND_INC_DIR}
)

target_link_libraries(${TOOLCHAIN_LIB}
    PUBLIC
        ${OPTIONS_LIB}

    PRIVATE
        ${INTERPRETER_LIB}
        ${OBJECTS_BUILDER_LIB}
        ${LINKER_LIB}
        ${PARSER_LIB}
        ${LEXER_LIB}
)

target_add_logger(${TOOLCHAIN_LIB})
target_hard_debug_options(${TOOLCHAIN_LIB})
target_stacktrace(${TOOLCHAIN_LIB})

# =================================================================================================
# backend lib

set(BACKEND_LIB backend)
add_library(${BACKEND_LIB} INTERFACE)

target_link_libraries(${BACKEND_LIB}
    INTERFACE
        ${INTERPRETER_LIB}
        ${TOOLCHAIN_LIB}
)

# =================================================================================================
# paracl runner lib

set(PARACL_RUNNER_LIB runner)
add_library(${PARACL_RUNNER_LIB})

set(PARACL_RUNNER_SRC_DIR ${SRC_DIR}/paracl_runner)
set(PARACL_RUNNER_SRC
    ${PARACL_RUNNER_SRC_DIR}/runner.cppm

    # TODO:
    ${PARACL_RUNNER_SRC_DIR}/parse_exceptions.cppm
    $<$<BOOL:${GRAPHVIZ}>:${DEBUG_DIR}/ast/ast-graph-dump.cppm>
)

target_sources(${PARACL_RUNNER_LIB}
    PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
            ${PARACL_RUNNER_SRC}
)

target_link_libraries(${PARACL_RUNNER_LIB}
    PUBLIC
        ${OPTIONS_LIB}

    PRIVATE
        ${INTERPRETER_OPTIONS_LIB}
        ${COMPILER_OPTIONS_LIB}
        ${BACKEND_LIB}
)

# =================================================================================================
# paracl executable
set(PARACL_EXE paracl)
add_executable(${PARACL_EXE})

target_sources(${PARACL_EXE}
    PRIVATE
        ${SRC_DIR}/main.cpp
)

target_link_libraries(${PARACL_EXE}
    PRIVATE
        ${OPTIONS_LIB}
        ${OPTIONS_LIB}
        ${PARACL_RUNNER_LIB}
)

target_include_directories(${PARACL_EXE}
    PRIVATE
        ${INC_DIR}
        ${FRONTEND_INC_DIR}
        ${BACKEND_INC_DIR}
        ${CODEGEN_DIR}
)

target_hard_debug_options(${PARACL_EXE})
target_add_logger(${PARACL_EXE})
target_stacktrace(${PARACL_EXE})

# =================================================================================================

if (BUILD_TESTING)
    enable_testing()
    include(CTest)

    set(PARACL_E2E_TESTS_DIR ${PROJECT_SOURCE_DIR}/tests/e2e)
    set(PARACL_E2E_DAT_DIR ${PARACL_E2E_TESTS_DIR}/dat)
    set(PARACL_E2E_ANS_DIR ${PARACL_E2E_TESTS_DIR}/ans)

    set(CMAKE_E2E_TESTS_SETTING_DIR ${CMAKE_SETTINGS_DIR}/tests/e2e)

    include(${CMAKE_E2E_TESTS_SETTING_DIR}/e2e-paracl-python-script.cmake)
    include(${CMAKE_E2E_TESTS_SETTING_DIR}/add-e2e-to-target-function.cmake)

    target_e2e_test(${PARACL_EXE}
        ${E2E_OUTPUT_SCRIPT}
        ${PARACL_E2E_DAT_DIR}
        ${PARACL_E2E_ANS_DIR}
    )

    target_e2e_test(${PARACL_EXE}
        ${E2E_COMPILE_OUTPUT_SCRIPT}
        ${PARACL_E2E_DAT_DIR}
        ${PARACL_E2E_ANS_DIR}
    )

    set(CMAKE_UNIT_TESTS_SETTING_DIR ${CMAKE_SETTINGS_DIR}/tests/unit)

    include(${CMAKE_UNIT_TESTS_SETTING_DIR}/add-all-unit-tests.cmake)
endif()

# =================================================================================================
