#!/bin/sh

set -e

PROJECT_DIR=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)
BINARY="$PROJECT_DIR/binary/EAE_Firmware"

if [ ! -x "$BINARY" ]; then
    "$PROJECT_DIR/build.sh"
fi

"$BINARY"
