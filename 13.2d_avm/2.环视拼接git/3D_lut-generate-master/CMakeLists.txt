cmake_minimum_required (VERSION 2.6)

set(CMAKE_BUILD_TYPE debug CACHE STRING "set build type to debug")

project (srv_lut_gen)

message(STATUS "the project_name is : " ${PROJECT_NAME})

add_subdirectory(src ../bin)
