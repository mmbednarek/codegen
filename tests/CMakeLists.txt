cmake_minimum_required(VERSION 3.15)

find_package(GTest REQUIRED)

add_executable(libmb_codegen_test codegen_test.cpp)
target_link_libraries(libmb_codegen_test LINK_PUBLIC libmb libmb_codegen gtest gtest_main)
