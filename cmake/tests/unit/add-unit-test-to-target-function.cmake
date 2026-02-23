# =============================================================================================================
# function to genererate unit-test
# IF SECOND AND/OR THIRD ARGS ARE/IS LIST(-S), DONT FORGET ABOUT QUOTES

function(target_unit_test unit_test_target src include_dirs libs)
    add_executable(${unit_test_target} $<$<BOOL:${BUILDING_TEST}>:EXCLUDE_FROM_ALL>
                   ${src}
    )

    target_link_libraries(${unit_test_target}
        PRIVATE
            ${libs}
            GTest::gtest_main
            # gtest::gtest
    )

    target_include_directories(${unit_test_target}
        PRIVATE
            ${include_dirs}
    )

    # NOT default cmake function
    # MY function for add compile options to executable
    target_hard_debug_options(${unit_test_target})
    target_add_logger(${unit_test_target})
    # target_hard_debug_options(${unit_test_target})
endfunction(target_unit_test)

# =============================================================================================================

# function to add googletets to ctest
# arg `gtest_targets' - is array
function(add_gtests_to_ctest gtest_targets)
    foreach(unit_test_target ${gtest_targets})
        gtest_discover_tests(${unit_test_target})
    endforeach()
endfunction(add_gtests_to_ctest)

# =============================================================================================================
