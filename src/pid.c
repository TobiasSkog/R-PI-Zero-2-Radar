#include "config.h"

void pid_init(PIDController *pid) {
  pid->integrator = 0.0f;
  pid->previous_error = 0.0f;
  pid->differentiator = 0.0f;
  pid->previous_measurement = 0.0f;
  pid->out = 0.0f;
}

float pid_calculate(PIDController *pid, float setpoint, float measurement) {
  // Error signal
  const float error = setpoint - measurement;
  // Proportional
  const float proportional = error * pid->integrator;
  // Integral
  pid->integrator = pid->integrator + 0.5f * pid->Ki * pid->T * (error + pid->previous_error);
  // Anti-wind-up via dynamic integrator clamping
  float lim_min_integrator, lim_max_integrator;

  // Compute integrator limits
  if (pid->lim_max > proportional) {
    lim_max_integrator = pid->lim_max - proportional;
  } else {
    lim_max_integrator = 0.0f;
  }
  if (pid->lim_min < proportional) {
    lim_min_integrator = pid->lim_min - proportional;
  } else {
    lim_min_integrator = 0.0f;
  }
  // Clamp integrator
  if (pid->integrator > lim_max_integrator) {
    pid->integrator = lim_max_integrator;
  } else if (pid->integrator < lim_min_integrator) {
    pid->integrator = lim_min_integrator;
  }
  // Derivative (band-limited differentiator)
  pid->differentiator = (2.0f * pid->Kd * (measurement - pid->previous_measurement)
                         + (2.0f * pid->tau - pid->T) * pid->differentiator)
                        / (2.0f * pid->tau + pid->T);
  // Compute output and apply limits
  pid->out = proportional + pid->integrator + pid->differentiator;

  if (pid->out > pid->lim_max) {
    pid->out = pid->lim_max;
  } else if (pid->out < pid->lim_min) {
    pid->out = pid->lim_min;
  }
  // Store error and measurement for late user
  pid->previous_error = error;
  pid->previous_measurement = measurement;
  // Return controller output
  return pid->out;
}
