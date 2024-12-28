#ifndef SERVO_H
#define SERVO_H

#define SERVO_PIN 18

extern int MIN_WIDTH;
extern int MAX_WIDTH;
extern const float SERVO_WIDTH_TIME;
extern const float SERVO_TIME_PER_STEP;

void initialize_servo(void);

void *servoMovement(void *arg);

void set_servo_range(int angle_min, int angle_max);

void cleanup_servo(void);

#endif //SERVO_H
