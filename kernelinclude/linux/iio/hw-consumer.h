/* SPDX-License-Identifier: GPL-2.0 */


#ifndef LINUX_IIO_HW_CONSUMER_H
#define LINUX_IIO_HW_CONSUMER_H

struct iio_hw_consumer;

struct iio_hw_consumer *iio_hw_consumer_alloc(struct device *dev);
void iio_hw_consumer_free(struct iio_hw_consumer *hwc);
struct iio_hw_consumer *devm_iio_hw_consumer_alloc(struct device *dev);
int iio_hw_consumer_enable(struct iio_hw_consumer *hwc);
void iio_hw_consumer_disable(struct iio_hw_consumer *hwc);

#endif
