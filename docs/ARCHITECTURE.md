# Architecture

- SPI transport: performs ADC transactions in a datasheet-specific function.
- Linux IIO device: exposes channels and raw reads to userspace.
- Triggered buffer: adds channel data plus timestamp to the IIO buffer path.
- Userspace tools: stream `/dev/iio:deviceN`, calculate throughput and inspect IIO sysfs.

## Current reference boundaries

The driver source is an integration scaffold. It uses a generic placeholder SPI exchange and does not implement any ADC-specific initialization, DRDY-trigger registration, channel scale/calibration or FIFO semantics. Select an actual ADC, replace the SPI operations, and test against its datasheet before deployment.
