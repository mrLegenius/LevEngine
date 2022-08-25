cmake_minimum_required(VERSION 3.23)
project(EnTT)

add_library(entt include/entt/entt.hpp src/entt.cpp)

target_include_directories(entt PUBLIC include/)