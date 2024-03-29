cmake_minimum_required(VERSION 3.15)
project(codegen)

set(CMAKE_CXX_STANDARD 20)
option(LIBMB_CODEGEN_TEST_TARGET "adds test target for the library" OFF)

find_package(fmt REQUIRED)

include(FetchContent)

FetchContent_Declare(
        libmb
        GIT_REPOSITORY https://github.com/mmbednarek/mb
        GIT_TAG libmb-0.0.21
)
FetchContent_MakeAvailable(libmb)

if (LIBMB_CODEGEN_TEST_TARGET)
    add_subdirectory(tests)
endif(LIBMB_CODEGEN_TEST_TARGET)

add_library(libmb_codegen src/class.cpp src/definable.cpp src/expression.cpp src/statement.cpp src/writer.cpp src/lambda.cpp src/component.cpp src/destination.cpp)
target_include_directories(libmb_codegen PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(libmb_codegen libmb fmt)
