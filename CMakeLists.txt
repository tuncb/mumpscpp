# define minimum cmake version
cmake_minimum_required(VERSION 2.6.2)
PROJECT(mumpscpp)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
 
add_subdirectory(./projects/tester)