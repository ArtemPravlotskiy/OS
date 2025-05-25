cmake_minimum_required(VERSION 3.30)
project(lab04)

set(GOOGLETEST_DIR ${CMAKE_SOURCE_DIR}/../external_libraries/googletest)
add_subdirectory(${GOOGLETEST_DIR} googletest)
add_executable(labtest tests/labtest.cpp)
target_link_libraries(labtest PRIVATE gtest gtest_main)

add_executable(receiver WinApi/mainWinAPI.cpp)
add_executable(sender WinApi/sender.cpp)

set_target_properties(receiver PROPERTIES CXX_STANDARD 98)
set_target_properties(sender PROPERTIES CXX_STANDARD 98)
