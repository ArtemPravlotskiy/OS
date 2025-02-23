cmake_minimum_required(VERSION 3.29)
project(Fibonacci)

set(CMAKE_CXX_STANDARD 98)

# Укажите путь к GoogleTest
set(GOOGLETEST_DIR ${CMAKE_SOURCE_DIR}/../external_libraries/googletest)

# Добавьте GoogleTest
add_subdirectory(${GOOGLETEST_DIR} googletest)

add_executable(Fibonacci
        src/main.cpp
        include/fibonacci.h
        src/fibonacci.cpp
)

include_directories(include)

add_executable(tests
        include/fibonacci.h
        src/fibonacci.cpp
        tests/tests.cpp
)

target_link_libraries(tests gtest)