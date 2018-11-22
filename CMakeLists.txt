cmake_minimum_required(VERSION 3.12)
project(terminate_thread)

set(CMAKE_CXX_STANDARD 11)

add_executable(terminate_thread main.cpp)

target_link_libraries(terminate_thread pthread)