/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __IIO_KXCJK_1013_H__
#define __IIO_KXCJK_1013_H__

#include <linux/iio/iio.h>

struct kxcjk_1013_platform_data {
	bool active_high_intr;
	struct iio_mount_matrix orientation;
};

#endif
