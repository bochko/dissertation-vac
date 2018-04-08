cmake_minimum_required(VERSION 3.9)
project(fsysnotify)

set(CMAKE_CXX_STANDARD 11)

add_executable(fsysnotify main.cpp fsnotify.cpp fsnotify.h idir.cpp idir.h)