/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __PLATFORM_DATA_X86_PWM_LPSS_H
#define __PLATFORM_DATA_X86_PWM_LPSS_H

#include <linux/types.h>

struct device;

struct pwm_lpss_chip;

struct pwm_lpss_boardinfo {
	unsigned long clk_rate;
	unsigned int npwm;
	unsigned long base_unit_bits;
	
	bool bypass;
	
	bool other_devices_aml_touches_pwm_regs;
};

struct pwm_lpss_chip *devm_pwm_lpss_probe(struct device *dev, void __iomem *base,
					  const struct pwm_lpss_boardinfo *info);

#endif	
