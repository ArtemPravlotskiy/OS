cmake_minimum_required(VERSION 3.30)
project(lab01_98)

set(CMAKE_CXX_STANDARD 98)

add_executable(lab01_98_main main.cpp
        employee.h)

add_executable(lab01_98_creator creator.cpp
        employee.h)

add_executable(lab01_98_reporter reporter.cpp
        employee.h)