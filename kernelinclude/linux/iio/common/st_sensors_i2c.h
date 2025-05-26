/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef ST_SENSORS_I2C_H
#define ST_SENSORS_I2C_H

#include <linux/i2c.h>
#include <linux/iio/common/st_sensors.h>

int st_sensors_i2c_configure(struct iio_dev *indio_dev,
			     struct i2c_client *client);

#endif 
