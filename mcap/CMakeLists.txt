cmake_minimum_required(VERSION 3.21)
project(mcap_tutorial
    VERSION 1.1.0
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

include(FetchContent)
FetchContent_Declare(
  mcap_builder
  GIT_REPOSITORY https://github.com/olympus-robotics/mcap_builder.git
  GIT_TAG main
)
FetchContent_MakeAvailable(mcap_builder)

execute_process(
    COMMAND protoc
    -I ${CMAKE_CURRENT_LIST_DIR}
    --cpp_out=${CMAKE_CURRENT_LIST_DIR}
    route_guide.proto
    RESULT_VARIABLE PROTOC_RESULT
)

# create your targets

find_package(protobuf)


add_executable(writer writer.cpp route_guide.pb.cc)
target_link_libraries(writer PUBLIC mcap protobuf::libprotobuf)


add_executable(reader reader.cpp route_guide.pb.cc)
target_link_libraries(reader PUBLIC mcap protobuf::libprotobuf)