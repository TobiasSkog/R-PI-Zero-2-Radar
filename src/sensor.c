#include "sensor.h"
#include "radar_display.h"
#include <pigpio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static uint32_t start_tick;
static int SENSOR_ID = 0;
static volatile uint32_t SENSOR_PULSE_WIDTH = 0;
static pthread_mutex_t SENSOR_MUTEX = PTHREAD_MUTEX_INITIALIZER;

void sensor_received_pulse(int gpio, int level, uint32_t tick) {
    if (level == 1) {
        start_tick = tick;
    } else if (level == 0) {
        pthread_mutex_lock(&SENSOR_MUTEX);
        SENSOR_PULSE_WIDTH = tick - start_tick;
        pthread_mutex_unlock(&SENSOR_MUTEX);
    }
}

void sensor_init(SensorController *sensor, int sensor_trigger_pin, int sensor_echo_pin, float distance,
                 float threshold) {
    sensor->trigger_pin = sensor_trigger_pin;
    sensor->echo_pin = sensor_echo_pin;
    sensor->id = SENSOR_ID++;
    sensor->distance = distance;
    sensor->threshold = threshold;

    gpioSetMode(sensor->trigger_pin, PI_OUTPUT);
    gpioSetMode(sensor->echo_pin, PI_INPUT);
    gpioWrite(sensor->trigger_pin, PI_LOW);
    gpioSetAlertFunc(sensor->echo_pin, sensor_received_pulse);
}


void *sensor_measure_distance_loop(void *args) {
    SensorController *sensor = (SensorController *) args;

    while (1) {
        pthread_mutex_lock(&SENSOR_MUTEX);
        uint32_t distance = get_distance(sensor->trigger_pin);
        pthread_mutex_unlock(&SENSOR_MUTEX);
        //update_console_display(angle, distance);
        printf("distance: %f cm\n", distance);
        gpioDelay(100000);
    }
    return NULL;
}

uint32_t get_distance(int trigger_pin) {
    gpioWrite(trigger_pin, PI_HIGH);
    gpioDelay(10);
    gpioWrite(trigger_pin, PI_LOW);
    gpioDelay(6000);
    return SENSOR_PULSE_WIDTH / 58;
}


// //uint32_t distance_centimeter = SENSOR_PULSE_WIDTH / 58;
// static volatile uint32_t radar_pulse_width = 0;
// static uint32_t start_tick;
// static pthread_mutex_t radar_mutex = PTHREAD_MUTEX_INITIALIZER;
//
// extern volatile uint32_t terminate;
//
// uint32_t get_distance(void);
//
// void pulseIn(int gpio, int level, uint32_t tick) {
//     if (level == 1) {
//         start_tick = tick;
//     } else if (level == 0) {
//         pthread_mutex_lock(&radar_mutex);
//         radar_pulse_width = tick - start_tick;
//         pthread_mutex_unlock(&radar_mutex);
//     }
// }
//
// void initialize_radar(void) {
//     gpioSetMode(TRIGGER_PIN, PI_OUTPUT);
//     gpioSetMode(ECHO_PIN, PI_INPUT);
//     gpioWrite(TRIGGER_PIN, PI_LOW);
//     gpioSetAlertFunc(ECHO_PIN, pulseIn);
// }
//
// void *radarPulse(void) {
//     while (!terminate) {
//         uint32_t distance = get_distance();
//
//         int angle;
//         pthread_mutex_lock(&servo_angle_mutex);
//         angle = current_servo_angle;
//         pthread_mutex_unlock(&servo_angle_mutex);
//
//         update_console_display(angle, distance);
//
//         gpioDelay(100000);
//     }
//     return NULL;
// }
//
// uint32_t get_distance(void) {
//     gpioWrite(TRIGGER_PIN, PI_HIGH);
//     gpioDelay(10);
//     gpioWrite(TRIGGER_PIN, PI_LOW);
//
//     // gpioDelay(6000);
//     // uint32_t distance = radar_pulse_width;
//     // radar_pulse_width = 0;
//
//     int timeout = 6000;
//     while (timeout-- > 0) {
//         pthread_mutex_lock(&radar_mutex);
//         if (radar_pulse_width > 0) {
//             uint32_t distance = radar_pulse_width;
//             radar_pulse_width = 0;
//             pthread_mutex_unlock(&radar_mutex);
//             // printf("\rDistance: %dcm  (%dm)", distance / 58, (distance / 58) / 100);
//             // fflush(stdout);
//             return distance / 58;
//         }
//         pthread_mutex_unlock(&radar_mutex);
//         gpioDelay(100);
//     }
//
//     //return distance / 58;
//     return 0;
// }
//
//
// //pthread_mutex_lock(&radar_mutex);
// //uint32_t duration = radar_pulse_width;
// //pthread_mutex_unlock(&radar_mutex);
//
// //distance >= 655
// //duration >= 38000
// // Change this to update a value to visualize the radar display
// //printf("\rDistance: %dcm  (%dm)", duration / 58, (duration / 58) / 100);
// //fflush(stdout);
