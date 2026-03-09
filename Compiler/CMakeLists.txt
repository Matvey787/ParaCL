cmake_minimum_required(VERSION 3.28)

project(ParaCLC#ompiler
    LANGUAGES CXX
    VERSION 5.0
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif(NOT CMAKE_BUILD_TYPE)

set(PARACL_COMPILER ${CMAKE_BINARY_DIR}/backend/paracl-compiler)

if (NOT PARACL_FRONT)
  message(FATAL_ERROR "Please set 'PARACL_FRONT' for this cmake lists")
endif (NOT PARACL_FRONT)

add_subdirectory(${PROJECT_SOURCE_DIR}/backend)

set(SRC_DIR ${PROJECT_SOURCE_DIR}/src)

set(PARACL_EXE paracl)
add_executable(${PARACL_EXE})

target_sources(${PARACL_EXE}
PRIVATE
    ${SRC_DIR}/paracl.cpp
)

target_compile_definitions(${PARACL_EXE}
PRIVATE
    PARACL_FRONT="${PARACL_FRONT}"
    PARACL_COMPILER="${PARACL_COMPILER}"
)

# TESTS
enable_testing()
include(CTest)

set(PARACL_E2E_TESTS_DIR ${PROJECT_SOURCE_DIR}/tests/e2e)
set(PARACL_E2E_DAT_DIR ${PARACL_E2E_TESTS_DIR}/dat)
set(PARACL_E2E_ANS_DIR ${PARACL_E2E_TESTS_DIR}/ans)

include(${CMAKE_SOURCE_DIR}/cmake/tests/e2e/e2e-paracl-python-script.cmake)
include(${CMAKE_SOURCE_DIR}/cmake/tests/e2e/add-e2e-to-target-function.cmake)

target_e2e_test(paracl
    ${E2E_OUTPUT_SCRIPT}
    ${PARACL_E2E_DAT_DIR}
    ${PARACL_E2E_ANS_DIR}
)
