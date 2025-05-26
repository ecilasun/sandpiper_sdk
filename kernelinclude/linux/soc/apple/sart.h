/* SPDX-License-Identifier: GPL-2.0-only OR MIT */


#ifndef _LINUX_SOC_APPLE_SART_H_
#define _LINUX_SOC_APPLE_SART_H_

#include <linux/device.h>
#include <linux/err.h>
#include <linux/types.h>

struct apple_sart;


struct apple_sart *devm_apple_sart_get(struct device *dev);


int apple_sart_add_allowed_region(struct apple_sart *sart, phys_addr_t paddr,
				  size_t size);


int apple_sart_remove_allowed_region(struct apple_sart *sart, phys_addr_t paddr,
				     size_t size);

#endif 
