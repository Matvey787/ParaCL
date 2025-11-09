# =============================================================================================================

# check thah pyhon3 exist.
# python need for e2e tests
find_program(PYTHON_EXECUTABLE 
    NAMES python3 python
    DOC "Python executable"
)

if (NOT PYTHON_EXECUTABLE)
    message(FATAL_ERROR "Python3 NOT find. No test available.")
endif()

# =============================================================================================================

# create tun_test script for e2e triangles test
set(DEBUG_DIR              ${PROJECT_SOURCE_DIR}/debug)

set(DEBUG_IN_DIR           ${DEBUG_DIR}/in)
set(RUN_TEST_SCRIPT_IN     ${DEBUG_IN_DIR}/run_test.sh.in)

set(DEBUG_RUN_TEST_DIR     ${DEBUG_DIR}/parse_program_output)
set(PYTHON_RUN_TEST_SCRIPT ${DEBUG_RUN_TEST_DIR}/parse_result.py)

set(OUTPUT_SCRIPT          ${PROJECT_BINARY_DIR}/run_test)

string(TIMESTAMP CURRENT_TIME "%Y-%m-%d %H:%M:%S")

# give execute permission to python scripp
file(CHMOD ${PYTHON_RUN_TEST_SCRIPT}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
)

# =============================================================================================================

# creating run_test from run_test.in
configure_file(
    ${RUN_TEST_SCRIPT_IN}
    ${OUTPUT_SCRIPT}
    @ONLY
)

# give execute permission to run_test
file(CHMOD ${OUTPUT_SCRIPT}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
)
# =============================================================================================================

# function for adding tests (not unit) to some executable files
function(target_e2e_test test_target run_script test_dir ans_dir)

    file(GLOB test_files "${test_dir}/*.dat") # save all tests -files in variable 'test_files'

    # get quantity of tests
    list(LENGTH test_files num_tests)

    # check that at least 1 test was found
    if(${num_tests} EQUAL 0)
        message(WARNING "No .dat files found in ${test_dir}")
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

