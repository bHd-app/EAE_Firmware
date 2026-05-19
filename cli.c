/**
 * @file cli.c
 * @brief Simple interactive CLI for cooling setpoints.
 */

#include <stdio.h>
#include <string.h>

#include "cli.h"

#define CLI_INPUT_SIZE          32U

/**
 * @brief Prints the current coolant temperature setpoints.
 */
static void print_config(const CoolingConfig_t *config)
{
    printf("\nCurrent coolant setpoints:\n");
    printf("1. Low      : %.1f C\n", config->temp_low_c);
    printf("2. Medium   : %.1f C\n", config->temp_medium_c);
    printf("3. High     : %.1f C\n", config->temp_high_c);
    printf("4. Critical : %.1f C\n", config->temp_critical_c);
}

/**
 * @brief Reads a new floating point value from the terminal.
 */
static void set_value(float *value)
{
    char input[CLI_INPUT_SIZE];
    float new_value;

    printf("Enter new value: ");
    if (fgets(input, sizeof(input), stdin) != NULL)
    {
        if (sscanf(input, "%f", &new_value) == 1)
        {
            *value = new_value;
        }
        else
        {
            printf("Invalid value.\n");
        }
    }
}

/**
 * @brief Allows the user to edit coolant temperature setpoints.
 */
static void config_menu(CoolingConfig_t *config)
{
    char input[CLI_INPUT_SIZE];
    int option;

    while (1)
    {
        print_config(config);
        printf("\nSelect 1-4 to change a value, or type run to start: ");

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            return;
        }

        if (strncmp(input, "run", 3U) == 0)
        {
            return;
        }

        if (sscanf(input, "%d", &option) != 1)
        {
            printf("Invalid option.\n");
            continue;
        }

        if (option == 1)
        {
            set_value(&config->temp_low_c);
        }
        else if (option == 2)
        {
            set_value(&config->temp_medium_c);
        }
        else if (option == 3)
        {
            set_value(&config->temp_high_c);
        }
        else if (option == 4)
        {
            set_value(&config->temp_critical_c);
        }
        else
        {
            printf("Invalid option.\n");
        }
    }
}

/**
 * @brief Gets cooling setpoints from a simple terminal menu.
 */
CoolingConfig_t CLI_GetCoolingConfig(void)
{
    char input[CLI_INPUT_SIZE];
    CoolingConfig_t config =
    {
        35.0f,
        45.0f,
        60.0f,
        75.0f
    };

    while (1)
    {
        printf("Type run to start, or cfg to change coolant setpoints: ");

        if (fgets(input, sizeof(input), stdin) == NULL)
        {
            return config;
        }

        if (strncmp(input, "run", 3U) == 0)
        {
            return config;
        }
        else if (strncmp(input, "cfg", 3U) == 0)
        {
            config_menu(&config);
            return config;
        }
        else
        {
            printf("Unknown command.\n");
        }
    }
}
