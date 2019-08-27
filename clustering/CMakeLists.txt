cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME "floc")
project(${PROJECT_NAME})
SET(CMAKE_CXX_STANDARD 17)

set(CMAKE_BUILD_TYPE Debug)

set(MAIN
${CMAKE_SOURCE_DIR}/clustering/WSN_cluster.cpp 
)

set(SRC  
${CMAKE_SOURCE_DIR}/clustering/wsensor.cpp
${CMAKE_SOURCE_DIR}/clustering/wsn.cpp 
)

set(HDR
${CMAKE_SOURCE_DIR}/clustering/wsensor.h
${CMAKE_SOURCE_DIR}/clustering/wsn.h
)


add_executable(${PROJECT_NAME} ${MAIN} ${SRC} ${HDR} ${CORE})

target_link_libraries(${PROJECT_NAME} -lpthread -lpython2.7 -lboost_program_options)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)


