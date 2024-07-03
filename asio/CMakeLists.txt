cmake_minimum_required(VERSION 3.16)
project(lab)

set(CMAKE_BUILD_TYPE Debug)

# find_package(Boost 1.39.0 REQUIRED)

# if(Boost_FOUND)
#     include_directories(${Boost_INCLUDE_DIRS})
# endif()

include(${CMAKE_CURRENT_SOURCE_DIR}/../cmake/CmakeHelpers.cmake)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/asio-1.30.2/include)

add_subdirectory(timer)
add_subdirectory(sockets)

