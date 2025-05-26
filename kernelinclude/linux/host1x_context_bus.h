/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef __LINUX_HOST1X_CONTEXT_BUS_H
#define __LINUX_HOST1X_CONTEXT_BUS_H

#include <linux/device.h>

#ifdef CONFIG_TEGRA_HOST1X_CONTEXT_BUS
extern struct bus_type host1x_context_device_bus_type;
#endif

#endif
