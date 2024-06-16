
file(GLOB CPP_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
foreach(CPP_FILE ${CPP_FILES})
    message(STATUS "file abs path: ${CPP_FILE}")
    get_filename_component(TARGET_NAME ${CPP_FILE} NAME_WE)
    message(STATUS "file name is : ${TARGET_NAME}")
    get_filename_component(FILE_NAME ${CPP_FILE} NAME)
    message(STATUS "file with extension: ${FILE_NAME}")
    init_executable(${TARGET_NAME} ${FILE_NAME})
    
endforeach()