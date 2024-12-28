#include "config.h"
#include <pthread.h>
#include <stdio.h>
#include <pigpio.h>
#include <unistd.h>
#include <signal.h>

pthread_t sensor_thread;
pthread_t servo_thread;
pthread_t limited_run_time_thread;

PIDController pid;
ServoController servo;
SensorController sensor;
ControllerArgs controllers;


int main(void) {
  if (gpioInitialise() < 0) {
    perror("Failed to initialize GPIO");
    gpioTerminate();
    return 1;
  }
  // Set up the PID Controller, Servo Controller and Sensor Controller
  system_setup(&pid, &servo, &sensor);
  controllers.servo_controller = &servo;
  controllers.pid_controller = &pid;

  // Create Threads
  pthread_create(&sensor_thread, NULL, sensor_measure_distance_loop, (void *) &sensor);
  pthread_create(&servo_thread, NULL, servo_position_update_loop, (void *) &controllers);
  //pthread_create(&limited_run_time_thread, NULL, limited_run_time, NULL); // REMOVE ME

  // Wait for Threads to complete
  pthread_join(sensor_thread, NULL);
  pthread_join(servo_thread, NULL);
  //pthread_join(limited_run_time_thread, NULL);

  //clear_console();

  printf("Exiting...\n");
  gpioTerminate();
  return 0;
}

void system_setup(PIDController *pid, ServoController *servo, SensorController *sensor) {
  pid_init(pid);
  servo_init(servo, SERVO_PIN, SERVO_START_POSITION, SERVO_START_SPEED);
  sensor_init(sensor, SENSOR_TRIGGER_PIN, SENSOR_TRIGGER_PIN, SENSOR_START_DISTANCE, SENSOR_POSITION_TOLERANCE);
  signal(SIGINT, system_shutdown);
  signal(SIGTERM, system_shutdown);
  signal(SIGHUP, system_shutdown);
}

// !!    LIMITED RUN TIME     !!
// !! ONLY USED FOR DEBUGGING !!
// void *limited_run_time(void) {
//   time_t start, end;
//   double elapsed;
//   start = time(NULL);
//   while (1) {
//     end = time(NULL);
//     elapsed = difftime(end, start);
//     //30 seconds run time test
//     if (elapsed > 120.0) {
//     } else {
//       usleep(100000);
//     }
//   }
//   return NULL;
// }


// ===== SIGNAL HANDLERS =====
//setup_signal_handlers();
// ========== RADAR ==========
//initialize_radar();
// ========== SERVO ==========
//initialize_servo();
// ===== CONSOLE DISPLAY =====
//initialize_console_display();
