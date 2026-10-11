# Driver implementation notes

This directory contains an IIO triggered-buffer driver structure for a generic SPI ADC. The SPI protocol in `adc_read_sample()` is a placeholder and MUST be replaced with transactions from the selected ADC datasheet. It is not safe to bind this driver to an ADS8688 and expect meaningful samples without that adaptation.

The initial interface exposes two 16-bit voltage channels and an IIO software timestamp. The current trigger handler assumes a trigger source is configured by the platform; external DRDY IRQ wiring and a dedicated IIO trigger are board-specific integration work.
