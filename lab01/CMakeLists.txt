cmake_minimum_required(VERSION 3.30)
project(lab01_98)

set(CMAKE_CXX_STANDARD 98)


add_executable(lab01_winAPI_main winAPI/main.cpp
        employee.h)

add_executable(lab01_winAPI_creator winAPI/creator.cpp
        employee.h)

add_executable(lab01_winAPI_reporter winAPI/reporter.cpp
        employee.h)

add_dependencies(lab01_winAPI_main lab01_winAPI_creator lab01_winAPI_reporter)



add_executable(lab01_thread_main thread/main.cpp
        employee.h)

add_executable(lab01_thread_creator thread/creator.cpp
        employee.h)

add_executable(lab01_thread_reporter thread/reporter.cpp
        employee.h)

add_dependencies(lab01_thread_main lab01_thread_creator lab01_thread_reporter)