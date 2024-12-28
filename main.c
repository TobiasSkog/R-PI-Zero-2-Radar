#include "radar.h"
#include "servo.h"
#include "signal_handler.h"
#include <pthread.h>
#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>

pthread_t radar_thread;
pthread_t servo_thread;
pthread_t terminator_thread;
volatile uint32_t terminate;

void *timedRunTime(void *arg);

int main(void) {
  if (gpioInitialise() < 0) {
    perror("Failed to initialize GPIO");
    gpioTerminate();
    return 1;
  }

  // ===== SIGNAL HANDLERS =====
  setup_signal_handlers();

  // ========== RADAR ==========
  initialize_radar();

  // ========== SERVO ==========
  initialize_servo();

  // Create Threads
  pthread_create(&radar_thread, NULL, radarPulse, NULL);
  pthread_create(&servo_thread, NULL, servoMovement, NULL);
  pthread_create(&terminator_thread, NULL, timedRunTime, NULL);

  // Wait for Threads to complete
  pthread_join(radar_thread, NULL);
  pthread_join(servo_thread, NULL);
  pthread_join(terminator_thread, NULL);

  printf("Exiting...\n");
  gpioTerminate();
  return 0;
}

void *timedRunTime(void *arg) {
  time_t start, end;
  double elapsed;
  start = time(NULL);
  while (!terminate) {
    end = time(NULL);
    elapsed = difftime(end, start);
    //30 seconds run time test
    if (elapsed > 30.0) {
      terminate = 1;
    } else {
      usleep(50000);
    }
  }
  return NULL;
}
