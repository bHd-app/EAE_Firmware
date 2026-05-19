# EAE Firmware

This project is a simple cooling control firmware simulation written in C.
It reads ignition status and coolant temperature, then calculates pump speed,
fan speed, safety shutdown status, and a simulated CAN bus frame.

## How It Works

The main control flow is in `EAE_Firmware.c`.

- Pump speed is controlled with temperature thresholds.
- Fan speed is controlled with a basic PID controller.
- A small state machine detects `OFF`, `NORMAL`, and `CRITICAL` cooling states.
- If coolant temperature reaches the critical setpoint, the system forces pump
  and fan speed to 100% and activates safety shutdown.
- The CANBUS module converts input and output values into simple printed CAN
  frames.
- The CLI module lets the user run the program or change coolant setpoints
  before running.

## Architecture

```text
EAE_Firmware.c        Main application and cooling logic
build.sh              Builds the project on Linux/macOS/MSYS2
run.sh                Runs the firmware binary
include/
  canbus.h           CAN bus simulator interface
  cli.h              CLI and coolant setpoint configuration interface
  pid.h              PID controller interface
src/
  canbus.c           CAN frame conversion and printing
  cli.c              Interactive terminal menu
  pid.c              Basic PID controller logic
```

## Build and Run

```sh
./build.sh
./run.sh
```

At startup, type:

```text
run
```

to run with default setpoints, or:

```text
cfg
```

to change coolant temperature setpoints before running.

## Test Inputs

For this simulation, the following fake input values are used in `main()`:

```c
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
```

They are only used to check the system in different operating conditions.
In a real firmware system, these values would come from other software modules,
sensors, or communication interfaces instead of being hardcoded in `main()`.
