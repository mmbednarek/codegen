cmake_minimum_required(VERSION 3.16)

set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

FetchContent_Declare(
        gtest
        GIT_REPOSITORY https://github.com/google/googletest
        GIT_TAG release-1.10.0
)
FetchContent_MakeAvailable(gtest)

add_executable(libmb_codegen_test codegen_test.cpp)
target_link_libraries(libmb_codegen_test LINK_PUBLIC libmb libmb_codegen gtest gtest_main)
