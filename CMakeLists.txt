cmake_minimum_required(VERSION 3.28)
project(paraCL
    LANGUAGES CXX
    VERSION 1.1
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
# set some global varibles

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(INC_DIR ${PROJECT_SOURCE_DIR}/include)

set(PARACL_SRC_DIR ${SRC_DIR}/paracl)

# =================================================================================================
# set some bison and flex variables

set( BISON_OUT_DIR        ${CMAKE_BINARY_DIR}/bison-out )
set( BISON_PARSER_SRC_DIR ${BISON_OUT_DIR}              )
set( BISON_PARSER_CPP     parser.tab.cpp                )
set( BISON_PARSER_HPP     parser.tab.hpp                )
set( BISON_SRC_DIR        ${SRC_DIR}/back/src           )
set( BISON_SRC            ${BISON_SRC_DIR}/parser.y     )

set( FLEX_OUT_DIR         ${CMAKE_BINARY_DIR}/flex-out  )
set( LEXER_SRC_DIR        ${FLEX_OUT_DIR}               )
set( LEXER_CPP            lexer.yy.cpp                  )
set( FLEX_SRC_DIR         ${SRC_DIR}/front/src          )
set( FLEX_INC_DIR         ${SRC_DIR}/front/inc          )
set( FLEX_SRC             ${FLEX_SRC_DIR}/lexer.l       )

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
)

set(PARSER_LIB parser)
add_library(${PARSER_LIB} STATIC)

target_sources(${PARSER_LIB}
    PRIVATE
        ${BISON_PARSER_SRC_DIR}/${BISON_PARSER_CPP}
)

target_include_directories(${PARSER_LIB}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src/back/inc/
        ${BISON_PARSER_SRC_DIR}/
        ${FLEX_INC_DIR}/
        ${INC_DIR}/
)

add_target_DEBUG__DEBUG_options(${PARSER_LIB})
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
)

set(LEXER_LIB lexer)
add_library(${LEXER_LIB} STATIC)

target_sources(${LEXER_LIB}
    PRIVATE
        ${LEXER_SRC_DIR}/${LEXER_CPP}
)

target_include_directories(${LEXER_LIB}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src/back/inc/
        ${BISON_PARSER_SRC_DIR}/
        ${INC_DIR}
)

add_target_DEBUG__DEBUG_options(${LEXER_LIB})

target_add_logger(${LEXER_LIB})

# =================================================================================================
# add paracl extension library

set(PARACL_EXTENSION_LIB paracl_extension)
add_library(${PARACL_EXTENSION_LIB})

set(PARACL_EXTENSION_SRC_DIR ${PARACL_SRC_DIR})
set(PARACL_SRC
    ${PARACL_EXTENSION_SRC_DIR}/paracl_extension.cppm
)

target_sources(${PARACL_EXTENSION_LIB}
    PUBLIC
        FILE_SET
            CXX_MODULES
        TYPE
            CXX_MODULES
        FILES
            ${PARACL_SRC}
)

add_target_debug_options(${PARACL_EXTENSION_LIB} hard)
target_add_logger(${PARACL_EXTENSION_LIB})

# =================================================================================================
# add options parser library

set(OPTIONS_PARSER_LIB options_parse)
add_library(${OPTIONS_PARSER_LIB})

set(OPTIONS_PARSER_SRC_DIR ${SRC_DIR}/options_parser)
set(OPTIONS_PARSER_SRC
    ${OPTIONS_PARSER_SRC_DIR}/options_parser.cppm
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
)

target_compile_definitions(${OPTIONS_PARSER_LIB}
    PRIVATE
        PARACL_VERSION="${PROJECT_VERSION}"
)

add_target_debug_options(${OPTIONS_PARSER_LIB} hard)
target_add_logger(${OPTIONS_PARSER_LIB})

# =================================================================================================
# run paracl library

set(RUN_PARACL_LIB run_paracl)
add_library(${RUN_PARACL_LIB})

set(RUN_PARACL_SRC_DIR ${PARACL_SRC_DIR})
set(RUN_PARACL_SRC
    ${RUN_PARACL_SRC_DIR}/run_paracl.cppm
    ${RUN_PARACL_SRC_DIR}/parse_paracl_exit_code.cppm
)

target_sources(${RUN_PARACL_LIB}
    PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES
            ${RUN_PARACL_SRC}
)

target_sources(${RUN_PARACL_LIB}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src/back/src/compiler.cpp
        ${PROJECT_SOURCE_DIR}/src/back/src/dump.cpp
)

target_include_directories(${RUN_PARACL_LIB}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/src/back/inc/
        ${INC_DIR}
        ${LEXER_SRC_DIR}
        ${BISON_OUT_DIR}
        ${FLEX_INC_DIR}
)

target_link_libraries(${RUN_PARACL_LIB}
    PRIVATE
        ${OPTIONS_PARSER_LIB}
        ${PARSER_LIB}
        ${LEXER_LIB}
)

if(GRAPHVIZ)
    target_compile_definitions(${RUN_PARACL_LIB} PRIVATE GRAPHVIZ)
endif()

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
