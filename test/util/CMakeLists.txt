set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(util_test OBJECT container_test.cpp shared_ptr_test.cpp)

target_include_directories(util_test PUBLIC ${CMAKE_SOURCE_DIR}/src/util)

target_link_libraries(util_test compiler_flags test_link)