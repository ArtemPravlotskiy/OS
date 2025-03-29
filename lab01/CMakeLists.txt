cmake_minimum_required(VERSION 3.30)
project(lab01_98)

set(CMAKE_CXX_STANDARD 98)


add_executable(lab01_winAPI_main src/main_winapi.cpp
        include/employee.h)

add_executable(lab01_winAPI_creator winAPI/creator.cpp
        include/employee.h)

add_executable(lab01_winAPI_reporter winAPI/reporter.cpp
        include/employee.h)

add_dependencies(lab01_winAPI_main lab01_winAPI_creator lab01_winAPI_reporter)



add_executable(lab01_thread_main src/main_thread.cpp
        include/employee.h)

add_executable(lab01_thread_creator src/creator.cpp
        include/employee.h)

add_executable(lab01_thread_reporter src/reporter.cpp
        include/employee.h)

add_dependencies(lab01_thread_main lab01_thread_creator lab01_thread_reporter)