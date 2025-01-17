#ifndef SENSOR_H
#define SENSOR_H

#include <stdint.h>

#define SENSOR_READ_INTERVAL 100
#define SENSOR_START_DISTANCE 0
#define SENSOR_POSITION_TOLERANCE 1

int sensor_init(void);
void *sensor_measure_distance_loop(void *args);
void sensor_received_pulse(int gpio, int level, uint32_t tick);
void get_distance();

#endif // SENSOR_H
