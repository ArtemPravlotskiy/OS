cmake_minimum_required(VERSION 3.30)
project(lab02)

set(GOOGLETEST_DIR ${CMAKE_SOURCE_DIR}/../external_libraries/googletest)
add_subdirectory(${GOOGLETEST_DIR} googletest)
add_executable(testThread tests/test_processArray.cpp)
target_link_libraries(testThread PRIVATE gtest gtest_main)

add_executable(main src/main.cpp)
add_executable(mainThread src/mainThread.cpp
        src/processArray.cpp)

set_target_properties(main PROPERTIES CXX_STANDARD 98)
set_target_properties(mainThread PROPERTIES CXX_STANDARD 11)
