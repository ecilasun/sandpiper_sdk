/* SPDX-License-Identifier: GPL-2.0 */


#ifndef STM32_DFSDM_ADC_H
#define STM32_DFSDM_ADC_H

#include <linux/iio/iio.h>

int stm32_dfsdm_get_buff_cb(struct iio_dev *iio_dev,
			    int (*cb)(const void *data, size_t size,
				      void *private),
			    void *private);
int stm32_dfsdm_release_buff_cb(struct iio_dev *iio_dev);

#endif
