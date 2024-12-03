cmake_minimum_required(VERSION 3.22)
project(renderers)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(glfw)

add_executable(raytracer src/raytracer/main.cpp src/raytracer/raytracer.cpp glad/src/glad.c)
target_link_libraries(raytracer glfw)
target_include_directories(raytracer PUBLIC glad/include include)

add_executable(rasterizer src/rasterizer/main.cpp src/rasterizer/rasterizer.cpp glad/src/glad.c)
target_link_libraries(rasterizer glfw)
target_include_directories(rasterizer PUBLIC glad/include include)
