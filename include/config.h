#ifndef CONFIG_H
#define CONFIG_H
#include <pthread.h>
#include <bits/types/struct_timeval.h>

#define SENSOR_TRIGGER_PIN 17
#define SENSOR_ECHO_PIN 27
#define SERVO_PIN 18

// Sensor definitions
#define SENSOR_READ_INTERVAL 100
#define SENSOR_START_DISTANCE 0
#define SENSOR_POSITION_TOLERANCE 1

// Servo definitions
#define SERVO_MAX_POSITION 2000//180
#define SERVO_MIN_POSITION 1000
#define SERVO_START_POSITION 1500
#define SERVO_START_SPEED 50
#define SERVO_MAX_SPEED 100
#define SERVO_MIN_SPEED 0
#define SERVO_START_PWM 0
#define SERVO_UPDATE_INTERVAL 100

// PID definitions
#define PID_UPDATE_INTERVAL 50000

// PIDController
typedef struct {
  // Controller gains
  float Kp;
  float Ki;
  float Kd;
  // Derivative low-pass filter time constant
  float tau;
  // Output limits
  float lim_min;
  float lim_max;
  // Sample time (seconds)
  float T;
  // Controller "memory"
  float integrator;
  float previous_error;
  float differentiator;
  float previous_measurement;
  // Controller output
  float out;
} PIDController;

// ServoController
typedef struct {
  int id;
  int pin;
  float speed;
  float position;
  int pwm_value;
  struct timeval last_update;
} ServoController;

// SensorController
typedef struct {
  int id;
  int trigger_pin;
  int echo_pin;
  float distance;
  float threshold;
  long last_update;
} SensorController;

// ControllerArgs
typedef struct {
  ServoController *servo_controller;
  PIDController *pid_controller;
} ControllerArgs;

// Initialize components
void pid_init(PIDController *pid);
void servo_init(ServoController *servo, int servo_pin, float position, float speed);
void sensor_init(SensorController *sensor, int sensor_trigger_pin, int sensor_echo_pin,
                 float distance, float threshold);

// Servo specific methods
void *servo_position_update_loop(void *args);

// Sensor specific methods (RADAR)
void *sensor_measure_distance_loop(void *args);

// Display specific methods

// System specific methods
void system_setup(PIDController *pid, ServoController *servo, SensorController *sensor);
void system_shutdown(pthread_t sensor_thread, pthread_t servo_thread);

// !! DEBUG METHODS !!
void *limited_run_time(void); // REMEMBER TO REMOVE ME!

#endif //CONFIG_H
