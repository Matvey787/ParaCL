find_program(BISON_EXECUTABLE
    NAMES bison bison.exe
)

if (NOT BISON_EXECUTABLE)
    message(STATUS "Fetching flex...")
    FetchContent_Declare (
        bison
        URL https://github.com/akimd/bison/archive/refs/tags/v3.8.2.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(bison)
    find_package(bison REQUIRED)

else ()
    message(STATUS "Using system bison: ${BISON_EXECUTABLE}")
endif()
