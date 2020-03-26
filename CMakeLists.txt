cmake_minimum_required(VERSION 2.8)

PROJECT(Bench)

aux_source_directory(. DIR_SRCS)

add_executable(Bench ${DIR_SRCS})

find_package(Threads)
target_link_libraries (${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
