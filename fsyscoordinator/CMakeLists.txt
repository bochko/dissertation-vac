cmake_minimum_required(VERSION 3.9)
project(fsyscoord)

set(CMAKE_CXX_STANDARD 17)

add_executable(fsyscoord main.cpp fsyscoord.h ansic_log.cpp ansic_log.h)