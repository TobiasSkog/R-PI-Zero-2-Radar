#include "servo.h"
#include <pigpio.h>
#include <stdio.h>
#include <unistd.h>

int MIN_WIDTH = 500;
int MAX_WIDTH = 2500;
const float SERVO_WIDTH_TIME = 2.0f;
const float SERVO_TIME_PER_STEP = 0.1f;
extern volatile uint32_t terminate;
extern volatile uint32_t pulse_width;
extern pthread_mutex_t servo_mutex;

static int calculate_num_steps(float total_time, const float time_per_step) {
    return (int) ((MAX_WIDTH - MIN_WIDTH) / (total_time / time_per_step));
}

void initialize_servo(void) {
    gpioSetMode(SERVO_PIN,PI_OUTPUT);
    gpioServo(SERVO_PIN, 1500);
}

void *servoMovement(void *arg) {
    set_servo_range(60, 150);
    int width = MIN_WIDTH;
    int step = calculate_num_steps(SERVO_WIDTH_TIME, SERVO_TIME_PER_STEP);
    while (!terminate) {
        gpioServo(SERVO_PIN, width);
        width += step;
        if (width >= MAX_WIDTH || width <= MIN_WIDTH) {
            step = -step;
            width += step;
        }
        time_sleep(SERVO_TIME_PER_STEP);
    }
    cleanup_servo();
    return NULL;
}


void set_servo_range(int angle_min, int angle_max) {
    const int FULL_RANGE_MIN = 500;
    const int FULL_RANGE_MAX = 2500;
    const int MAX_ANGLE = 180;

    if (angle_min < 0 || angle_max > MAX_ANGLE || angle_min >= angle_max) {
        printf("Invalid angle range. Setting to default (0° to 180°).\n");
        MIN_WIDTH = FULL_RANGE_MIN;
        MAX_WIDTH = FULL_RANGE_MAX;
        return;
    }

    MIN_WIDTH = FULL_RANGE_MIN + (angle_min * (FULL_RANGE_MAX - FULL_RANGE_MIN)) / MAX_ANGLE;
    MAX_WIDTH = FULL_RANGE_MIN + (angle_max * (FULL_RANGE_MAX - FULL_RANGE_MIN)) / MAX_ANGLE;

    printf("Servo range set: MIN_WIDTH = %d, MAX_WIDTH = %d\n", MIN_WIDTH, MAX_WIDTH);
}

void cleanup_servo(void) {
    gpioServo(SERVO_PIN, 0);
}
