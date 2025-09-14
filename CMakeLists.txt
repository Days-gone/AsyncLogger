cmake_minimum_required(VERSION 3.28)
project(AsyncLogger)
set(CMAKE_CXX_STANDARD 17)

# Add include
include_directories(./include)

# Main application
add_executable(main src/main.cpp)

# Tests
enable_testing()
find_package(GTest CONFIG REQUIRED)

add_executable(test_lib tests/test_lib.cpp)
target_link_libraries(test_lib PRIVATE GTest::gtest GTest::gtest_main pthread)

add_executable(test_queue tests/test_queue.cpp)
target_link_libraries(test_queue PRIVATE GTest::gtest GTest::gtest_main pthread)

add_executable(test_logger tests/test_logger.cpp)
target_link_libraries(test_logger PRIVATE GTest::gtest GTest::gtest_main pthread)

add_test(NAME test_lib COMMAND test_lib)
add_test(NAME test_queue COMMAND test_queue)
add_test(NAME test_logger COMMAND test_logger)