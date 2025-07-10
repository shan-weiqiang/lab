cmake_minimum_required(VERSION 3.10)
project(DependencyTest)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Build liba.so
add_library(a SHARED a.cpp)

# =============================================================================
# CASE #1: Understanding Shared Library Dependencies
# =============================================================================
# 
# When library 'b' is linked to library 'a' during compile time, the symbols 
# that depend on 'a' are already resolved and liba.so is added to the DT_NEEDED 
# section of 'b'. One might think that 'main' doesn't need to link to 'a' 
# anymore, but this is NOT true from the linker's perspective!
# 
# Check the linking command in build/CMakeFiles/main.dir/link.txt:
# /usr/bin/c++ -g CMakeFiles/main.dir/main.cpp.o -o main \
#   -Wl,-rpath,/mnt/cfs-software/algorithm/weiqiang.shan/pgsim/tmp/dep_test/build \
#   libb.so \
#   -Wl,-rpath-link,/mnt/cfs-software/algorithm/weiqiang.shan/pgsim/tmp/dep_test/build
# 
# There is no explicit linking to liba.so, but if we delete liba.so and run 
# this command, there will be a linking error since ld cannot find liba.so:
# 
# >>>>>>>>> SHELL OUTPUT >>>>>>>>
# ➜  build git:(logsim_v2) /usr/bin/c++ -g CMakeFiles/main.dir/main.cpp.o -o main \
#   -Wl,-rpath,/mnt/cfs-software/algorithm/weiqiang.shan/pgsim/tmp/dep_test/build \
#   libb.so \
#   -Wl,-rpath-link,/mnt/cfs-software/algorithm/weiqiang.shan/pgsim/tmp/dep_test/build
# /usr/bin/ld: warning: liba.so, needed by libb.so, not found (try using -rpath or -rpath-link)
# /usr/bin/ld: libb.so: undefined reference to `example_function_a'
# /usr/bin/ld: libb.so: undefined reference to `add_numbers'
# collect2: error: ld returned 1 exit status
# >>>>>>>>> END SHELL OUTPUT >>>>>>>>
# 
# The ld linker will resolve the DT_NEEDED section of libb.so to recursively 
# find dependent shared libraries, even if they are not present in the ld 
# command!
# 
# This proves that when a shared library depends on another shared library, 
# all the relevant symbols will be UNDEFINED whether this shared library is 
# linked or not linked during compile time!
# 
# The only difference is the DT_NEEDED section of the target shared library: 
# if linked during compile time, the dependent shared library will be added 
# to this section. Other than that, there is little difference. The ld linker 
# will search shared libraries in command parameters AND in the DT_NEEDED 
# section! So from CMake's point of view, it falsely seems that liba.so is 
# not needed, but under the hood, ld still requires the presence of liba.so 
# to build an executable!

# =============================================================================
# IMPORTANT FACTS ABOUT LIBRARY LINKING
# =============================================================================
# 
# 1. When building and linking static libraries or shared libraries, only 
#    header files are required; all symbols can be unresolved.
# 
# 2. When building and linking executables, all dependent libraries must be 
#    present to the compiler and linker! Including those recursively 
#    dependent libraries.
# 
# 3. Compiler and linker will search libraries according to:
#    - Command line arguments (decided by CMake)
#    - DT_NEEDED section (in shared libraries)
# =============================================================================

# =============================================================================
# CASE #1 IMPLEMENTATION: b links to a, main links to b
# =============================================================================
add_library(b SHARED b.cpp)
target_link_libraries(b PRIVATE a)
add_executable(main main.cpp)
target_link_libraries(main PRIVATE b)

# =============================================================================
# CASE #2: b NOT linked to a (COMMENTED OUT)
# =============================================================================
# Since 'b' is not linked to 'a', 'b' will still be built successfully, since 
# dynamic libraries do not need to be fully linked during compile time. But 
# when linking 'main', all symbols need to be resolved, and since liba is not 
# in the dependency list AND not in libb's DT_NEEDED section, there will be a 
# linking error.
# =============================================================================
# add_library(b SHARED b.cpp)
# add_executable(main main.cpp)
# target_link_libraries(main PRIVATE b)

# =============================================================================
# CASE #3: Explicit linking of both b and a to main (COMMENTED OUT)
# =============================================================================
# Add 'a' to the linking dependency; symbols are resolved in 'b' during 
# linking of the main executable.
# =============================================================================
# add_library(b SHARED b.cpp)
# add_executable(main main.cpp)
# target_link_libraries(main PRIVATE b a)

# =============================================================================
# CASE #4: Using INTERFACE linking (COMMENTED OUT)
# =============================================================================
# Now we link 'a' to 'b' using INTERFACE, which creates dependency 
# transmission. Even if 'main' doesn't link to 'a', 'a' will be added to 
# the dependency through 'b'.
# =============================================================================
# add_library(b SHARED b.cpp)
# target_link_libraries(b INTERFACE a)
# add_executable(main main.cpp)
# target_link_libraries(main PRIVATE b)

# =============================================================================
# EXPLICIT LINKING: This will always be successful
# =============================================================================
add_executable(main_explicit main.cpp)
target_link_libraries(main_explicit PRIVATE b a)