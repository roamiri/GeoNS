cmake_minimum_required(VERSION 2.6)
set(PROJECT_NAME "pathSelection")
project(${PROJECT_NAME})

set(MAIN
${CMAKE_SOURCE_DIR}/path_selection/dist_path_IAB.cpp
)

set(SRC  
${CMAKE_SOURCE_DIR}/path_selection/iabn.cpp
${CMAKE_SOURCE_DIR}/path_selection/mmwavebs.cpp
)

set(HDR
${CMAKE_SOURCE_DIR}/path_selection/iabn.h
${CMAKE_SOURCE_DIR}/path_selection/mmwavebs.h
)

add_executable(${PROJECT_NAME} ${MAIN} ${SRC} ${HDR} ${CORE})

target_link_libraries(${PROJECT_NAME} -lpthread -lpython2.7 -lboost_program_options)

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
