#!/bin/bash

protoc --proto_path=. --cpp_out=. example.proto
protoc --proto_path=. --descriptor_set_out=example.pb example.proto

cmake -S . -B build -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build