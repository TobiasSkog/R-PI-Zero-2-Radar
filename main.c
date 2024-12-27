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

  printf("GPIO initialization successful.\n");

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

// #include <pigpio.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <pthread.h>
// #include <signal.h>

//#define TRIGGER_PIN 17  // GPIO pin for Trigger
//#define ECHO_PIN 27  // GPIO pin for Echo
//#define SERVO_PIN 18 // GPIO pin for Servo

// #define MIN_WIDTH 500
// #define MAX_WIDTH 2500
// #define SERVO_WIDTH_TIME 3.0;
// #define SERVO_TIME_PER_STEP 0.1;
// #define RADAR_MAX_RANGE 38000
// #define DISTANCE_DIVISOR 58
// #define SERVO_DELAY_MS 100
// #define RADAR_TRIGGER_DURATION_US 10
// #define RADAR_CYCLE_DELAY_US 6000


//volatile uint32_t terminate = 0;
//volatile uint32_t startTick;
//volatile uint32_t pulseWidth;
//pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
//pthread_t radarThread, servoThread;

//void *servoMovement();

//void *radarPulse();

//void shutdown_handler(int signum);

//void pulseIn(int gpio, int level, uint32_t tick);

//int calculate_num_steps(float total_time, float time_per_step);

//void calculate_pulse_width_for_angle(int angle_min, int angle_max, int *min_width, int *max_width);
//
// int main(void) {
//     if (gpioInitialise() < 0) {
//         perror("Failed to initialize GPIO");
//         gpioTerminate();
//         return 1;
//     }
//     printf("GPIO initialization successful.\n");
//
//     // Set up signal handlers
//     gpioSetSignalFunc(SIGINT, shutdown_handler); // Ctrl+C
//     gpioSetSignalFunc(SIGTERM, shutdown_handler); // Termination request
//
//     // ============= RADAR ============
//     // gpioSetMode(TRIGGER_PIN, PI_OUTPUT);
//     // gpioSetMode(ECHO_PIN, PI_INPUT);
//     // gpioWrite(TRIGGER_PIN, PI_LOW);
//     // gpioSetAlertFunc(ECHO_PIN, pulseIn);
//
//     // ============ SERVO ============
//     //gpioSetMode(SERVO_PIN, PI_OUTPUT);
//
//     // Create Threads
//     pthread_create(&radarThread, NULL, radarPulse, NULL);
//     pthread_create(&servoThread, NULL, servoMovement, NULL);
//
//     // Wait for Threads to complete
//     pthread_join(radarThread, NULL);
//     pthread_join(servoThread, NULL);
//
//     // Stop sending pulses to the servo
//     //gpioServo(SERVO_PIN, 0);
//
//     // Terminate gpio correctly then exit the software
//     gpioTerminate();
//     printf("Exiting...\n");
//     return 0;
// }


// void *radarPulse() {
//     while (terminate == 0) {
//         gpioWrite(TRIGGER_PIN, PI_HIGH);
//         gpioDelay(10);
//         gpioWrite(TRIGGER_PIN, PI_LOW);
//         gpioDelay(6000);
//
//         pthread_mutex_lock(&mutex);
//         uint32_t duration = pulseWidth;
//         pthread_mutex_unlock(&mutex);
//
//         if (duration >= 38000) {
//             printf("Out of range\n");
//         } else {
//             uint32_t distance = duration / 58;
//             printf("Distance: %dcm  (%dm)\n", distance, distance / 100);
//         }
//
//         gpioDelay(1000000);
//     }
//     printf("Returning from radarPulse...\n");
//     return NULL;
// }
//
// void *servoMovement() {
//     int width = MIN_WIDTH;
//     float total_time = SERVO_WIDTH_TIME;
//     float time_per_step = SERVO_TIME_PER_STEP;
//     int step = calculate_num_steps(total_time, time_per_step);
//     while (terminate == 0) {
//         gpioServo(SERVO_PIN, width);
//         width += step;
//         if (width < MIN_WIDTH || width > MAX_WIDTH) {
//             step = -step;
//             width += step;
//         }
//
//         printf("width: %d, step: %d\n", width, step);
//         time_sleep(0.1);
//     }
//     printf("Returning from servoMovement...\n");
//     return NULL;
// }

// void pulseIn(int gpio, int level, uint32_t tick) {
//     if (level == 1) {
//         startTick = tick;
//     } else if (level == 0) {
//         pthread_mutex_lock(&mutex);
//         pulseWidth = tick - startTick;
//         pthread_mutex_unlock(&mutex);
//     }
// }

// Return the number of steps as the total time divided by time per step
//int calculate_num_steps(float total_time, float time_per_step) {
//    return (int) (total_time / time_per_step);
//}

// void shutdown_handler(int signum) {
//     printf("\nTidying up...\n");
//     terminate = 1;
//
//     pthread_join(radarThread, NULL);
//     pthread_join(servoThread, NULL);
//
//     gpioServo(SERVO_PIN, 0);
//     gpioTerminate();
//     printf("Exiting...\n");
//     exit(0);
// }

