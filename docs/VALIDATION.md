# Validation plan

## Bring-up
- confirm SPI mode, chip select, clock frequency and device ID/configuration from ADC datasheet;
- check probe/registration and IIO sysfs channels;
- test direct conversion reads against a known voltage;
- validate scaling, signedness, channel order and endianness.

## Sustained acquisition
- enable buffer and selected scan elements through IIO sysfs;
- set a supported sampling frequency;
- capture for fixed durations at several rates;
- measure bytes/s, records/s, sample sequence gaps and kernel errors;
- record CPU utilization and memory pressure.

## Error cases
- unplug/power down ADC;
- force SPI transfer errors where possible;
- stop/restart trigger source;
- fill buffers faster than userspace consumes them;
- ensure errors surface in kernel logs and the reader can recover.

No throughput or dropped-sample value is claimed until measured on the chosen ADC and board.
