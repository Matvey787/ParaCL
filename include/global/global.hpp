#pragma once

//---------------------------------------------------------------------------------------------------------------

#if defined(NDEBUG) and (defined(_DEBUG) or defined(DEBUG))
#error "bad release-debug macro combo"
#endif /* defined(NDEBUG) and (defined(_DEBUG) or defined(DEBUG)) */

//---------------------------------------------------------------------------------------------------------------

#include <cstdlib>  // for EXIT_SUCCESS macro
#include <iostream> // for msg_assert

#include "global/custom_console_output.hpp"

//---------------------------------------------------------------------------------------------------------------

#if not defined(NDEBUG)

//---------------------------------------------------------------------------------------------------------------

#define ON_DEBUG(...) __VA_ARGS__
#define OFF_DEBUG(...)

#define builtin_unreachable_wrapper(debug_message)                                                                     \
    msg_assert(false, "__builtin_unreachable() reaached\n" << debug_message)

#define msg_assert(bool_expression, message)                                                                           \
    do                                                                                                                 \
    {                                                                                                                  \
        if (not(bool_expression))                                                                                      \
        {                                                                                                              \
            std::cerr << RED BOLD "assertation failed:\n" RESET_CONSOLE_OUT WHITE << #bool_expression << "\n"          \
                      << __FILE__ << ":" << __LINE__ << " [" << __func__                                               \
                      << "]\n" RESET_CONSOLE_OUT RED "message:\n" BOLD << message << RESET_CONSOLE_OUT "\n";           \
            exit(EXIT_FAILURE);                                                                                        \
        }                                                                                                              \
    } while (0)

//---------------------------------------------------------------------------------------------------------------

#else /* not defined(NDEBUG) */

//---------------------------------------------------------------------------------------------------------------

#define ON_DEBUG(...)
#define OFF_DEBUG(...) __VA_ARGS__

#define builtin_unreachable_wrapper(debug_message) __builtin_unreachable()

#define msg_assert(bool_expression, message)

//---------------------------------------------------------------------------------------------------------------

#endif /* not defined(NDEBUG) */

//---------------------------------------------------------------------------------------------------------------

#define msg_bad_exit(bool_expression, message)                                                                         \
    do                                                                                                                 \
    {                                                                                                                  \
        if (not(bool_expression))                                                                                      \
        {                                                                                                              \
            std::cerr << RED BOLD "run program failed: " ON_DEBUG("\n")                                                \
                             RESET_CONSOLE_OUT WHITE ON_DEBUG(<< #bool_expression << "\n"                              \
                                                              << __FILE__ << ":" << __LINE__ << " [" << __func__       \
                                                              << "]\n" RED "message:\n") /* ON_DEBUG */                \
                BOLD << message                                                                                        \
                      << RESET_CONSOLE_OUT "\n";                                                                       \
            exit(EXIT_FAILURE);                                                                                        \
        }                                                                                                              \
    } while (0)

//---------------------------------------------------------------------------------------------------------------

#define msg_warning(bool_expression, message)                                                                          \
    do                                                                                                                 \
    {                                                                                                                  \
        if (not(bool_expression))                                                                                      \
        {                                                                                                              \
            std::cerr << YELLOW BOLD "warning: " ON_DEBUG("\n")                                                        \
                             RESET_CONSOLE_OUT WHITE ON_DEBUG(<< #bool_expression << "\n"                              \
                                                              << __FILE__ << ":" << __LINE__ << " [" << __func__       \
                                                              << "]\n" YELLOW "message:\n") /* ON_DEBUG */             \
                BOLD << message                                                                                        \
                      << RESET_CONSOLE_OUT "\n";                                                                       \
        }                                                                                                              \
    } while (0)

//---------------------------------------------------------------------------------------------------------------

#if defined(GRAPHVIZ)
#define ON_GRAPHVIZ(...) __VA_ARGS__
#else /* defined(GRAPHVIZ) */
#define ON_GRAPHVIZ(...)
#endif /* defined(GRAPHVIZ) */

//---------------------------------------------------------------------------------------------------------------
