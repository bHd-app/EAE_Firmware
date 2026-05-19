/**
 * @file cli.h
 * @brief Simple terminal menu for cooling setpoints.
 */

#ifndef CLI_H
#define CLI_H

typedef struct
{
    float temp_low_c;
    float temp_medium_c;
    float temp_high_c;
    float temp_critical_c;
} CoolingConfig_t;

CoolingConfig_t CLI_GetCoolingConfig(void);

#endif
