#ifndef CONFIG_H
#define CONFIG_H

#include <signal.h>

#define SENSOR_TRIGGER_PIN 17
#define SENSOR_ECHO_PIN 27
#define SERVO_PIN 18
#define SAMPLE_TIME_S 0.01f
#define SIMULATION_TIME_MAX 5.0f

extern volatile sig_atomic_t stop_threads;

// Sensor Controller
typedef struct {
  float distance;
  float threshold;
  float last_update;
} SensorController;

// Servo Controller
typedef struct {
  float speed;
  float position;
  float last_update;
} ServoController;

// PID Controller
typedef struct {
  float Kp;
  float Ki;
  float Kd;
  float lim_min;
  float lim_max;
  float lim_min_int;
  float lim_max_int;
  float integrator;
  float previous_error;
  float previous_measurement;
} PIDController;

int pid_init(PIDController *pid);
float pid_compute(PIDController *pid, float setpoint, float measurement, float dt);


#endif //CONFIG_H

