#include "config.h"
#include "servo.h"
#include "pid.h"
#include <stdint.h>
#include <pthread.h>
#include <pigpio.h>
#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>

static int SERVO_ID = 0;

void servo_init(ServoController *servo, int servo_pin, float position, float speed) {
    if (servo_pin < 0 || servo_pin > 99) {
        printf("Servo pin out of range\n");
        return;
    }

    if (position > SERVO_MAX_POSITION) {
        printf(
            "Servo position is greater than maximum allowed position (%d > %d)\nSetting the position to the maximum allowed position: %d\n",
            position, SERVO_MAX_POSITION, SERVO_MAX_POSITION);
        position = SERVO_MAX_POSITION;
    } else if (position < -SERVO_MIN_POSITION) {
        printf(
            "Servo position is less than minimum allowed position, setting the position to the minimum allowed position: %d\n",
            SERVO_MIN_POSITION);
        position = -SERVO_MIN_POSITION;
    }

    if (speed > SERVO_MAX_SPEED) {
        printf(
            "Servo speed is greater than maximum allowed speed, setting the speed to the maximum allowed speed: %d\n",
            SERVO_MAX_SPEED);

        speed = SERVO_MAX_SPEED;
    } else if (speed < -SERVO_MIN_SPEED) {
        printf("Servo speed is less than minimum allowed speed, setting the speed to the minimum allowed speed: %d\n",
               SERVO_MIN_SPEED);
        speed = -SERVO_MIN_SPEED;
    }

    servo->id = SERVO_ID++;
    servo->pin = servo_pin;
    servo->speed = speed;
    servo->pwm_value = SERVO_START_PWM;
    servo->position = position;

    gpioSetMode(servo->pin, PI_OUTPUT);
    gpioServo(servo->pin, servo->position);
}

void *servo_position_update_loop(void *args) {
    ControllerArgs *controllers = (ControllerArgs *) args;
    ServoController *servo = controllers->servo_controller;
    PIDController *pid = controllers->pid_controller;

    struct timeval current_time;
    gettimeofday(&servo->last_update, NULL);
    float target_position;
    while (1) {
        gettimeofday(&current_time, NULL);
        long elapsed_time = (current_time.tv_sec - servo->last_update.tv_sec) * 1000000L + (
                                current_time.tv_usec - servo->last_update.tv_usec);

        if (elapsed_time >= PID_UPDATE_INTERVAL) {
            if (servo->position >= SERVO_MAX_POSITION) {
                target_position = SERVO_MIN_POSITION;
            } else if (servo->position <= SERVO_MIN_POSITION) {
                target_position = SERVO_MAX_POSITION;
            }

            float pid_output = pid_calculate(pid, target_position, servo->position);
            if (pid_output > SERVO_MAX_POSITION) {
                pid_output = SERVO_MAX_POSITION;
            } else if (pid_output < SERVO_MIN_POSITION) {
                pid_output = SERVO_MIN_POSITION;
            }

            servo->position = pid_output;
            servo->last_update = current_time;
            gpioServo(servo->pin, (int) servo->position);
        }

        usleep(SERVO_UPDATE_INTERVAL * 1000);
    }

    return NULL;
}

int calculate_angle(int width, int angle_min, int angle_max, int min_width, int max_width) {
    return angle_min + ((width - min_width) * (angle_max - angle_min)) / (max_width - min_width);
}

// int MIN_WIDTH = 500;
// int MAX_WIDTH = 2500;
// const float SERVO_WIDTH_TIME = 2.0f;
// const float SERVO_TIME_PER_STEP = 0.1f;
// volatile int current_servo_angle = 90;
// extern volatile uint32_t terminate;
// extern volatile uint32_t pulse_width;
// extern pthread_mutex_t servo_mutex;
// pthread_mutex_t servo_angle_mutex = PTHREAD_MUTEX_INITIALIZER;
//
// static int calculate_num_steps(float total_time, const float time_per_step) {
//     return (int) ((MAX_WIDTH - MIN_WIDTH) / (total_time / time_per_step));
// }
//
// void initialize_servo(void) {
//     gpioSetMode(SERVO_PIN,PI_OUTPUT);
//     gpioServo(SERVO_PIN, 1500);
// }
//
// void *servoMovement(void) {
//     int new_min_width = 60;
//     int new_max_width = 150;
//     set_servo_range(new_min_width, new_max_width);
//     int width = MIN_WIDTH;
//     int step = calculate_num_steps(SERVO_WIDTH_TIME, SERVO_TIME_PER_STEP);
//
//     while (!terminate) {
//         gpioServo(SERVO_PIN, width);
//
//         pthread_mutex_lock(&servo_angle_mutex);
//         current_servo_angle = calculate_angle(width, new_min_width, new_max_width, MIN_WIDTH, MAX_WIDTH);
//         pthread_mutex_unlock(&servo_angle_mutex);
//
//         width += step;
//         if (width >= MAX_WIDTH || width <= MIN_WIDTH) {
//             step = -step;
//             width += step;
//         }
//         time_sleep(SERVO_TIME_PER_STEP);
//     }
//     cleanup_servo();
//     return NULL;
// }
//
//
// void set_servo_range(int angle_min, int angle_max) {
//     const int FULL_RANGE_MIN = 500;
//     const int FULL_RANGE_MAX = 2500;
//     const int MAX_ANGLE = 180;
//
//     if (angle_min < 0 || angle_max > MAX_ANGLE || angle_min >= angle_max) {
//         printf("Invalid angle range. Setting to default (0° to 180°).\n");
//         MIN_WIDTH = FULL_RANGE_MIN;
//         MAX_WIDTH = FULL_RANGE_MAX;
//         return;
//     }
//
//     MIN_WIDTH = FULL_RANGE_MIN + (angle_min * (FULL_RANGE_MAX - FULL_RANGE_MIN)) / MAX_ANGLE;
//     MAX_WIDTH = FULL_RANGE_MIN + (angle_max * (FULL_RANGE_MAX - FULL_RANGE_MIN)) / MAX_ANGLE;
//
//     printf("Servo range set: MIN_WIDTH = %d, MAX_WIDTH = %d\n", MIN_WIDTH, MAX_WIDTH);
// }
//
// void cleanup_servo(void) {
//     gpioServo(SERVO_PIN, 0);
// }
//
// int calculate_angle(int width, int angle_min, int angle_max, int min_width, int max_width) {
//     return angle_min + ((width - min_width) * (angle_max - angle_min)) / (max_width - min_width);
// }
