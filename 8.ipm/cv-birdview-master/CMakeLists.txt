cmake_minimum_required(VERSION 3.0.0)
project(Birdview CXX)

find_package(OpenCV 3.4.2 REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

find_package(Boost 1.67.0 REQUIRED)
include_directories(${Boost_INCLUDE_DIR})

set(SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB_RECURSE SOURCES ${SOURCE_DIR}/*.cpp)

add_executable(Birdview ${SOURCES} ${INCLUDES})
target_link_libraries(Birdview ${OpenCV_LIBS} ${Boost_LIBRARIES} tesseract)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(EXTRA_COMPILE_FLAGS "-Wall -Werror")