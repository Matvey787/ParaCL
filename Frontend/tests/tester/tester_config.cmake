# Tester framework configuration (all info in readme)

# CTest setting

set(CTEST_PROGRAM_EXEC   "${CMAKE_CURRENT_SOURCE_DIR}/../../../examples/geo_lib/build/intersection")
set(CTEST_INPUTS_DIR     "${CMAKE_CURRENT_SOURCE_DIR}/../e2e/dat")
set(CTEST_ANSWERS_DIR    "${CMAKE_CURRENT_SOURCE_DIR}/../e2e/ans")

set(CTEST_INPUT_PATTERN  "dat")
set(CTEST_ANSWER_PATTERN "ans")
set(CTEST_PREFIX         "...")
set(CTEST_TIMEOUT        30)










# Gtest setting

set(GTEST_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../gtest")

set(GTEST_OUTPUT_EXEC_FILE_NAME "front_run_tests")

set(GTEST_ADDITIONAL_LIBS_NAMES "paracl-front-lib")

set(LLVMCOV TRUE)

set(GTEST_ADDITIONAL_LIBS_NAMES_ENABLE_LLVMCOV 0)











# Do not change without any special reason
set(CTESTER_SCRIPT       "${CMAKE_CURRENT_SOURCE_DIR}/cmake/tester_script.py")
set(GTEST_FETCH_URL "https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip")

string(REPLACE "${CMAKE_SOURCE_DIR}" "" TESTER_RELATIVE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")

set(INFO_USER_FONT_COLOR_MESSAGE   ${Color_Bright_Green})
set(INFO_USER_BACK_COLOR_MESSAGE   ${Bg_Default})

set(STATUS_COLOR_MESSAGE      ${Color_White}  )
set(WARNING_COLOR_MESSAGE     ${Color_Magenta})
set(FATAL_ERROR_COLOR_MESSAGE ${Color_Red}    )