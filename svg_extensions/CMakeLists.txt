cmake_minimum_required(VERSION 3.8.2)

include(${CMAKE_CURRENT_LIST_DIR}/cmake_util/util.cmake)

include(${ul_CMAKE_DIRS}/cmake_util.cmake)

project(simple_svg_ext)

ul_include(defaults.cmake)

set(UL_ADDITIONAL_CLANG_FORMAT_DIRS "." CACHE STRING "" FORCE)
ul_include(clang-format.cmake)
ul_include(githooks.cmake)

add_library(${PROJECT_NAME} STATIC
    ../simple_svg_1.0.0.hpp
    chart_axes.hpp
    linechart.hpp
    histchart.hpp
    simple_svg_ext.hpp
    text.hpp
    )

target_include_directories(${PROJECT_NAME} PUBLIC ${ul_INCLUDE_DIRS})

ul_set_target_defaults(${PROJECT_NAME})

file(GLOB_RECURSE ${PROJECT_NAME}_INCLUDE_FILES "*.h*")

set(${PROJECT_NAME}_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/..
    CACHE INTERNAL "${PROJECT_NAME}: include directories" FORCE)
