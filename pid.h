/**
 * @file pid.h
 * @brief PID controller interface.
 */

#ifndef PID_H
#define PID_H

unsigned int PID_Controller(float current_temp_c, float target_temp_c);

#endif
