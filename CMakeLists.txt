cmake_minimum_required(VERSION 3.0)
project(radar C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
        main.c
        src/radar.c
        src/servo.c
        src/signal_handler.c
        src/utils.c
)

add_executable(radar ${SOURCES})

target_link_libraries(radar /usr/lib/libpigpio.so pthread)

# Unit Testing
# enable_testing()
# add_subdirectory(tests)
