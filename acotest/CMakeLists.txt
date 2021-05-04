cmake_minimum_required(VERSION 2.8.12)

project(libaco_test)
add_definitions(-std=c++17) 

set ( PROJECT_LINK_LIBS libaco.so )
set ( ACO_DIR libaco-master)
link_directories(/usr/local/lib)
include_directories(${ACO_DIR})

add_executable(test1 libacotest.cpp)
add_executable(test2 libaco_Eigen.cpp)

target_link_libraries(test1 ${PROJECT_LINK_LIBS} )
target_link_libraries(test2 ${PROJECT_LINK_LIBS} )
