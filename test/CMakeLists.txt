set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(util)

add_executable(calculator_test postfix_test.cpp $<TARGET_OBJECTS:util_test>)
# target_link_libraries(calculator_test )
target_link_libraries(calculator_test compiler_flags test_link)

set_target_properties(calculator_test 
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)