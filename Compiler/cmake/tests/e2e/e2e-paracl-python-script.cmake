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
set(E2E_SRC_DIR                    ${PROJECT_SOURCE_DIR}/tests/e2e)
set(RUN_TEST_SCRIPT_IN             ${E2E_SRC_DIR}/run_test.sh.in)

set(PYTHON_RUN_TEST_SCRIPT         ${E2E_SRC_DIR}/parse_result.py)

set(E2E_OUTPUT_SCRIPT             ${PROJECT_BINARY_DIR}/rt)

# give execute permission to python script
file(CHMOD ${PYTHON_RUN_TEST_SCRIPT}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
)

# =================================================================================================

# creating run_test from run_test.in
configure_file(
    ${RUN_TEST_SCRIPT_IN}
    ${E2E_OUTPUT_SCRIPT}
    @ONLY
)

# give execute permission to run_test
file(CHMOD ${E2E_OUTPUT_SCRIPT}
    PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
)

# =================================================================================================
