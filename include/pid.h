#ifndef PID_H
#define PID_H

#include "config.h"

float pid_calculate(PIDController *pid, float setpoint, float measurement);

#endif //PID_H
