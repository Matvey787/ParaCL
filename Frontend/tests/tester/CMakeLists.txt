project(Tester VERSION 1.0.0 LANGUAGES CXX)

include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/colors.cmake)
include(${CMAKE_CURRENT_SOURCE_DIR}/tester_config.cmake)






if(NOT DEFINED CTEST)
    option(CTEST "Enable ctest generation" OFF)
endif()

if(NOT DEFINED GTEST)
    option(GTEST "Enable gtest generation" OFF)
endif()






print_colored_message(
    "Added tester: ${TESTER_RELATIVE_PATH}"
    ${INFO_USER_FONT_COLOR_MESSAGE} 
    ${INFO_USER_BACK_COLOR_MESSAGE}
    ""
)






get_property(TESTER_INFO_ALREADY_PRINTED GLOBAL PROPERTY TESTER_INFO_FLAG)

if (CTEST OR GTEST AND NOT TESTER_INFO_ALREADY_PRINTED)
    print_colored_message(
        "NOTE: CHANGE TESTER CONFIGURATION in tester_config.cmake" 
        ${INFO_USER_FONT_COLOR_MESSAGE} 
        ${INFO_USER_BACK_COLOR_MESSAGE}
        ""
    )
    
    set_property(GLOBAL PROPERTY TESTER_INFO_FLAG TRUE)
endif()






if (CTEST)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/ctest.cmake)
endif()

if (GTEST)
    include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/gtest.cmake)
endif()





