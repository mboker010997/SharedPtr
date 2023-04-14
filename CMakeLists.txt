cmake_minimum_required(VERSION 3.26)
project(SharedPtr)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(SharedPtr
        exe/shared_ptr.cpp
        exe/shared_ptr.hpp)
