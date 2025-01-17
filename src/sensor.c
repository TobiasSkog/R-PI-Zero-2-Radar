#include <sensor.h>
#include <config.h>
#include <pigpio.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

static SensorController sensor;
static uint32_t start_tick;
static volatile uint32_t sensor_pulse_width = 0;
static pthread_mutex_t sensor_mutex = PTHREAD_MUTEX_INITIALIZER;

void sensor_received_pulse(const int gpio, const int level, const uint32_t tick) {
    if (gpio != SENSOR_TRIGGER_PIN) {
        return;
    }

    pthread_mutex_lock(&sensor_mutex);
    if (level == 1) {
        start_tick = tick;
    } else if (level == 0) {
        sensor_pulse_width = tick - start_tick;
    }
    pthread_mutex_unlock(&sensor_mutex);
}


void *sensor_measure_distance_loop(void *args) {
    while (!stop_threads) {
        get_distance();
        //update_console_display(angle, distance);
        gpioDelay(100000);
    }
    return NULL;
}

void get_distance(void) {
    pthread_mutex_lock(&sensor_mutex);
    const uint32_t pulse_width = sensor_pulse_width;
    pthread_mutex_unlock(&sensor_mutex);

    gpioWrite(SENSOR_TRIGGER_PIN, PI_HIGH);
    gpioDelay(10);
    gpioWrite(SENSOR_TRIGGER_PIN, PI_LOW);
    gpioDelay(6000);

    if (pulse_width > 0) {
        sensor.distance = (float)pulse_width / 58.0f;
    } else {
        sensor.distance = 0.0f;
    }

    printf("\rDistance: %fcm", sensor.distance);
    printf("\033[H\033[J");
    fflush(stdout);
}


int sensor_init() {
    sensor.distance = SENSOR_START_DISTANCE;
    sensor.threshold = SENSOR_POSITION_TOLERANCE;
    sensor.last_update = SAMPLE_TIME_S;

    gpioSetMode(SENSOR_TRIGGER_PIN, PI_OUTPUT);
    gpioSetMode(SENSOR_ECHO_PIN, PI_INPUT);
    gpioWrite(SENSOR_TRIGGER_PIN, PI_LOW);
    gpioSetAlertFunc(SENSOR_ECHO_PIN, sensor_received_pulse);

    return 0;
}

