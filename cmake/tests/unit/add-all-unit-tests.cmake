find_package(GTest REQUIRED)

# =================================================================================================

include(${CMAKE_UNIT_TESTS_SETTING_DIR}/add-unit-test-to-target-function.cmake)

# =================================================================================================

set(UNIT_TESTS_SRC_DIR ${PROJECT_SOURCE_DIR}/tests/unit)

# =================================================================================================

target_unit_test(
        "options_parser_unit_test"
        "${UNIT_TESTS_SRC_DIR}/options_parser/options_parser.cpp"
        ""
        ${OPTIONS_PARSER_LIB}
)

# =================================================================================================

add_gtests_to_ctest("options_parser_unit_test")

