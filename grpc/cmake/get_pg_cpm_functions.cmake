#
# @file get_pg_cpm_functions.cmake @author Xiyang.Jia (xiyang.jia@phigent.ai)
# @brief @version 0.8
#
# @copyright Copyright (c) 2022 phigent
#

set(CPM_FUNCTIONS_VERSION 1.6)
set(CPM_DOWNLOAD_LOCATION "${CMAKE_BINARY_DIR}")

if(NOT (DEFINED CPM_URL_PREFIX))
  set(CPM_URL_PREFIX $ENV{CPM_URL_PREFIX})
  if("${CPM_URL_PREFIX}" STREQUAL "")
    set(CPM_URL_PREFIX /artifactory/nfs/pkgs)
  endif()
endif()

function(download_file name)
  file(DOWNLOAD
       ${CPM_URL_PREFIX}/cpm_functions/v${CPM_FUNCTIONS_VERSION}/${name}
       ${CMAKE_BINARY_DIR}/${name})
endfunction(download_file)

function(download_files)
  message(
    STATUS
      "Downloading ${CPM_URL_PREFIX}/cpm_functions/v${CPM_FUNCTIONS_VERSION}/ to ${CPM_DOWNLOAD_LOCATION}"
  )
  download_file(pg_cpm_functions.cmake)
  download_file(version.cmake)
  download_file(version.c.in)
  download_file(version_tool.cpp)
endfunction(download_files)

if(EXISTS ${CPM_DOWNLOAD_LOCATION}/pg_cpm_functions.cmake)
  include(${CPM_DOWNLOAD_LOCATION}/pg_cpm_functions.cmake)
  if(CURRENT_CPM_FUNCTIONS_VERSION LESS CPM_FUNCTIONS_VERSION)
    message(
      STATUS
        "CURRENT_CPM_FUNCTIONS_VERSION ${CURRENT_CPM_FUNCTIONS_VERSION} is less than CPM_FUNCTIONS_VERSION ${CPM_FUNCTIONS_VERSION}}"
    )
    download_files()
  endif()
else()
  download_files()
endif()

include(${CPM_DOWNLOAD_LOCATION}/pg_cpm_functions.cmake)
message(
  STATUS "CURRENT_CPM_FUNCTIONS_VERSION=\"${CURRENT_CPM_FUNCTIONS_VERSION}\"")
