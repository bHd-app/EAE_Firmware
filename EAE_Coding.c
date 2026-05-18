/**
 * @file EAE_Coding.c
 * @author Mahmoud Bahmani
 * @date May 18, 2026
 * @brief Cooling system control logic.
 */

#include <stdio.h>
#include <stdbool.h>

#define TEMP_LOW_C              35.0f
#define TEMP_MEDIUM_C           45.0f
#define TEMP_HIGH_C             60.0f
#define TEMP_CRITICAL_C         75.0f

#define PUMP_SPEED_OFF          0U
#define PUMP_SPEED_LOW          40U
#define PUMP_SPEED_MEDIUM       70U
#define PUMP_SPEED_HIGH         100U

#define FAN_SPEED_OFF           0U
#define FAN_SPEED_LOW           30U
#define FAN_SPEED_MEDIUM        60U
#define FAN_SPEED_HIGH          100U

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

/**
 * @brief Controls pump speed based on ignition state and coolant temperature.
 */
static void control_pump_speed(const Inputs_t *inputs, Outputs_t *outputs)
{
    if (inputs->ignition_on == false)
    {
        outputs->pump_speed_percent = PUMP_SPEED_OFF;
    }
    else if (inputs->coolant_temp_c < TEMP_LOW_C)
    {
        outputs->pump_speed_percent = PUMP_SPEED_LOW;
    }
    else if (inputs->coolant_temp_c < TEMP_MEDIUM_C)
    {
        outputs->pump_speed_percent = PUMP_SPEED_MEDIUM;
    }
    else
    {
        outputs->pump_speed_percent = PUMP_SPEED_HIGH;
    }
}

/**
 * @brief Controls radiator fan speed based on coolant temperature.
 */
static void control_fan_speed(const Inputs_t *inputs, Outputs_t *outputs)
{
    if (inputs->ignition_on == false)
    {
        outputs->fan_speed_percent = FAN_SPEED_OFF;
    }
    else if (inputs->coolant_temp_c < TEMP_MEDIUM_C)
    {
        outputs->fan_speed_percent = FAN_SPEED_OFF;
    }
    else if (inputs->coolant_temp_c < TEMP_HIGH_C)
    {
        outputs->fan_speed_percent = FAN_SPEED_MEDIUM;
    }
    else
    {
        outputs->fan_speed_percent = FAN_SPEED_HIGH;
    }
}

/**
 * @brief Applies safety protection when coolant temperature is critical.
 */
static void check_safety(const Inputs_t *inputs, Outputs_t *outputs)
{
    if (inputs->coolant_temp_c >= TEMP_CRITICAL_C)
    {
        outputs->safety_shutdown = true;
        outputs->pump_speed_percent = PUMP_SPEED_HIGH;
        outputs->fan_speed_percent = FAN_SPEED_HIGH;
    }
    else
    {
        outputs->safety_shutdown = false;
    }
}

/**
 * @brief Runs the cooling control logic.
 */
void run_cooling_logic(const Inputs_t *inputs, Outputs_t *outputs)
{
    outputs->pump_speed_percent = PUMP_SPEED_OFF;
    outputs->fan_speed_percent = FAN_SPEED_OFF;
    outputs->safety_shutdown = false;

    control_pump_speed(inputs, outputs);
    control_fan_speed(inputs, outputs);
    check_safety(inputs, outputs);
}

/**
 * @brief Prints cooling system status.
 */
static void print_status(const Inputs_t *inputs, const Outputs_t *outputs)
{
    printf("Ignition: %s\n", inputs->ignition_on ? "ON" : "OFF");
    printf("Coolant Temperature: %.1f C\n", inputs->coolant_temp_c);
    printf("Pump Speed: %u%%\n", outputs->pump_speed_percent);
    printf("Fan Speed: %u%%\n", outputs->fan_speed_percent);
    printf("Safety Shutdown: %s\n", outputs->safety_shutdown ? "ACTIVE" : "INACTIVE");
    printf("-----------------------------\n");
}

int main(void)
{
    Inputs_t test_cases[] =
    {
        {false, 25.0f},
        {true, 30.0f},
        {true, 38.0f},
        {true, 50.0f},
        {true, 65.0f},
        {true, 80.0f}
    };

    Outputs_t outputs;
    int number_of_tests = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < number_of_tests; i++)
    {
        run_cooling_logic(&test_cases[i], &outputs);
        print_status(&test_cases[i], &outputs);
    }

    return 0;
}
