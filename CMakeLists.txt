
cmake_minimum_required(VERSION 3.23)
project(star-stuff)

find_package(Threads REQUIRED)
find_package(X11 REQUIRED)

add_executable(
    star-stuff
    src/main.cpp
    src/starlist.cpp
    src/startransform.cpp
    )

target_compile_features(
    star-stuff
    PRIVATE
    cxx_std_17
    )

target_link_libraries(
    star-stuff
    PRIVATE
    Threads::Threads
    ${X11_LIBRARIES}
    )

target_include_directories(
    star-stuff
    PRIVATE
    libs/CImg
    )

target_precompile_headers(
    star-stuff
    PRIVATE
    src/common.h
    )


file(
    COPY
    testdata
    DESTINATION
    .)
