# =================================================================================================

# check thah pyhon3 exist.
# python need for e2e tests
find_program(PYTHON_EXECUTABLE 
    NAMES python3 python
    DOC "Python executable"
)

if (NOT PYTHON_EXECUTABLE)
    message(FATAL_ERROR "Python3 NOT find. No test available.")
endif()

# =================================================================================================

# create tun_test script for e2e paracl test
set(DEBUG_IN_DIR                   ${DEBUG_DIR}/in)
set(RUN_TEST_SCRIPT_IN             ${DEBUG_IN_DIR}/run_test.sh.in)
set(RUN_TEST_COMPILE_SCRIPT_IN     ${DEBUG_IN_DIR}/run_compile_tets.sh.in)

set(DEBUG_RUN_TEST_DIR             ${DEBUG_DIR}/parse_program_output)
set(PYTHON_RUN_TEST_SCRIPT         ${DEBUG_RUN_TEST_DIR}/parse_result.py)
set(PYTHON_RUN_TEST_COMPILE_SCRIPT ${DEBUG_RUN_TEST_DIR}/parse_compile_test_result.py)

set(E2E_OUTPUT_SCRIPT             ${PROJECT_BINARY_DIR}/rt)
set(E2E_COMPILE_OUTPUT_SCRIPT     ${PROJECT_BINARY_DIR}/rtc)

# give execute permission to python script
file(CHMOD ${PYTHON_RUN_TEST_SCRIPT}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
)

file(CHMOD ${PYTHON_RUN_TEST_COMPILE_SCRIPT}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
)


# =================================================================================================

# creating run_test from run_test.in
configure_file(
    ${RUN_TEST_SCRIPT_IN}
    ${E2E_OUTPUT_SCRIPT}
    @ONLY
)

# creating run_test_compile from run_test.in
configure_file(
    ${RUN_TEST_COMPILE_SCRIPT_IN}
    ${E2E_COMPILE_OUTPUT_SCRIPT}
    @ONLY
)

# give execute permission to run_test
file(CHMOD ${E2E_OUTPUT_SCRIPT}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
)

# give execute permission to run_test_compile
file(CHMOD ${E2E_COMPILE_OUTPUT_SCRIPT}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
)

# =================================================================================================
