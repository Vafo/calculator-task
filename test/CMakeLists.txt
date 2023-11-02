set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Collect util tests
add_subdirectory(util)
# Collect src tests
add_library(src_test OBJECT postfix_test.cpp token_test.cpp)
target_include_directories(src_test PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(src_test compiler_flags test_link)

# Merge tests
add_executable(calculator_test $<TARGET_OBJECTS:src_test> $<TARGET_OBJECTS:util_test>)
target_link_libraries(calculator_test compiler_flags test_link)

set_target_properties(calculator_test 
    PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)