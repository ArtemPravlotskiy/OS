cmake_minimum_required(VERSION 3.30)
project(lab03)

set(GOOGLETEST_DIR ${CMAKE_SOURCE_DIR}/../external_libraries/googletest)
add_subdirectory(${GOOGLETEST_DIR} googletest)
#add_executable(testThread tests/test_processArray.cpp)
#target_link_libraries(testThread PRIVATE gtest gtest_main)

add_executable(mainWinAPI src/mainWinAPI.cpp)
add_executable(mainThread src/mainThread.cpp)

set_target_properties(mainWinAPI PROPERTIES CXX_STANDARD 98)
set_target_properties(mainThread PROPERTIES CXX_STANDARD 11)
