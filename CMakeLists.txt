cmake_minimum_required(VERSION 3.0)
project(radar C)

set(CMAKE_C_STANDARD 11)

add_executable(radar main.c)
target_link_libraries(radar /usr/lib/libpigpio.so rt)
