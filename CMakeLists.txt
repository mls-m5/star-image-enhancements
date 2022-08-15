
cmake_minimum_required(VERSION 3.23)
project(star-stuff)

add_executable(
    star-stuff
    src/main.cpp
    )

target_compile_features(
    star-stuff
    PRIVATE
    cxx_std_17
    )

target_include_directories(
    star-stuff
    PRIVATE
    libs
    )

