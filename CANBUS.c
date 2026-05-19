/**
 * @file CANBUS.c
 * @brief Basic CAN bus frame simulator.
 */

#include <stdio.h>
#include <stdint.h>

#include "CANBUS.h"

#define CAN_INPUT_FRAME_ID      0x100U
#define CAN_OUTPUT_FRAME_ID     0x101U
#define CAN_FRAME_DATA_SIZE     8U

/**
 * @brief Converts cooling inputs and outputs to simulated CAN frames.
 */
void CANBUS_Simulator(const Inputs_t *inputs, const Outputs_t *outputs)
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
