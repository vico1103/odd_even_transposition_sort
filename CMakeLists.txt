cmake_minimum_required(VERSION 3.15)
project(odd_even)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_COMPILER mpic++)

add_executable(odd_even ots.cpp ots.h)