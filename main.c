#include <config.h>
#include <sensor.h>
#include <servo.h>
#include <pthread.h>
#include <stdio.h>
#include <pigpio.h>
#include <signal.h>
#include <stdlib.h>

pthread_t sensor_thread;
pthread_t servo_thread;
volatile sig_atomic_t stop_threads = 0;

void system_setup(void);
void system_shutdown(int signal_number);

int main(void) {
  if (gpioInitialise() < 0) {
    perror("Failed to initialize GPIO");
    gpioTerminate();
    return 1;
  }

  system_setup();

  // Create Threads
  pthread_create(&sensor_thread, NULL, sensor_measure_distance_loop, NULL); // (void *) &sensor);
  pthread_create(&servo_thread, NULL, servo_position_update_loop, NULL); // (void *) &controllers);

  // Wait for Threads to complete
  pthread_join(servo_thread, NULL);
  pthread_join(sensor_thread, NULL);

  //clear_console();
  printf("Exiting...\n");
  gpioTerminate();
  return 0;
}

void system_setup(void) {
  if (servo_init() == -1) {
    perror("Failed to initialize servo");
    system_shutdown(SIGTERM);
  }
  if (sensor_init() == -1) {
    perror("Failed to initialize sensor");
    system_shutdown(SIGTERM);
  }

  signal(SIGINT, system_shutdown);
  signal(SIGTERM, system_shutdown);
  signal(SIGHUP, system_shutdown);
}

void system_shutdown(int signal_number) {
  printf("\nTidying up...\n");
  stop_threads = 1;
  pthread_cancel(sensor_thread);
  pthread_cancel(servo_thread);
  gpioServo(SERVO_PIN, 0);
  gpioTerminate();
  exit(0);
}
