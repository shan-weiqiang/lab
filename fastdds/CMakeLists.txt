cmake_minimum_required(VERSION 3.16)

project(fast)

set(CMAKE_BUILD_TYPE Debug)

set(FAST_DDS_INSTALL_PATH "/home/swq/Fast-DDS/install")

list(APPEND CMAKE_PREFIX_PATH "${FAST_DDS_INSTALL_PATH}/share/fastdds/cmake")
list(APPEND CMAKE_PREFIX_PATH "${FAST_DDS_INSTALL_PATH}/lib/cmake")
list(APPEND CMAKE_PREFIX_PATH "${FAST_DDS_INSTALL_PATH}/lib/foonathan_memory/cmake")

find_package(fastdds REQUIRED)
find_package(fastcdr REQUIRED)

file(GLOB CPP_FILE msg/*.cxx)

add_library(msg STATIC ${CPP_FILE})

target_link_libraries(msg PUBLIC fastdds fastcdr)

add_executable(pub pub.cpp)
add_executable(sub sub.cpp)
target_link_libraries(pub PUBLIC msg fastdds fastcdr)
target_link_libraries(sub PUBLIC msg fastdds fastcdr)
