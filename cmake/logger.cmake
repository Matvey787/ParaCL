function(target_add_logger target)
    if (NOT LOGGER)
        return()
    endif(NOT LOGGER)

    target_include_directories(${target}
        PRIVATE
            ${PROJECT_SOURCE_DIR}/third-party/logger/Src/header-only
    )

    target_compile_definitions(${target}
        PRIVATE
            LOGGER
            LOG_DIR="${CMAKE_SOURCE_DIR}/log/"
    )

endfunction(target_add_logger)
