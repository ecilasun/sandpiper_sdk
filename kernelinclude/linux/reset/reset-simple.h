/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __RESET_SIMPLE_H__
#define __RESET_SIMPLE_H__

#include <linux/io.h>
#include <linux/reset-controller.h>
#include <linux/spinlock.h>


struct reset_simple_data {
	spinlock_t			lock;
	void __iomem			*membase;
	struct reset_controller_dev	rcdev;
	bool				active_low;
	bool				status_active_low;
	unsigned int			reset_us;
};

extern const struct reset_control_ops reset_simple_ops;

#endif 
