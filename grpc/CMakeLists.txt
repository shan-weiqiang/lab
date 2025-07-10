cmake_minimum_required(VERSION 3.21)
project(grpc_tutorial
    VERSION 1.1.0
)
include(cmake/CustomMacro.cmake)
include(cmake/option_utils.cmake)

if (NOT (DEFINED ARCH_PLAT_VENDOR))
    # default for linux
    set(ARCH_PLAT_VENDOR x86_64-ubuntu-linux-gcc9.3.0)
endif ()

include(extern_dep.cmake)

CPMAddPackage(
        NAME json
        URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
)
include_directories(${json_SOURCE_DIR}/single_include/nlohmann)

execute_process(
    COMMAND ${protobuf_SOURCE_DIR}/bin/protoc
    -I ${CMAKE_CURRENT_LIST_DIR}
    --cpp_out=${CMAKE_CURRENT_LIST_DIR}
    --grpc_out=${CMAKE_CURRENT_LIST_DIR}
    --plugin=protoc-gen-grpc=/usr/local/bin/grpc_cpp_plugin
    route_guide.proto
    RESULT_VARIABLE PROTOC_RESULT
)

include(cmake/grpc.cmake)

add_executable(server route_guide_server.cc route_guide.grpc.pb.cc route_guide.pb.cc)
add_executable(client route_guide_client.cc route_guide.grpc.pb.cc route_guide.pb.cc)

target_link_libraries(server PRIVATE protobuf ${_GRPC_GRPCPP} gflags)
target_link_libraries(client PRIVATE protobuf ${_GRPC_GRPCPP} gflags)


