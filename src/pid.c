#include <pid.h>
#include <config.h>
#include <servo.h>

int pid_init(PIDController *pid) {
    pid->Kp = PID_KP;
    pid->Ki = PID_KI;
    pid->Kd = PID_KD;
    pid->lim_min = PID_LIM_MIN;
    pid->lim_max = PID_LIM_MAX;
    pid->lim_min_int = PID_LIM_MIN_INT;
    pid->lim_max_int = PID_LIM_MAX_INT;
    pid->integrator = 0.0f;
    pid->previous_error = 0.0f;
    pid->previous_measurement = 0.0f;

    return 0;
}

float pid_compute(PIDController *pid, const float setpoint, const float measurement, const float dt) {
    const float error = setpoint - measurement;

    // Proportional term
    const float proportional = pid->Kp * error;

    // Integral term with clamping (anti-windup)
    pid->integrator += pid->Ki * error * dt;
    if (pid->integrator > pid->lim_max_int) {
        pid->integrator = pid->lim_max_int;
    } else if (pid->integrator < pid->lim_min_int) {
        pid->integrator = pid->lim_min_int;
    }

    // Derivative term (avoiding derivative kick)
    const float derivative = pid->Kd * (measurement - pid->previous_measurement) / dt;

    // Compute final PID output
    float output = proportional + pid->integrator - derivative;

    // Constrain output to servo pulse width range (500-2500 µs)
    if (output > SERVO_MAX_POSITION) output = SERVO_MAX_POSITION;
    if (output < SERVO_MIN_POSITION) output = SERVO_MIN_POSITION;

    // Store previous values
    pid->previous_error = error;
    pid->previous_measurement = measurement;

    return output;
}
