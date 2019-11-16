cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME "GeoNS")
project(${PROJECT_NAME})
SET(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE RELEASE)

find_package(Boost REQUIRED)

add_definitions(-DGenerate_Poisson=TRUE)

include_directories("/usr/include/python2.7")
include_directories("./core")
include_directories("./RL")
include_directories("./utils")

set(CORE 
${CMAKE_SOURCE_DIR}/core/node.cpp
${CMAKE_SOURCE_DIR}/core/trx.cpp
${CMAKE_SOURCE_DIR}/core/container.cpp
)

add_subdirectory(range_control)
add_subdirectory(path_selection)
add_subdirectory(clustering)
