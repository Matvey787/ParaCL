include(CTest)
enable_testing()

file(GLOB TEST_INPUTS "${CTEST_INPUTS_DIR}/*.${CTEST_INPUT_PATTERN}")

function (create_tests)
    foreach(test_input ${TEST_INPUTS})
        get_filename_component(test_name "${test_input}" NAME_WE)

        set(test_answer "${CTEST_ANSWERS_DIR}/${test_name}.${CTEST_ANSWER_PATTERN}")

        if (EXISTS "${test_answer}")
            add_test(
                NAME "${test_name}"
                COMMAND "python3" "${CTESTER_SCRIPT}"
                        "${CTEST_PROGRAM_EXEC}"
                        "${test_input}"
                        "${test_answer}"
            )
            set_tests_properties("${test_name}" PROPERTIES
                    TIMEOUT "${CTEST_TIMEOUT}"
            )
            message(STATUS "Added test: ${test_name}")  
        else()
            message(WARNING "Answer file not found: ${test_answer}")
        endif()
    endforeach()
endfunction()


if(TEST_INPUTS)
    message(STATUS
        "Tests founded in ${CTEST_INPUTS_DIR}, creating tests..."
    )
    create_tests()
    print_colored_message("CTests generated succesfully, run it by command: \"ctest --test-dir build${TESTER_RELATIVE_PATH} -V\"" 
        ${Color_Bright_Green} ${Bg_Default}
    )
else()
    message(WARNING
        "No test inputs found in ${CTEST_INPUTS_DIR} with pattern *.${CTEST_INPUT_PATTERN}"
    )
endif()
