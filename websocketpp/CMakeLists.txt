cmake_minimum_required(VERSION 3.16)
project(websocket)

set(WEBSOCKETPP_BOOST_LIBS system thread)
find_package(Boost 1.39.0 REQUIRED COMPONENTS ${WEBSOCKETPP_BOOST_LIBS})

add_executable(server server.cpp)
add_executable(client client.cpp)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})


if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIRS})
    target_link_libraries(server ${Boost_LIBRARIES})
    target_link_libraries(client ${Boost_LIBRARIES})
else()
    message(FATAL_ERROR "boost package not found")
endif()