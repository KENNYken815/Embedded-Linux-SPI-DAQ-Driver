#!/bin/sh
set -eu
IIO=${1:-/sys/bus/iio/devices/iio:device0}
test -d "$IIO" || { echo "IIO device path not found: $IIO" >&2; exit 1; }
echo "name: $(cat "$IIO/name" 2>/dev/null || echo unknown)"
find "$IIO" -maxdepth 1 -type f \( -name 'in_voltage*_raw' -o -name 'sampling_frequency*' -o -name 'buffer*' \) -print
