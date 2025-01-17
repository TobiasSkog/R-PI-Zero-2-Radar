cmake_minimum_required(VERSION 3.0)
project(radar C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/include)

set(SOURCES
        main.c
        src/config.c
        src/pid.c
        src/servo.c
        src/sensor.c
        src/radar_display.c
        include/config.h
        include/pid.h
        include/servo.h
        include/sensor.h
        include/radar_display.h
)

add_executable(radar ${SOURCES}
        include/radar_display.h
        src/radar_display.c)

target_link_libraries(radar /usr/lib/libpigpio.so pthread m)

# Unit Testing
# enable_testing()
# add_subdirectory(tests)
