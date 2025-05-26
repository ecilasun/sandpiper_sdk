/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _GSC_HWMON_H
#define _GSC_HWMON_H

enum gsc_hwmon_mode {
	mode_temperature,
	mode_voltage_24bit,
	mode_voltage_raw,
	mode_voltage_16bit,
	mode_fan,
	mode_max,
};


struct gsc_hwmon_channel {
	unsigned int reg;
	unsigned int mode;
	const char *name;
	unsigned int mvoffset;
	unsigned int vdiv[2];
};


struct gsc_hwmon_platform_data {
	int nchannels;
	unsigned int resolution;
	unsigned int vreference;
	unsigned int fan_base;
	struct gsc_hwmon_channel channels[];
};
#endif
