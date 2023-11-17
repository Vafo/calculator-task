set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(token_obj OBJECT token_concrete.cpp token_builder.cpp)

target_include_directories(token_obj PUBLIC .)

target_link_libraries(token_obj compiler_flags)