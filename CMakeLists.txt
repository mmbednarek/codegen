cmake_minimum_required(VERSION 3.17)
project(codegen)

set(CMAKE_CXX_STANDARD 17)
option(LIBMB_CODEGEN_TEST_TARGET "adds test target for the library" ON)

include(FetchContent)

FetchContent_Declare(
        libmb
        GIT_REPOSITORY https://github.com/mmbednarek/mb
        GIT_TAG libmb-0.0.16
)
FetchContent_MakeAvailable(libmb)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG 7.1.3
)
FetchContent_MakeAvailable(fmt)

if (LIBMB_CODEGEN_TEST_TARGET)
    add_subdirectory(tests)
endif(LIBMB_CODEGEN_TEST_TARGET)

add_library(libmb_codegen src/class.cpp src/definable.cpp src/expression.cpp src/statement.cpp src/writer.cpp src/lambda.cpp src/component.cpp)
target_include_directories(libmb_codegen PUBLIC $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)
target_link_libraries(libmb_codegen libmb fmt)
