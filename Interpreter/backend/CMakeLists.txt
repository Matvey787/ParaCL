cmake_minimum_required(VERSION 3.30)

# =================================================================================================

project(ParaCL-Interpreter
    VERSION 1.0
    LANGUAGES CXX
)

# =================================================================================================

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# =================================================================================================

# in default we build project in release
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

option(BUILD_TESTING "Build tests" OFF)
option(LOGGER "logger" OFF)
option(GRAPHVIZ "Enable graphviz visualization" OFF)

string(TIMESTAMP CURRENT_TIME "%Y-%m-%d %H:%M:%S")

# =================================================================================================

set(PARACL_INTERPRETER_SRC_DIR ${PROJECT_SOURCE_DIR}/src)
set(PARACL_INTERPRETER_THIRD_PARTY_DIR ${PROJECT_SOURCE_DIR}/third-party)

# =================================================================================================
# add ast library

add_subdirectory(
    ${CMAKE_SOURCE_DIR}/../AST
    ${CMAKE_BINARY_DIR}/subprojects/ast    
)

# =================================================================================================

# nametable library

set(NAMETABLE_LIB nametable)
add_library(${NAMETABLE_LIB})

set(NAMETABLE_SRC_DIR ${PARACL_INTERPRETER_SRC_DIR}/nametable)
set(NAMETABLE_SRC
    ${NAMETABLE_SRC_DIR}/nametable.cppm
)

target_sources(${NAMETABLE_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${NAMETABLE_SRC}
)

# =================================================================================================
# interpreter library

set(INTERPRETER_LIB interpreter)
add_library(${INTERPRETER_LIB})

set(INTERPRETER_SRC_DIR ${PARACL_INTERPRETER_SRC_DIR}/interpreter)
set(INTERPRETER_SRC
    ${INTERPRETER_SRC_DIR}/interpreter.cppm
)

target_sources(${INTERPRETER_LIB}
  PUBLIC
    FILE_SET CXX_MODULES
    TYPE CXX_MODULES
    FILES
        ${INTERPRETER_SRC}
)

target_link_libraries(${INTERPRETER_LIB}
  PRIVATE
    ${NAMETABLE_LIB}
    TheLast::TheLast
)

# =================================================================================================

set(INTERPRETER paracl-interpreter)

add_executable(${INTERPRETER}
    ${PARACL_INTERPRETER_SRC_DIR}/main.cpp
)

target_link_libraries(${INTERPRETER}
  PRIVATE
    ${INTERPRETER_LIB}
)

# =================================================================================================
