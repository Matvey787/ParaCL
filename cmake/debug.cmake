# flags
# =================================================================================================
# =================================================================================================
# =================================================================================================

function(target_debug_flags target)
    if (NOT CMAKE_BUILD_TYPE STREQUAL Debug)
        return()
    endif(NOT CMAKE_BUILD_TYPE STREQUAL Debug)

    if (MSVC) # правда что ли?
        set(DEBUG_FLAGS
            /W4
            /Wx
        )

        target_compile_options(${target}
            PRIVATE
                ${DEBUG_FLAGS}
        )

    else() # exptect g++ or clang++
        # some saitizers for debug version
        set(DEBUG_FLAGS
            -Wall
            -Wextra
            -Wshadow
        )

        target_compile_options(${target}
            PRIVATE
                ${DEBUG_FLAGS}
        )

        # linking sanitizers
        target_link_options(${target}
            PRIVATE
                ${DEBUG_SANITIZERS}
        )

    endif()

    set(DEBUG_MACROSES
        _DEBUG
        DEBUG
    )

    target_compile_definitions(${target}
        PRIVATE
            ${DEBUG_MACROSES}>
    )

endfunction(target_debug_flags)

# =================================================================================================

function(target_hard_debug_flags target)
    if (NOT CMAKE_BUILD_TYPE STREQUAL Debug)
        return()
    endif(NOT CMAKE_BUILD_TYPE STREQUAL Debug)

    if (MSVC) # правда что ли?
        set(DEBUG_FLAGS
            /W4
            /Wx
        )

        target_compile_options(${target}
            PRIVATE
                ${DEBUG_FLAGS}
        )

    else() # exptect g++ or clang++
        # some saitizers for debug version
        set(DEBUG_FLAGS
            -Wall
            -Wextra
            -Wshadow
            -Wnon-virtual-dtor
            -Weffc++
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wconversion
            -Wdouble-promotion
            -Wformat=2
            -Wimplicit-fallthrough
        )

        target_compile_options(${target} PRIVATE ${DEBUG_FLAGS})

        # linking sanitizers
        target_link_options(${target} PRIVATE ${DEBUG_SANITIZERS})

    endif()

    set(DEBUG_MACROSES
        _DEBUG
        DEBUG
    )

    target_compile_definitions(${target}
        PRIVATE
            ${DEBUG_MACROSES}
    )

endfunction(target_hard_debug_flags)

# sanitizers
# =================================================================================================
# =================================================================================================
# =================================================================================================

function(target_debug_sanitizers target)
    if (NOT Sanitize)
        return()
    endif(NOT Sanitize)

    if (MSVC)
        message("Sanitizers are unsupported for MSVC")
    else() # exptect g++ or clang++

        set(DEBUG_SANITIZERS
            -fsanitize=address
            -fsanitize=leak
            -fsanitize=null
        )

        target_compile_options(${target} PRIVATE ${DEBUG_SANITIZERS})

        # linking sanitizers
        target_link_options(${target} PRIVATE ${DEBUG_SANITIZERS})

    endif()

    set(DEBUG_MACROSES
        _DEBUG
        DEBUG
    )

    target_compile_definitions(${target}
        PRIVATE
            ${DEBUG_MACROSES}
    )

endfunction(target_debug_sanitizers)

# =================================================================================================

function(target_hard_debug_sanitizers target)
    if (NOT Sanitize)
        return()
    endif(NOT Sanitize)

    if (MSVC)
        message("Sanitizers are unsupported for MSVC")
    else() # exptect g++ or clang++

        set(DEBUG_SANITIZERS
            -fsanitize=address
            -fsanitize=alignment
            -fsanitize=bool
            -fsanitize=bounds
            -fsanitize=enum
            -fsanitize=float-cast-overflow
            -fsanitize=undefined
            -fsanitize=unreachable
            -fsanitize=vla-bound
            -fsanitize=vptr
            -fsanitize=float-divide-by-zero
            -fsanitize=integer-divide-by-zero
            -fsanitize=leak
            -fsanitize=nonnull-attribute
            -fsanitize=null
            -fsanitize=return
            -fsanitize=returns-nonnull-attribute
            -fsanitize=shift
            -fsanitize=signed-integer-overflow
            # -fsanitze=size
        )
    
        target_compile_options(${target} PRIVATE ${DEBUG_SANITIZERS})

        # linking sanitizers
        target_link_options(${target} PRIVATE ${DEBUG_SANITIZERS})

    endif()

endfunction(target_hard_debug_sanitizers)

# =================================================================================================
# =================================================================================================
# =================================================================================================
# functions to add all debug options, that autor know and sanitizers to target in Debug mode

function(target_debug_definitions target)
    
    set(DEBUG_MACROSES
        _DEBUG
        DEBUG
    )

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")
        target_compile_definitions(${target} PRIVATE ${DEBUG_MACROSES})
    endif()

endfunction(target_debug_definitions)


function(add_target_debug_options target type)

    if(NOT TARGET ${target})
        message(FATAL_ERROR "add_target_debug_options: target '${target}' does not exist")
    endif()

    if(CMAKE_BUILD_TYPE STREQUAL "Debug")

        if (type STREQUAL hard)
            target_hard_debug_options(${target})
        else()
            target_debug_options(${target})
        endif()

        target_debug_definitions(${target})

    endif()
endfunction(add_target_debug_options)


function(target_hard_debug_options target)
    target_hard_debug_flags(${target})
    target_hard_debug_sanitizers(${target})
endfunction(target_hard_debug_options)

# =================================================================================================
# functions to add all debug options, that autor know and sanitizers to target in Debug mode

function(target_debug_options target)
    target_debug_flags(${target})
    target_debug_sanitizers(${target})
endfunction(target_debug_options)
