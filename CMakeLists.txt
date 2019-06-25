cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME "mmWave_IAB")
project(PROJECT_NAME)
SET(CMAKE_CXX_FLAGS "-std=c++0x")



add_executable(PROJECT_NAME main.cpp )

target_link_libraries(PROJECT_NAME -lpthread)

install(TARGETS PROJECT_NAME RUNTIME DESTINATION bin)
