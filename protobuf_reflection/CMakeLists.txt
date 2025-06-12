cmake_minimum_required(VERSION 3.10)
project(proto_reflection)

# Include FetchContent module
include(FetchContent)

# Declare protobuf dependency
FetchContent_Declare(
    protobuf
    GIT_REPOSITORY https://github.com/protocolbuffers/protobuf.git
    GIT_TAG v3.22.1  # Using a stable version
)

# Configure protobuf build options
set(protobuf_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(protobuf_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(protobuf_INSTALL OFF CACHE BOOL "" FORCE)

# Make protobuf available
FetchContent_MakeAvailable(protobuf)

# Generate protobuf files using the protoc built by FetchContent
add_custom_command(
    OUTPUT
        ${CMAKE_CURRENT_BINARY_DIR}/my_message.pb.h
        ${CMAKE_CURRENT_BINARY_DIR}/my_message.pb.cc
    COMMAND $<TARGET_FILE:protoc>
    ARGS --cpp_out=${CMAKE_CURRENT_BINARY_DIR}
         -I${CMAKE_CURRENT_SOURCE_DIR}
         ${CMAKE_CURRENT_SOURCE_DIR}/my_message.proto
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/my_message.proto protoc
    VERBATIM
)

# Add executable
add_executable(proto_reflection
    main.cc
    ${CMAKE_CURRENT_BINARY_DIR}/my_message.pb.cc
)

# Add include directories
target_include_directories(proto_reflection PRIVATE
    ${protobuf_SOURCE_DIR}/src
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Link against protobuf and protoc libraries
target_link_libraries(proto_reflection PRIVATE
    libprotobuf
)
