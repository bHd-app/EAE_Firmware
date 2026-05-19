#!/bin/sh

set -e

PROJECT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

mkdir -p "$PROJECT_DIR/binary"

cc -Wall -Wextra -Wpedantic \
    -I"$PROJECT_DIR/include" \
    "$PROJECT_DIR/EAE_Firmware.c" \
    "$PROJECT_DIR/src/pid.c" \
    "$PROJECT_DIR/src/canbus.c" \
    "$PROJECT_DIR/src/cli.c" \
    -o "$PROJECT_DIR/binary/EAE_Firmware"
