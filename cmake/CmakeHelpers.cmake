macro(init_executable target_name cpp_file)
    add_executable(${target_name} ${cpp_file})
    target_link_libraries(${target_name} ${Boost_LIBRARIES})
endmacro()