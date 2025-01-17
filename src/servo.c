#include <servo.h>
#include <config.h>
#include <pigpio.h>
#include <stdio.h>
#include <unistd.h>

PIDController pid;

int servo_init(void) {
    //Pulse Frequency / Duty cycle: 50 Hz / 20 ms square wave

    gpioSetMode(SERVO_PIN, PI_ALT5);
    gpioServo(SERVO_PIN, (SERVO_MAX_POSITION + SERVO_MIN_POSITION) / 2);
    return pid_init(&pid);
}

void *servo_position_update_loop(void *args) {
    float desired_angle = 0.0f; // start at 0°
    float current_angle = 0.0f;
    float dt = 0.01f; // 10ms update time
    static int direction = 1;
    while (!stop_threads) {
        const float pwm_signal = pid_compute(&pid, desired_angle, current_angle, dt);
        gpioServo(SERVO_PIN, (int)pwm_signal);
        const float pulse_width = (float)gpioGetServoPulsewidth(SERVO_PIN);

        if (direction == 1 && current_angle + pulse_width > 180.0f) {
            direction = -1;
        } else if (direction == -1 && current_angle + pulse_width < 0.0f) {
            direction = 1;
        }

        current_angle = current_angle + (float)direction * pwm_signal;

        gpioDelay(10000 * SERVO_SPEED);
    }

    return NULL;
}

float calculate_measurement(const float input) {
    static float output = 0.0f;
    static const float alpha = 0.02f;

    output = (SAMPLE_TIME_S * input + output) / (1.0f * alpha * SAMPLE_TIME_S);

    return output;
}

int calculate_angle(const int width, const int angle_min, const int angle_max, const int min_width,
                    const int max_width) {
    return angle_min + (width - min_width) * (angle_max - angle_min) / (max_width - min_width);
}
