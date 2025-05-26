/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef ST_SENSORS_SPI_H
#define ST_SENSORS_SPI_H

#include <linux/spi/spi.h>
#include <linux/iio/common/st_sensors.h>

int st_sensors_spi_configure(struct iio_dev *indio_dev,
			     struct spi_device *spi);

#endif 
