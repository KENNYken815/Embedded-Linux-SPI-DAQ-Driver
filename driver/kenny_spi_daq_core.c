#include <linux/module.h>
#include <linux/spi/spi.h>
#include <linux/iio/iio.h>
#include <linux/iio/buffer.h>
#include <linux/iio/triggered_buffer.h>
#include <linux/iio/trigger_consumer.h>
#include <linux/mutex.h>
#include <linux/property.h>
#include <linux/bitops.h>
#include <linux/interrupt.h>

#define KENNY_DAQ_CHANNELS 2

struct kenny_daq_state {
 struct spi_device *spi;
 struct mutex lock;
 u32 conversion_errors;
};

/*
 * REPLACE with the selected ADC datasheet's command framing, conversion delay,
 * status-bit handling and calibration. This placeholder is intentionally not
 * advertised as a driver for ADS8688 or any other specific ADC.
 */
static int adc_read_sample(struct kenny_daq_state *st, unsigned int channel, u16 *value)
{
 u8 tx[2] = { (u8)(0x80 | (channel & 0x0f)), 0x00 };
 u8 rx[2] = { 0 };
 struct spi_transfer xfer = {
  .tx_buf = tx, .rx_buf = rx, .len = sizeof(tx),
  .speed_hz = st->spi->max_speed_hz,
 };
 int ret = spi_sync_transfer(st->spi, &xfer, 1);
 if (ret) {
  st->conversion_errors++;
  return ret;
 }
 *value = ((u16)rx[0] << 8) | rx[1];
 return 0;
}

static int kenny_daq_read_raw(struct iio_dev *indio_dev,
 const struct iio_chan_spec *chan, int *val, int *val2, long mask)
{
 struct kenny_daq_state *st = iio_priv(indio_dev);
 u16 sample;
 int ret;
 if (mask != IIO_CHAN_INFO_RAW)
  return -EINVAL;
 mutex_lock(&st->lock);
 ret = adc_read_sample(st, chan->channel, &sample);
 mutex_unlock(&st->lock);
 if (ret)
  return ret;
 *val = sample;
 return IIO_VAL_INT;
}

static const struct iio_info kenny_daq_iio_info = { .read_raw = kenny_daq_read_raw };

#define KENNY_DAQ_CHAN(_idx) { \
 .type = IIO_VOLTAGE, .indexed = 1, .channel = (_idx), \
 .info_mask_separate = BIT(IIO_CHAN_INFO_RAW), \
 .scan_index = (_idx), \
 .scan_type = { .sign = 'u', .realbits = 16, .storagebits = 16, .endianness = IIO_LE } \
}
static const struct iio_chan_spec kenny_daq_channels[] = {
 KENNY_DAQ_CHAN(0), KENNY_DAQ_CHAN(1),
 IIO_CHAN_SOFT_TIMESTAMP(2),
};

static irqreturn_t kenny_daq_trigger_handler(int irq, void *p)
{
 struct iio_poll_func *pf = p;
 struct iio_dev *indio_dev = pf->indio_dev;
 struct kenny_daq_state *st = iio_priv(indio_dev);
 struct { u16 ch0; u16 ch1; s64 ts __aligned(8); } scan = { 0 };
 int ret0, ret1;
 (void)irq;
 mutex_lock(&st->lock);
 ret0 = adc_read_sample(st, 0, &scan.ch0);
 ret1 = ret0 ? ret0 : adc_read_sample(st, 1, &scan.ch1);
 mutex_unlock(&st->lock);
 if (!ret1) {
  scan.ts = iio_get_time_ns(indio_dev);
  iio_push_to_buffers_with_timestamp(indio_dev, &scan, scan.ts);
 }
 iio_trigger_notify_done(indio_dev->trig);
 return IRQ_HANDLED;
}

static int kenny_daq_probe(struct spi_device *spi)
{
 struct iio_dev *indio_dev;
 struct kenny_daq_state *st;
 int ret;
 indio_dev = devm_iio_device_alloc(&spi->dev, sizeof(*st));
 if (!indio_dev)
  return -ENOMEM;
 st = iio_priv(indio_dev);
 st->spi = spi;
 mutex_init(&st->lock);
 spi_set_drvdata(spi, indio_dev);
 indio_dev->name = "kenny-spi-daq";
 indio_dev->info = &kenny_daq_iio_info;
 indio_dev->modes = INDIO_DIRECT_MODE;
 indio_dev->channels = kenny_daq_channels;
 indio_dev->num_channels = ARRAY_SIZE(kenny_daq_channels);
 ret = devm_iio_triggered_buffer_setup(&spi->dev, indio_dev, NULL,
  kenny_daq_trigger_handler, NULL);
 if (ret)
  return dev_err_probe(&spi->dev, ret, "buffer setup failed\\n");
 ret = devm_iio_device_register(&spi->dev, indio_dev);
 if (ret)
  return dev_err_probe(&spi->dev, ret, "IIO registration failed\\n");
 dev_info(&spi->dev, "reference SPI DAQ IIO device registered\\n");
 return 0;
}

static const struct of_device_id kenny_daq_of_match[] = {
 { .compatible = "kenny,spi-daq-reference" }, { }
};
MODULE_DEVICE_TABLE(of, kenny_daq_of_match);
static const struct spi_device_id kenny_daq_ids[] = { { "kenny-spi-daq" }, { } };
MODULE_DEVICE_TABLE(spi, kenny_daq_ids);
static struct spi_driver kenny_daq_driver = {
 .driver = { .name = "kenny-spi-daq", .of_match_table = kenny_daq_of_match },
 .probe = kenny_daq_probe,
 .id_table = kenny_daq_ids,
};
module_spi_driver(kenny_daq_driver);
MODULE_AUTHOR("KENNYken815");
MODULE_DESCRIPTION("Reference SPI ADC driver integrated with Linux IIO");
MODULE_LICENSE("GPL");
