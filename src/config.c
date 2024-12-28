#include "config.h"
#include <pigpio.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

void system_shutdown(pthread_t sensor_thread, pthread_t servo_thread) {
  printf("\nTidying up...\n");
  pthread_cancel(sensor_thread);
  pthread_cancel(servo_thread);
  gpioTerminate();
  exit(0);
}
