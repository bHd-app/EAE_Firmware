/**
 * @file pid.c
 * @brief Basic PID controller example for cooling control.
 */

#include "pid.h"

#define PID_KP                  2.0f
#define PID_KI                  0.5f
#define PID_KD                  0.1f
#define PID_TARGET_TEMP_C       45.0f

/**
 * @brief Calculates a simple PID output from coolant temperature.
 */
unsigned int PID_Controller(float current_temp_c)
{
    float error = current_temp_c - PID_TARGET_TEMP_C;
    float integral = error;
    float derivative = error;
    float output = (PID_KP * error) + (PID_KI * integral) + (PID_KD * derivative);

    if (output < 0.0f)
    {
        output = 0.0f;
    }
    else if (output > 100.0f)
    {
        output = 100.0f;
    }

    return (unsigned int)output;
}
