# =================================================================================================

# function for adding tests (not unit) to some executable files
function(target_e2e_test test_target run_script test_dir ans_dir)

    file(GLOB test_files "${test_dir}/*.cl") # save all tests -files in variable 'test_files'

    # get quantity of tests
    list(LENGTH test_files num_tests)

    # check that at least 1 test was found
    if(${num_tests} EQUAL 0)
        message(WARNING "No dat files found in ${test_dir}")
        return()
    endif()

    set(test_it 0)

    # add tests
    while (test_it LESS num_tests)
        math(EXPR test_it "${test_it} + 1")

        set(TEST_COMMAND ${run_script} ${test_it})

        # add test
        add_test(
            NAME test_${test_target}_${run_script}_${test_it}
            # execute test
            COMMAND ${TEST_COMMAND}
            # working directory - dircetory with executable file
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        )
    endwhile()
endfunction(target_e2e_test)

# =================================================================================================