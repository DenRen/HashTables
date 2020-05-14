cmake_minimum_required(VERSION 3.14.5)
project(6_HashTable)

set(CMAKE_CXX_STANDARD 17)

add_executable(6_HashTable main.cpp opt_func.cpp opt_func.h HashFuncs.cpp HashFuncs.h)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")