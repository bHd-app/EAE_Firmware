/**
 * @file EAE_Coding.c
 * @author Mahmoud Bahmani
 * @date May 18, 2026
 * @brief Cooling system control logic.
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#define TEMP_LOW_C              35.0f
#define TEMP_MEDIUM_C           45.0f
#define TEMP_HIGH_C             60.0f
#define TEMP_CRITICAL_C         75.0f

#define PUMP_SPEED_OFF          0U
#define PUMP_SPEED_LOW          40U
#define PUMP_SPEED_MEDIUM       70U
#define PUMP_SPEED_HIGH         100U

#define FAN_SPEED_OFF           0U
// #define FAN_SPEED_LOW           30U
// #define FAN_SPEED_MEDIUM        60U
#define FAN_SPEED_HIGH          100U

#define PID_KP                  2.0f
#define PID_KI                  0.5f
#define PID_KD                  0.1f
#define PID_TARGET_TEMP_C       45.0f

#define CAN_INPUT_FRAME_ID      0x100U
#define CAN_OUTPUT_FRAME_ID     0x101U
#define CAN_FRAME_DATA_SIZE     8U

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
 * @brief Basic PID example for cooling control.
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
 * @brief Controls radiator fan speed using a basic PID output.
 */
static void control_fan_speed(const Inputs_t *inputs, Outputs_t *outputs)
{
    if (inputs->ignition_on == false)
    {
        outputs->fan_speed_percent = FAN_SPEED_OFF;
    }
    else
    {
        outputs->fan_speed_percent = PID_Controller(inputs->coolant_temp_c);
    }

    /*
     * Previous threshold based fan logic.
     * Replaced by PID_Controller for a smoother fan output.
     *
     * else if (inputs->coolant_temp_c < TEMP_MEDIUM_C)
     * {
     *     outputs->fan_speed_percent = FAN_SPEED_OFF;
     * }
     * else if (inputs->coolant_temp_c < TEMP_HIGH_C)
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
 * @brief Converts cooling inputs and outputs to simulated CAN frames.
 */
static void CANBUS_Simulator(const Inputs_t *inputs, const Outputs_t *outputs)
{
    int16_t coolant_temp_deci_c = (int16_t)(inputs->coolant_temp_c * 10.0f);

    uint8_t received_frame[CAN_FRAME_DATA_SIZE] =
        {   //Can bus data length is 8 bytes and receiving frames are in the following order:
            (uint8_t)(inputs->ignition_on ? 1 : 0),
            (uint8_t)(coolant_temp_deci_c & 0xFF),
            (uint8_t)((coolant_temp_deci_c >> 8) & 0xFF),
            0,0,0,0,0
        };
        
        //Can bus data length is 8 bytes and Sending frames are in the following order:
    uint8_t send_frame[CAN_FRAME_DATA_SIZE] =
        {
            (uint8_t)outputs->pump_speed_percent,
            (uint8_t)outputs->fan_speed_percent,
            (uint8_t)(outputs->safety_shutdown ? 1 : 0),
            0,0,0,0,0
        };

    printf("CAN Received: [0x%03X", CAN_INPUT_FRAME_ID); // First print Id then all 8 bytes -- all frames converting to hex
    for (unsigned int i = 0U; i < CAN_FRAME_DATA_SIZE; i++)
    {
        printf(" 0x%02X", received_frame[i]);
    }

    printf("]\nCAN Send:     [0x%03X", CAN_OUTPUT_FRAME_ID);
    for (unsigned int i = 0U; i < CAN_FRAME_DATA_SIZE; i++)
    {
        printf(" 0x%02X", send_frame[i]);
    }

    printf("]\n");
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
    CANBUS_Simulator(inputs, outputs);
    printf("-----------------------------------------------------------\n");
}

int main(void)
{
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
        run_cooling_logic(&test_cases[i], &outputs);
        print_status(&test_cases[i], &outputs);
    }

    return 0;
}
