set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(converter)
add_subdirectory(token)

add_library(postfix_calculator_impl )

target_link_libraries(postfix_calculator_impl token_obj converter_obj)
target_include_directories(postfix_calculator_impl PUBLIC .)