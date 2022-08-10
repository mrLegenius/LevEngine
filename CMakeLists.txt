cmake_minimum_required(VERSION 3.23)
project (LevEngine)

set(CMAKE_CXX_STANDARD 20)

add_executable(LevEngine main.cpp)

add_subdirectory(LevCore/external/GLFW)
