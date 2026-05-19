/**
 * @file CANBUS.h
 * @brief CAN bus simulator interface.
 */

#ifndef CANBUS_H
#define CANBUS_H

#include <stdbool.h>

typedef struct
{
    bool ignition_on;
    float coolant_temp_c;
} Inputs_t;

typedef struct
{
    unsigned int pump_speed_percent;
    unsigned int fan_speed_percent;
    bool safety_shutdown;
} Outputs_t;

void CANBUS_Simulator(const Inputs_t *inputs, const Outputs_t *outputs);

#endif
