cmake_minimum_required(VERSION 3.16)
project(boost_lab)

find_package(Boost 1.39.0 REQUIRED)

if(Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/CmakeHelpers.cmake)

add_subdirectory(timer)
