/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __LINUX_PLATFORM_DATA_EMC2305__
#define __LINUX_PLATFORM_DATA_EMC2305__

#define EMC2305_PWM_MAX	5


struct emc2305_platform_data {
	u8 max_state;
	u8 pwm_num;
	bool pwm_separate;
	u8 pwm_min[EMC2305_PWM_MAX];
};

#endif
