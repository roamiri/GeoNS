cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME "rc")
project(${PROJECT_NAME})
SET(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

find_package(GSL REQUIRED)

set(MAIN
${CMAKE_SOURCE_DIR}/range_control/RLRC.cpp 
)

set(SRC  
${CMAKE_SOURCE_DIR}/range_control/rlagent.cpp
${CMAKE_SOURCE_DIR}/range_control/rlnetwork.cpp 
)

set(HDR
${CMAKE_SOURCE_DIR}/range_control/rlagent.h
${CMAKE_SOURCE_DIR}/range_control/rlnetwork.h 
)


add_executable(${PROJECT_NAME} ${MAIN} ${SRC} ${HDR} ${CORE})

target_link_libraries(${PROJECT_NAME} -lpthread -lpython2.7 -lboost_program_options GSL::gsl GSL::gslcblas)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)

