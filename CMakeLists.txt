cmake_minimum_required(VERSION 3.14)
project(FHeap)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(GTest REQUIRED)

add_executable(fheap_test test/fheap_test.cpp)
target_link_libraries(fheap_test GTest::gtest GTest::gtest_main)
target_include_directories(fheap_test PRIVATE fheap)
