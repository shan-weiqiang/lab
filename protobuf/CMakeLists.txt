cmake_minimum_required(VERSION 3.12)
project(ProtobufDynamicExample)

# Find the required packages
find_package(absl REQUIRED)
find_package(GTest REQUIRED)
find_package(Protobuf CONFIG REQUIRED)


# Include the Protobuf headers and your build directory for the generated files
include_directories(${Protobuf_INCLUDE_DIRS})
include_directories(${CMAKE_CURRENT_BINARY_DIR})


# Specify the executable and link against the Protobuf libraries
add_executable(ProtobufDynamicExample app.cpp example.pb.cc)
target_link_libraries(ProtobufDynamicExample ${Protobuf_LIBRARIES})