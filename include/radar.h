#ifndef RADAR_H
#define RADAR_H

#include <stdint.h>

#define TRIGGER_PIN 17
#define ECHO_PIN 27

void initialize_radar(void);

void *radarPulse(void *arg);

void pulseIn(int gpio, int level, uint32_t tick);

#endif // RADAR_H
