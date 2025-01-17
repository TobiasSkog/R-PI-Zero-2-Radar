#ifndef SERVO_H
#define SERVO_H

#define SERVO_MAX_POSITION 2500
#define SERVO_MIN_POSITION 500
#define SERVO_SPEED 50


void *servo_position_update_loop(void *args);
int servo_init(void);

#endif //SERVO_H

