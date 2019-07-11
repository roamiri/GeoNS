cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME "mmWave_IAB")
project(${PROJECT_NAME})
SET(CMAKE_CXX_FLAGS "-std=c++0x")

add_definitions(-DGenerate_Poisson=TRUE)

include_directories("/usr/include/python2.7")

set(SRC mmwavebs.cpp manager.cpp mmwavebs.cpp painter.cpp)
set(HDR mmwavebs.h idgenerator.h common.h Signal.h ppunfix5.h manager.h painter.h plotter.h)

add_executable(${PROJECT_NAME} cdf_hop_cnt.cpp ${SRC} ${HDR})

target_link_libraries(${PROJECT_NAME} -lpthread -lpython2.7)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
