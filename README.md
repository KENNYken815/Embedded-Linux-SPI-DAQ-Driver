# Embedded Linux SPI DAQ Driver (IIO)

A portfolio-oriented Embedded Linux driver project demonstrating an SPI ADC integration path into the Linux Industrial I/O (IIO) subsystem, including direct raw reads, triggered-buffer plumbing, device-tree binding, userspace capture, and sustained-read throughput measurement tools.

> **Important scope note:** The kernel driver is a reference scaffold, not a ready-to-bind driver for ADS8688 or another specific ADC. The SPI transaction in `adc_read_sample()` is intentionally generic and must be replaced using the chosen ADC's datasheet. The triggered-buffer handler is present, but this initial scaffold does not register a hardware DRDY trigger or configure ADC conversion mode. Do not expect valid measurements or sustained hardware acquisition until those target-specific parts are implemented and tested.

## Architecture

    SPI ADC + conversion-ready signal
              |
        SPI controller / GPIO IRQ
              |
        Linux kernel driver
              |
        IIO channels + triggered buffer
              |
        /dev/iio:deviceN + sysfs
              |
        C capture utility / Python benchmark

## Current Deliverables

- Linux SPI driver skeleton with `spi_driver` registration.
- IIO voltage-channel model and `read_raw` callback.
- IIO triggered-buffer setup and timestamped scan push.
- Example Raspberry Pi device-tree overlay.
- C userspace tool for reading an IIO device and reporting throughput.
- Python userspace capture benchmark.
- Simple IIO sysfs inspection test.
- Kernel-module build target, userspace build target, and hardware validation docs.

## Repository Layout

    Embedded-Linux-SPI-DAQ-Driver/
    +-- driver/
    |   +-- kenny_spi_daq_core.c
    |   +-- Makefile
    |   +-- README.md
    +-- dts/raspberry-pi-spi-daq-overlay.dts
    +-- userspace/
    |   +-- iio_capture.c
    |   +-- benchmark_iio.py
    +-- tests/test_sysfs.sh
    +-- docs/
    |   +-- ARCHITECTURE.md
    |   +-- BUILD_AND_RUN.md
    |   +-- VALIDATION.md
    +-- .gitignore
    +-- LICENSE
    +-- Makefile
    +-- README.md

## Requirements

- Linux SBC with SPI enabled (for example, Raspberry Pi).
- Kernel headers matching the running kernel.
- Kernel configured with SPI and IIO support.
- Selected SPI ADC with known command framing and conversion timing.
- Optional conversion-ready GPIO/IRQ and an IIO trigger design.
- `iio-utils` for `iio_info` and `iio_readdev` when available.

## Build

Build userspace capture tool:

    make userspace

Build module against the running kernel:

    make module

The driver may need small API adjustments for the target kernel version. Building alone is not proof of correct ADC communications.

## Inspect IIO

After porting the SPI transaction and matching the device-tree binding to your board:

    ls /sys/bus/iio/devices/
    cat /sys/bus/iio/devices/iio:deviceX/name
    iio_info -n
    iio_readdev -b 128 iio:deviceX

The exact channel names and buffer scan elements depend on the final ADC channel configuration and target kernel.

## Throughput Capture

Build and run the C reader:

    make userspace
    sudo ./build/iio_capture /dev/iio:device0 10

Or use the Python benchmark:

    python3 userspace/benchmark_iio.py --device /dev/iio:device0 --seconds 10

These tools measure bytes delivered to userspace from the selected IIO device. To calculate samples/s accurately, configure and document the enabled scan elements and bytes per scan. Dropped-sample accounting requires a sequence counter from hardware/driver or a known waveform/test fixture; throughput alone cannot reveal every loss.

## Driver Integration Work Remaining

Before physical use, select one ADC and implement its actual driver-specific details:

1. SPI command/register map, chip-select framing, conversion wait time, and sample endianness.
2. Probe-time ADC reset/configuration and optional device-ID validation.
3. Real ADC channel count, signedness, scale, offset, and calibration.
4. Conversion-ready GPIO/IRQ handling or an explicit timer-trigger approach.
5. Correct IIO scan packing and enabled-channel masks.
6. Error counters and recoverable behavior for SPI errors and trigger overruns.
7. Device-tree interrupt/pinctrl properties for the selected SBC.
8. Kernel-version compilation and target-board validation.

## Validation Plan

Measure on the real target:
- direct-read agreement with a known input voltage;
- achieved samples per second at each rate;
- bytes/s and records/s;
- buffer overruns and sample sequence gaps;
- CPU utilization and memory pressure;
- latency from conversion-ready edge to scan availability;
- behavior during ADC disconnect and SPI errors.

`docs/VALIDATION.md` provides a repeatable bring-up and sustained-acquisition plan. No actual throughput, loss-rate, or CPU-load numbers are claimed in this repository yet.

## License

GPL-2.0-only for Linux kernel driver code.
