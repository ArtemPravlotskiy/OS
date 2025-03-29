cmake_minimum_required(VERSION 3.30)
project(lab01)

set(GOOGLETEST_DIR ${CMAKE_SOURCE_DIR}/../external_libraries/googletest)

add_subdirectory(${GOOGLETEST_DIR} googletest)

add_executable(lab01_winAPI_main src/main_winapi.cpp)
add_executable(lab01_creator src/creator.cpp)
add_executable(lab01_reporter src/reporter.cpp)
add_executable(lab01_thread_main src/main_thread.cpp)

set_target_properties(lab01_winAPI_main PROPERTIES CXX_STANDARD 98)
set_target_properties(lab01_thread_main PROPERTIES CXX_STANDARD 11)

add_dependencies(lab01_winAPI_main lab01_creator lab01_reporter)
add_dependencies(lab01_thread_main lab01_creator lab01_reporter)

target_link_libraries(tests gtest)