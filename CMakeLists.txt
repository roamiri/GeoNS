cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME "mmWave_IAB")
project(${PROJECT_NAME})
SET(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

find_package(GSL REQUIRED)

add_definitions(-DGenerate_Poisson=TRUE)

include_directories("/usr/include/python2.7")
include_directories("./RL")

set(SRC node.cpp trx.cpp mmwavebs.cpp iabn.cpp wsn.cpp rlnetwork.cpp)
set(HDR common.h Signal.h painter.h plotter.h node.h trx.h mmwavebs.h container.h iabn.h wsn.h rlagent.h rlnetwork.h)


add_executable(${PROJECT_NAME} dist_path_IAB.cpp ${SRC} ${HDR})

target_link_libraries(${PROJECT_NAME} -lpthread -lpython2.7 -lboost_program_options GSL::gsl GSL::gslcblas)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
