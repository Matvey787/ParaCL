cmake_minimum_required(VERSION 3.28)
project(paraCL
    LANGUAGES CXX
    VERSION 1.5
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

# why not?
if (NOT CMAKE_EXPORT_COMPILE_COMMANDS)
    set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
endif()

option(BUILD_TESTING "Build tests" OFF)
option(LOGGER "logger" OFF)
option(GRAPHVIZ "Enable graphviz visualization (graphviz)" OFF)

string(TIMESTAMP CURRENT_TIME "%Y-%m-%d %H:%M:%S")

# =================================================================================================
# set some global varibles

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INC_DIR ${PROJECT_SOURCE_DIR}/include)

set(BACKEND_SRC_DIR ${SRC_DIR}/backend)
set(FRONTEND_SRC_DIR ${SRC_DIR}/frontend)

set(FRONTEND_INC_DIR ${INC_DIR}/frontend)

set(DEBUG_DIR ${PROJECT_SOURCE_DIR}/debug)

# =================================================================================================

set(CMAKE_SETTINGS_DIR ${PROJECT_SOURCE_DIR}/cmake)

include(${CMAKE_SETTINGS_DIR}/debug.cmake)
include(${CMAKE_SETTINGS_DIR}/logger.cmake)

# =================================================================================================
# check depecndencies and install it if not found

set(CMAKE_DEPS_DIR ${CMAKE_SETTINGS_DIR}/dep)

include(${CMAKE_DEPS_DIR}/check-flex.cmake)
include(${CMAKE_DEPS_DIR}/check-bison.cmake)

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
)

target_include_directories(${PARSER_LIB}
    PRIVATE
        ${FRONTEND_INC_DIR}
        ${BISON_PARSER_SRC_DIR}/
        ${INC_DIR}/
)

target_debug_definitions(${PARSER_LIB})
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

target_add_logger(${LEXER_LIB})

# =================================================================================================
# add options parser library

set(OPTIONS_PARSER_LIB options_parser)
add_library(${OPTIONS_PARSER_LIB})

set(OPTIONS_PARSER_SRC_DIR ${SRC_DIR}/options_parser)
set(OPTIONS_PARSER_SRC
    ${OPTIONS_PARSER_SRC_DIR}/options_parser.cppm
    ${OPTIONS_PARSER_SRC_DIR}/paracl_extension.cppm
)

target_sources(${OPTIONS_PARSER_LIB}
    PUBLIC
        FILE_SET
            CXX_MODULES
        TYPE
            CXX_MODULES
        FILES
            ${OPTIONS_PARSER_SRC}   
)

target_include_directories(${OPTIONS_PARSER_LIB}
    PRIVATE
        ${INC_DIR}
)

target_link_libraries(${OPTIONS_PARSER_LIB}
    PRIVATE
        ${PARACL_EXTENSION_LIB}
        ${PARACL_INFO_LIB}
)

target_compile_definitions(${OPTIONS_PARSER_LIB}
    PRIVATE
        PARACL_VERSION="${PROJECT_VERSION}"
)

add_target_debug_options(${OPTIONS_PARSER_LIB} hard)
target_add_logger(${OPTIONS_PARSER_LIB})

# =================================================================================================
# paracl intepretor

set(PARACL_INTERPRETOR_LIB interpreter)
add_library(${PARACL_INTERPRETOR_LIB})

set(PARACL_INTERPRETOR_SRC_DIR ${BACKEND_SRC_DIR}/interpreter)
set(PARACL_INTERPRETOR_SRC
    ${PARACL_INTERPRETOR_SRC_DIR}/interpreter.cppm
    ${PARACL_INTERPRETOR_SRC_DIR}/name_table.cppm
)

target_sources(${PARACL_INTERPRETOR_LIB}
    PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
            ${PARACL_INTERPRETOR_SRC}
    PRIVATE
        
)

target_include_directories(${PARACL_INTERPRETOR_LIB}
    PRIVATE
        ${INC_DIR}
        ${FRONTEND_INC_DIR}
)

# target_link_libraries(${PARACL_INTERPRETOR_LIB}
#     PRIVATE
#         ${PARSER_LIB}
#         ${LEXER_LIB}
# )

add_target_debug_options(${PARACL_INTERPRETOR_LIB} hard)
target_add_logger(${PARACL_INTERPRETOR_LIB})

# =================================================================================================
# run paracl library

set(RUN_PARACL_LIB run_paracl)
add_library(${RUN_PARACL_LIB})

set(RUN_PARACL_SRC_DIR ${BACKEND_SRC_DIR})
set(RUN_PARACL_SRC
    ${RUN_PARACL_SRC_DIR}/run_paracl.cppm
    ${RUN_PARACL_SRC_DIR}/parse_paracl_exceptions.cppm
)

if (GRAPHVIZ)
set(RUN_PARACL_SRC
    ${RUN_PARACL_SRC}
    ${DEBUG_DIR}/ast/ast-graph-dump.cppm    
)
target_compile_definitions(${RUN_PARACL_LIB} PRIVATE GRAPHVIZ)
endif(GRAPHVIZ)

target_sources(${RUN_PARACL_LIB}
    PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
            ${RUN_PARACL_SRC}
    PRIVATE
        
)

target_include_directories(${RUN_PARACL_LIB}
    PRIVATE
        ${FRONTEND_DIR}
        ${INC_DIR}
        ${LEXER_SRC_DIR}
        ${BISON_OUT_DIR}
        ${FRONTEND_INC_DIR}
)

target_link_libraries(${RUN_PARACL_LIB}
    PRIVATE
        ${PARACL_INTERPRETOR_LIB}
        ${OPTIONS_PARSER_LIB}
        ${PARSER_LIB}
        ${LEXER_LIB}
)

add_target_debug_options(${RUN_PARACL_LIB} hard)
target_add_logger(${RUN_PARACL_LIB})

# =================================================================================================

set(PARACL_EXE paracl)

add_executable(${PARACL_EXE})

target_sources(${PARACL_EXE}
    PRIVATE
        ${SRC_DIR}/main.cpp
)

target_link_libraries(${PARACL_EXE}
    PRIVATE
        ${OPTIONS_PARSER_LIB}
        ${RUN_PARACL_LIB}
)

target_include_directories(${PARACL_EXE}
    PRIVATE
        ${INC_DIR}
)

add_target_debug_options(${PARACL_EXE} hard)
target_add_logger(${PARACL_EXE})

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

    target_e2e_test(${CMAKE_BINARY_DIR}/${PARACL_EXE}
        ${PYTHON_RUN_TEST_SCRIPT}
        ${PARACL_E2E_DAT_DIR}
        ${PARACL_E2E_ANS_DIR}
    )
endif()

# =================================================================================================
