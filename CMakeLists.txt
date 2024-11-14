cmake_minimum_required(VERSION 3.22)
project(renderers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(glfw)

add_executable(main src/main.cpp glad/src/glad.c)
target_link_libraries(main glfw)
target_include_directories(main PUBLIC glad/include)