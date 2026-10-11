# Build and run

Install matching kernel headers/build tools on the target Linux SBC. Build the module from this repository root:

    make module

Load/unload:

    sudo insmod driver/kenny_spi_daq.ko
    dmesg | tail
    sudo rmmod kenny_spi_daq

Inspect IIO:

    ls /sys/bus/iio/devices/
    cat /sys/bus/iio/devices/iio:deviceX/name
    iio_info -n

Userspace throughput tool:

    make userspace
    sudo ./build/iio_capture /dev/iio:deviceX 10
    python3 userspace/benchmark_iio.py --device /dev/iio:deviceX --seconds 10

These commands are a workflow template only. The generic reference driver's SPI transaction and trigger wiring must match your ADC/board first. Do not load it on hardware expecting useful samples before completing that adaptation.
