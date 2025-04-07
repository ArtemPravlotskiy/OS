cmake_minimum_required(VERSION 3.30)
project(lab02)

set(GOOGLETEST_DIR ${CMAKE_SOURCE_DIR}/../external_libraries/googletest)
add_subdirectory(${GOOGLETEST_DIR} googletest)
#target_link_libraries(run_tests PRIVATE gtest gtest_main)
#add_executable(run_tests tests/test_employee.cpp)

add_executable(main src/main.cpp)
set_target_properties(main PROPERTIES CXX_STANDARD 98)
#set_target_properties( PROPERTIES CXX_STANDARD 11)
