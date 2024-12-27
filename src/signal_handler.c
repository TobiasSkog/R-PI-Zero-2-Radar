#include "signal_handler.h"
#include <pigpio.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>

extern pthread_t radar_thread;
extern pthread_t servo_thread;
extern volatile uint32_t terminate;


void shutdown_handler(int signum) {
  printf("\nTidying up...\n");
  terminate = 1;
  pthread_cancel(radar_thread);
  pthread_cancel(servo_thread);

  gpioTerminate();
  exit(0);
}

void setup_signal_handlers(void) {
  terminate = 0;
  signal(SIGINT, shutdown_handler);
  signal(SIGTERM, shutdown_handler);
  signal(SIGHUP, shutdown_handler);
}
