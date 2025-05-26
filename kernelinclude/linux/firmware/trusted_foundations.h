/* SPDX-License-Identifier: GPL-2.0-or-later */




#ifndef __FIRMWARE_TRUSTED_FOUNDATIONS_H
#define __FIRMWARE_TRUSTED_FOUNDATIONS_H

#include <linux/printk.h>
#include <linux/bug.h>
#include <linux/of.h>
#include <linux/cpu.h>
#include <linux/smp.h>
#include <linux/types.h>

#include <asm/hardware/cache-l2x0.h>
#include <asm/outercache.h>

#define TF_PM_MODE_LP0			0
#define TF_PM_MODE_LP1			1
#define TF_PM_MODE_LP1_NO_MC_CLK	2
#define TF_PM_MODE_LP2			3
#define TF_PM_MODE_LP2_NOFLUSH_L2	4
#define TF_PM_MODE_NONE			5

struct trusted_foundations_platform_data {
	unsigned int version_major;
	unsigned int version_minor;
};

#if IS_ENABLED(CONFIG_TRUSTED_FOUNDATIONS)

void register_trusted_foundations(struct trusted_foundations_platform_data *pd);
void of_register_trusted_foundations(void);
bool trusted_foundations_registered(void);

#else 
static inline void tf_dummy_write_sec(unsigned long val, unsigned int reg)
{
}

static inline void register_trusted_foundations(
				   struct trusted_foundations_platform_data *pd)
{
	
	pr_err("No support for Trusted Foundations, continuing in degraded mode.\n");
	pr_err("Secondary processors as well as CPU PM will be disabled.\n");
#if IS_ENABLED(CONFIG_CACHE_L2X0)
	pr_err("L2X0 cache will be kept disabled.\n");
	outer_cache.write_sec = tf_dummy_write_sec;
#endif
#if IS_ENABLED(CONFIG_SMP)
	setup_max_cpus = 0;
#endif
	cpu_idle_poll_ctrl(true);
}

static inline void of_register_trusted_foundations(void)
{
	struct device_node *np = of_find_compatible_node(NULL, NULL, "tlm,trusted-foundations");

	if (!np)
		return;
	of_node_put(np);
	
	register_trusted_foundations(NULL);
}

static inline bool trusted_foundations_registered(void)
{
	return false;
}
#endif 

#endif
