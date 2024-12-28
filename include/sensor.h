#ifndef SENSOR_H
#define SENSOR_H
#include "config.h"
#include <stdint.h>

void sensor_received_pulse(int gpio, int level, uint32_t tick);
uint32_t get_distance();

#endif // SENSOR_H
