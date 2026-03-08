find_program(FLEX_EXECUTABLE
    NAMES flex flex.exe
    DOC "flex executable"
)

if (NOT FLEX_EXECUTABLE)
    message(STATUS "Fetching flex...")
    FetchContent_Declare(
        flex
        URL https://github.com/westes/flex/archive/refs/tags/v2.6.4.zip
        DOWNLOAD_EXTRACT_TIMESTAMP TRUE
    )
    FetchContent_MakeAvailable(flex)
    find_package(flex REQUIRED)
else ()
    message(STATUS "Using system flex: ${FLEX_EXECUTABLE}")
endif()
