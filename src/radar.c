#include "radar.h"
#include "utils.h"
#include <pigpio.h>
#include <pthread.h>
#include <stdio.h>

static volatile uint32_t radar_pulse_width = 0;
static uint32_t start_tick;
static pthread_mutex_t radar_mutex = PTHREAD_MUTEX_INITIALIZER;

extern volatile uint32_t terminate;

void pulseIn(int gpio, int level, uint32_t tick) {
    if (level == 1) {
        start_tick = tick;
    } else if (level == 0) {
        pthread_mutex_lock(&radar_mutex);
        radar_pulse_width = tick - start_tick;
        pthread_mutex_unlock(&radar_mutex);
    }
}

void initialize_radar(void) {
    gpioSetMode(TRIGGER_PIN, PI_OUTPUT);
    gpioSetMode(ECHO_PIN, PI_INPUT);
    gpioWrite(TRIGGER_PIN, PI_LOW);
    gpioSetAlertFunc(ECHO_PIN, pulseIn);
}

void *radarPulse(void *arg) {
    while (terminate == 0) {
        gpioWrite(TRIGGER_PIN, PI_HIGH);
        gpioDelay(10);
        gpioWrite(TRIGGER_PIN, PI_LOW);
        gpioDelay(6000);

        pthread_mutex_lock(&radar_mutex);
        uint32_t duration = radar_pulse_width;
        pthread_mutex_unlock(&radar_mutex);

        if (duration >= 38000) {
            printf("Out of range\n");
        } else {
            printf("Distance: %dcm  (%dm)\n", duration / 58, (duration / 58) / 100);
        }

        gpioDelay(6000);
    }
    return NULL;
}

uint32_t get_distance(void) {
    pthread_mutex_lock(&radar_mutex);
    uint32_t duration = radar_pulse_width;
    pthread_mutex_unlock(&radar_mutex);
    return duration / 58;
}

