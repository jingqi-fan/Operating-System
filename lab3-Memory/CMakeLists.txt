cmake_minimum_required(VERSION 3.28)
project(lab3Memory_Own)

set(CMAKE_CXX_STANDARD 17)

add_executable(lab3Memory_Own main.cpp
        PageTable.h
        Process.h
        VMemoryManagement.h)
