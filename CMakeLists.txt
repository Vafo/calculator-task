cmake_minimum_required(VERSION 3.15)

project(Calculator)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(compiler_flags INTERFACE)
target_compile_features(compiler_flags INTERFACE cxx_std_11)

# Include src folders
add_subdirectory(src)
add_subdirectory(test)

target_link_libraries(calculator_test calculator_impl)

include(FetchContent)

FetchContent_Declare(
    Catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v3.3.0
)

FetchContent_MakeAvailable(Catch2)

add_library(test_link INTERFACE)
target_link_libraries(test_link INTERFACE Catch2::Catch2WithMain)

add_executable(calculator_app main.cpp)
target_link_libraries(calculator_app calculator_impl)


