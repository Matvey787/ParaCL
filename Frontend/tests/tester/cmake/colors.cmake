string(ASCII 27 Esc)

set(Color_Black   "${Esc}[30m")
set(Color_Red     "${Esc}[31m")
set(Color_Green   "${Esc}[32m")
set(Color_Yellow  "${Esc}[33m")
set(Color_Blue    "${Esc}[34m")
set(Color_Magenta "${Esc}[35m")
set(Color_Cyan    "${Esc}[36m")
set(Color_White   "${Esc}[37m")
set(Color_Reset   "${Esc}[0m")

set(Color_Bright_Black   "${Esc}[90m")
set(Color_Bright_Red     "${Esc}[91m")
set(Color_Bright_Green   "${Esc}[92m")
set(Color_Bright_Yellow  "${Esc}[93m")
set(Color_Bright_Blue    "${Esc}[94m")
set(Color_Bright_Magenta "${Esc}[95m")
set(Color_Bright_Cyan    "${Esc}[96m")
set(Color_Bright_White   "${Esc}[97m")

set(Bg_Black   "${Esc}[40m")
set(Bg_Red     "${Esc}[41m")
set(Bg_Green   "${Esc}[42m")
set(Bg_Yellow  "${Esc}[43m")
set(Bg_Blue    "${Esc}[44m")
set(Bg_Magenta "${Esc}[45m")
set(Bg_Cyan    "${Esc}[46m")
set(Bg_White   "${Esc}[47m")

set(Bg_Bright_Black   "${Esc}[100m")
set(Bg_Bright_Red     "${Esc}[101m")
set(Bg_Bright_Green   "${Esc}[102m")
set(Bg_Bright_Yellow  "${Esc}[103m")
set(Bg_Bright_Blue    "${Esc}[104m")
set(Bg_Bright_Magenta "${Esc}[105m")
set(Bg_Bright_Cyan    "${Esc}[106m")
set(Bg_Bright_White   "${Esc}[107m")

set(Bg_Default   "${Esc}[49m")

function(check_color_format color color_is_corect)
    if ("${color}" MATCHES "^${Esc}\\[[0-9;]+m$")
        set(${color_is_corect} TRUE PARENT_SCOPE)
    else()
        set(${color_is_corect} FALSE PARENT_SCOPE)
        message(WARNING "Color ${color} is not ANSII formated: 27[[0-9;]+m")
    endif()
endfunction()

function(print_colored_message message font_color background_color type)
    set(font_color_is_correct FALSE)
    set(background_color_is_correct FALSE)

    if (NOT "${font_color}" STREQUAL "")
        check_color_format(${font_color} font_color_is_correct)
    endif()

    if (NOT "${background_color}" STREQUAL "")
        check_color_format(${background_color} background_color_is_correct)
    endif()
    
    set(message_body "")

    if (font_color_is_correct AND background_color_is_correct)
        set(message_body "${font_color}${background_color}${message}")
    endif()

    if (font_color_is_correct AND NOT message_body)
        set(message_body "${font_color}${message}")
    endif()

    if (background_color_is_correct AND NOT message_body)
        set(message_body "${background_color}${message}")
    endif()

    if (NOT message_body)
        set(message_body "${message}")
    endif()

    set(message_body "${message_body}${Color_Reset}")

    if ("${type}" STREQUAL "")
        message(${type} "${message_body}")
    else()
        message("${message_body}")
    endif()
    
endfunction()

