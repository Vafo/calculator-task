
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(calculator_impl postfix.cpp token.cpp)

target_include_directories(calculator_impl PUBLIC util)
target_include_directories(calculator_impl PUBLIC .)

target_link_libraries(calculator_impl compiler_flags)