/**
 * @file EAE_Firmware.c
 * @author Mahmoud Bahmani
 * @date May 18, 2026
 * @brief Cooling system control logic.
 */

#include <stdio.h>
#include <stdbool.h>

#include "canbus.h"
#include "cli.h"
#include "pid.h"

#define PUMP_SPEED_OFF          0U
#define PUMP_SPEED_LOW          40U
#define PUMP_SPEED_MEDIUM       70U
#define PUMP_SPEED_HIGH         100U

#define FAN_SPEED_OFF           0U
// #define FAN_SPEED_LOW           30U
// #define FAN_SPEED_MEDIUM        60U
#define FAN_SPEED_HIGH          100U


// enumeration for state machine
typedef enum
{
    COOLING_OFF,
    COOLING_NORMAL,
    COOLING_CRITICAL
} CoolingStateMachine_t;

/**
 * @brief Selects the current cooling system state.
 */
static CoolingStateMachine_t get_cooling_state(const Inputs_t *inputs, const CoolingConfig_t *config)
{
    CoolingStateMachine_t state;

    if (inputs->ignition_on == false)
    {
        state = COOLING_OFF;
    }
    else if (inputs->coolant_temp_c >= config->temp_critical_c)
    {
        state = COOLING_CRITICAL;
    }
    else
    {
        state = COOLING_NORMAL;
    }

    return state;
}

/**
 * @brief Converts cooling state to text.
 */
static const char *get_cooling_state_name(CoolingStateMachine_t state)
{
    const char *state_name = "UNKNOWN";

    if (state == COOLING_OFF)
    {
        state_name = "OFF";
    }
    else if (state == COOLING_NORMAL)
    {
        state_name = "NORMAL";
    }
    else if (state == COOLING_CRITICAL)
    {
        state_name = "CRITICAL";
    }

    return state_name;
}

/**
 * @brief Controls pump speed based on ignition state and coolant temperature.
 */
static void control_pump_speed(const Inputs_t *inputs, Outputs_t *outputs, const CoolingConfig_t *config)
{
    if (inputs->ignition_on == false)
    {
        outputs->pump_speed_percent = PUMP_SPEED_OFF;
    }
    else if (inputs->coolant_temp_c < config->temp_low_c)
    {
        outputs->pump_speed_percent = PUMP_SPEED_LOW;
    }
    else if (inputs->coolant_temp_c < config->temp_medium_c)
    {
        outputs->pump_speed_percent = PUMP_SPEED_MEDIUM;
    }
    else
    {
        outputs->pump_speed_percent = PUMP_SPEED_HIGH;
    }
}

/**
 * @brief Controls radiator fan speed using a basic PID output.
 */
static void control_fan_speed(const Inputs_t *inputs, Outputs_t *outputs, const CoolingConfig_t *config)
{
    if (inputs->ignition_on == false)
    {
        outputs->fan_speed_percent = FAN_SPEED_OFF;
    }
    else
    {
        outputs->fan_speed_percent = PID_Controller(inputs->coolant_temp_c, config->temp_medium_c);
    }

    /*
     * Previous threshold based fan logic.
     * Replaced by PID_Controller for a smoother fan output.
     *
     * else if (inputs->coolant_temp_c < config->temp_medium_c)
     * {
     *     outputs->fan_speed_percent = FAN_SPEED_OFF;
     * }
     * else if (inputs->coolant_temp_c < config->temp_high_c)
     * {
     *     outputs->fan_speed_percent = FAN_SPEED_MEDIUM;
     * }
     * else
     * {
     *     outputs->fan_speed_percent = FAN_SPEED_HIGH;
     * }
     */
}

/**
 * @brief Applies safety protection when coolant temperature is critical.
 */
#if 0
static void check_safety(const Inputs_t *inputs, Outputs_t *outputs)
{
    if (inputs->coolant_temp_c >= config->temp_critical_c)
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
#endif

/**
 * @brief Runs the cooling control logic.
 */
void run_cooling_logic(const Inputs_t *inputs, Outputs_t *outputs, const CoolingConfig_t *config)
{
    outputs->pump_speed_percent = PUMP_SPEED_OFF;
    outputs->fan_speed_percent = FAN_SPEED_OFF;
    outputs->safety_shutdown = false;

    control_pump_speed(inputs, outputs, config);
    control_fan_speed(inputs, outputs, config);

    if (get_cooling_state(inputs, config) == COOLING_CRITICAL)
    {
        outputs->pump_speed_percent = PUMP_SPEED_HIGH;
        outputs->fan_speed_percent = FAN_SPEED_HIGH;
        outputs->safety_shutdown = true;
    }

#if 0
    /* Previous flow before adding the state machine. */
    control_pump_speed(inputs, outputs);
    control_fan_speed(inputs, outputs);
    check_safety(inputs, outputs);
#endif
}

/**
 * @brief Prints cooling system status.
 */
static void print_status(const Inputs_t *inputs, const Outputs_t *outputs, const CoolingConfig_t *config)
{
    printf("Ignition: %s\n", inputs->ignition_on ? "ON" : "OFF");
    printf("Coolant Temperature: %.1f C\n", inputs->coolant_temp_c);
    printf("Cooling State: %s\n", get_cooling_state_name(get_cooling_state(inputs, config)));
    printf("Pump Speed: %u%%\n", outputs->pump_speed_percent);
    printf("Fan Speed: %u%%\n", outputs->fan_speed_percent);
    printf("Safety Shutdown: %s\n", outputs->safety_shutdown ? "ACTIVE" : "INACTIVE");
    CANBUS_Simulator(inputs, outputs);
    printf("-----------------------------------------------------------\n");
}

int main(void)
{
    CoolingConfig_t config = CLI_GetCoolingConfig();

    Inputs_t test_cases[] =
    {
        {false,25},
        {true, 30},
        {true, 38},
        {true, 47},
        {true, 53},
        {true, 58},
        {true, 61},
        {true, 80}
    };

    Outputs_t outputs;
    int number_of_tests = sizeof(test_cases) / sizeof(test_cases[0]);

    for (int i = 0; i < number_of_tests; i++)
    {
        run_cooling_logic(&test_cases[i], &outputs, &config);
        print_status(&test_cases[i], &outputs, &config);
    }

    return 0;
}
