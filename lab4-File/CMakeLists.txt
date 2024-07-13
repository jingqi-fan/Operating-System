cmake_minimum_required(VERSION 3.28)
project(lab4File_own)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab4File_own main.cpp
        fileSystem_Operations.cpp
        fileSystem.h
)
