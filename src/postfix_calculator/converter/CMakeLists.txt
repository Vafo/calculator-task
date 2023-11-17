set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(converter_obj OBJECT postfix.cpp)

target_include_directories(converter_obj PUBLIC .)

target_link_libraries(converter_obj compiler_flags)