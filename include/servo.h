#ifndef SERVO_H
#define SERVO_H

#include <stdint.h>

#define SERVO_PIN 18

extern int MIN_WIDTH;
extern int MAX_WIDTH;
extern const float SERVO_WIDTH_TIME;
extern const float SERVO_TIME_PER_STEP;

void initialize_servo(void);

void *servoMovement(void *arg);

void set_servo_range(int angle_min, int angle_max);

void cleanup_servo(void);

//int calculate_num_steps(float total_time, float time_per_step);
//void set_servo_range(int angle_min, int angle_max);

#endif //SERVO_H
