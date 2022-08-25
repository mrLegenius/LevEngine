cmake_minimum_required(VERSION 3.23)
project(Stb)

add_library(stb stb.cpp include/stb_image.h)

target_include_directories(stb PUBLIC include/)