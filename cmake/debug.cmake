# =============================================================================================================
# functions to add some debug options and sanitizers to target in Debug mode

function(target_hard_debug_options target)
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
            # -fsanitize=object-size
        )

        set(DEBUG_FLAGS
            -Wall
            -Wextra
            -Wshadow
            -Wnon-virtual-dtor
            -Weffc++
            # experimental flags:
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wconversion
            -Wdouble-promotion
            -Wformat=2
            -Wimplicit-fallthrough
        )

        target_compile_options(${target}
            PRIVATE
                ${DEBUG_SANITIZERS}
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
            ${DEBUG_MACROSES}
    )

endfunction(target_hard_debug_options)

# =============================================================================================================

function(target_debug_options target)
    if (MSVC) # правда что ли?
        set(DEBUG_FLAGS
            /W4
            /Wx
        )

        target_compile_options(${target}
            PRIVATE
                $<$<CONFIG:Debug>:${DEBUG_FLAGS}>
        )    

    else() # exptect g++ or clang++
        # some saitizers for debug version
        set(DEBUG_SANITIZERS
            -fsanitize=address
            -fsanitize=leak
        )

        set(DEBUG_FLAGS
            -Wall
            -Wextra
            -Wshadow
        )

        target_compile_options(${target}
            PRIVATE
                $<$<CONFIG:Debug>:
                    ${DEBUG_SANITIZERS}
                    ${DEBUG_FLAGS}
                >
        )

        # linking sanitizers
        target_link_options(${target}
            PRIVATE
                $<$<CONFIG:Debug>:${DEBUG_SANITIZERS}>
        )

    endif()

    set(DEBUG_MACROSES
        _DEBUG
        DEBUG
    )

    target_compile_definitions(${target}
        PRIVATE
            $<$<CONFIG:Debug>:${DEBUG_MACROSES}>
    )

endfunction(target_debug_options)

# =============================================================================================================
