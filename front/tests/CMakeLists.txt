set(TESTS_EXEC_FILE_NAME "paracl_tests")
set(SOURCE_FILES
    ${CMAKE_SOURCE_DIR}/front/src/main.cpp
    ${CMAKE_SOURCE_DIR}/front/src/lexer.cpp
    ${CMAKE_SOURCE_DIR}/front/src/parcer.cpp
    ${CMAKE_SOURCE_DIR}/front/src/compiler.cpp
)

set(INCLUDE_DIRECTORIES ${CMAKE_SOURCE_DIR}/front/inc)

set(FETCH_LIBS_NAMES
    googletest
)
set(FETCH_LIBS_URLS
    https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
)

find_package(GTest CONFIG QUIET)

if(NOT GTest_FOUND)
    message(STATUS "GTest not found, downloading via FetchContent...")
    include(FetchContent)
    foreach(libName libURL IN ZIP_LISTS FETCH_LIBS_NAMES FETCH_LIBS_URLS)
        message(STATUS "Fetching library: ${libName}, URL: ${libURL}")
        FetchContent_Declare(
            ${libName}
            URL ${libURL}
        )
        FetchContent_MakeAvailable(${libName})
    endforeach()
else()
    message(STATUS "Using system GTest: ${GTest_DIR}")
endif()

add_executable(${TESTS_EXEC_FILE_NAME} ${SOURCE_FILES})

target_link_libraries(${TESTS_EXEC_FILE_NAME} PRIVATE GTest::gtest GTest::gtest_main)
target_include_directories(${TESTS_EXEC_FILE_NAME} PRIVATE ${INCLUDE_DIRECTORIES})

include(CTest)
enable_testing()

function(create_tests_from_pairs)
    file(GLOB TEST_FILES ${CMAKE_SOURCE_DIR}/front/tests/test*.dat)

    if(!TEST_FILES)
        message(WARNING "No test files found.")
        return()
    endif()

    foreach(TEST_FILE ${TEST_FILES})

        get_filename_component(TEST_NAME ${TEST_FILE} NAME_WE)
        string(REGEX REPLACE "^test" "" TEST_NUMBER ${TEST_NAME})
        
        set(ANS_FILE ${CMAKE_SOURCE_DIR}/front/tests/ans${TEST_NUMBER}.dat)
        
        if(EXISTS ${ANS_FILE})
            file(READ ${ANS_FILE} EXPECTED_OUTPUT)
            string(STRIP "${EXPECTED_OUTPUT}" EXPECTED_OUTPUT)
            
            add_test(NAME test_${TEST_NUMBER}
                COMMAND bash -c "../../build/${EXEC_FILE_NAME} test${TEST_NUMBER}.dat"
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/front/tests
            )
            
            set_tests_properties(test_${TEST_NUMBER} PROPERTIES
                PASS_REGULAR_EXPRESSION "^${EXPECTED_OUTPUT}([ \t\n\r]|$)"
            )
        else()
            message(WARNING "Answer file not found for test ${TEST_NUMBER}: ${ANS_FILE}")
        endif()
    endforeach()
endfunction()

create_tests_from_pairs()