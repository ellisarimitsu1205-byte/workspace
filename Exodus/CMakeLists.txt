cmake_minimum_required(VERSION 3.16)
project(snake CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(snake_lib src/snake.cpp)
target_include_directories(snake_lib PUBLIC include)

add_executable(snake src/main.cpp)
target_link_libraries(snake PRIVATE snake_lib)

add_executable(tests tests/test_snake.cpp)
target_link_libraries(tests PRIVATE snake_lib)