cmake_minimum_required(VERSION 3.30)
project(lab05)

set(CMAKE_CXX_STANDARD 98)

set(GOOGLETEST_DIR ${CMAKE_SOURCE_DIR}/../external_libraries/googletest)
add_subdirectory(${GOOGLETEST_DIR} googletest)
add_executable(labtest tests/labtest.cpp)
target_link_libraries(labtest PRIVATE gtest gtest_main)

add_executable(server src/server.cpp
        src/utils.h)
add_executable(client src/client.cpp
        src/utils.h)
