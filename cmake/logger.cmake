if (LOGGER)
SET(PINEAPLOG_DIR ${PROJECT_SOURCE_DIR}/third-party/logger/Src/modules-based)
set(SPDLOG_DIR    ${PROJECT_SOURCE_DIR}/third-party/spdlog)

set(LOG_OUT_DIR "${CMAKE_SOURCE_DIR}/log/")


add_subdirectory(${PINEAPLOG_DIR})
add_subdirectory(${SPDLOG_DIR})

set(SPDLOG_INIT_LIB spdlog_init)

add_library(${SPDLOG_INIT_LIB})

target_sources(${SPDLOG_INIT_LIB}
    PUBLIC
        FILE_SET
            CXX_MODULES
        TYPE
            CXX_MODULES
        FILES
            ${DEBUG_DIR}/log/spdlog-init/spdlog_init.cppm
)

target_include_directories(${SPDLOG_INIT_LIB}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/third-party/spdlog/include
)

target_compile_definitions(${SPDLOG_INIT_LIB}
    PRIVATE
        LOG_DIR="${LOG_OUT_DIR}"
)

endif(LOGGER)




function(target_add_logger target)
    target_include_directories(${target}
        PRIVATE
            ${DEBUG_DIR}/
    )

    if (NOT LOGGER)
        return()
    endif(NOT LOGGER)

    target_link_libraries(${target}
        PRIVATE
            pineaplog_mb
            spdlog::spdlog
            spdlog_init
    )

    target_include_directories(${target}
        PRIVATE
            ${DEBUG_DIR}/
    )

    target_compile_definitions(${target}
        PRIVATE
            LOGGER
            LOG_DIR="${LOG_OUT_DIR}"
    )

endfunction(target_add_logger)
