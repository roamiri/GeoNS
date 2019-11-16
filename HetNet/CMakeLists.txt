cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME "HetNet")
project(${PROJECT_NAME})

set(MAIN
${CMAKE_SOURCE_DIR}/HetNet/hetnet_driver.cpp
)

set(SRC  
${CMAKE_SOURCE_DIR}/HetNet/hetnet.cpp
${CMAKE_SOURCE_DIR}/HetNet/macrobs.cpp
${CMAKE_SOURCE_DIR}/HetNet/smallbs.cpp
${CMAKE_SOURCE_DIR}/HetNet/user.cpp
)

set(HDR
${CMAKE_SOURCE_DIR}/HetNet/hetnet.h
${CMAKE_SOURCE_DIR}/HetNet/macrobs.h
${CMAKE_SOURCE_DIR}/HetNet/smallbs.h
${CMAKE_SOURCE_DIR}/HetNet/user.h
${CMAKE_SOURCE_DIR}/HetNet/blockage.h
)

add_executable(${PROJECT_NAME} ${MAIN} ${SRC} ${HDR} ${CORE})

target_link_libraries(${PROJECT_NAME} -lpthread -lboost_program_options)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
