# =============================================================================================================

# function for adding tests (not unit) to some executable files
function(target_e2e_test test_target run_script test_dir ans_dir)

    file(GLOB test_files "${test_dir}/*.pcl") # save all tests -files in variable 'test_files'

    # get quantity of tests
    list(LENGTH test_files num_tests)

    # check that at least 1 test was found
    if(${num_tests} EQUAL 0)
        message(WARNING "No .pcl files found in ${test_dir}")
        return()
    endif()

    # add tests
    foreach(test_file ${test_files})
        # get name of answer file
        get_filename_component(test_file_name "${test_file}" NAME_WE) # NAME_WE = without extension
        set(answer_file "${ans_dir}/${test_file_name}.ans")

        # create test_name
        set(test_name ${test_target}_${test_file_name})

        set(TEST_COMMAND ${run_script} ${test_target} ${test_file} ${answer_file})

        # add test
        add_test(
            NAME ${test_name}
            # execute test
            COMMAND ${TEST_COMMAND}
            # working directory - dircetory with executable file
            WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
        )
    endforeach()
endfunction(target_e2e_test)

# =============================================================================================================
