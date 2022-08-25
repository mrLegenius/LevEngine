cmake_minimum_required(VERSION 3.23)
project(Glad)

add_library(glad
        include/glad/gl.h src/gl.c
        include/khr/khrplatform.h)

target_include_directories(glad PUBLIC include/)