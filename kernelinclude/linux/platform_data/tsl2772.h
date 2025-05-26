/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef __TSL2772_H
#define __TSL2772_H

struct tsl2772_lux {
	unsigned int ch0;
	unsigned int ch1;
};


#define TSL2772_MAX_LUX_TABLE_SIZE		6

#define TSL2772_DEF_LUX_TABLE_SZ		3
#define TSL2772_DEFAULT_TABLE_BYTES (sizeof(struct tsl2772_lux) * \
				     TSL2772_DEF_LUX_TABLE_SZ)


#define TSL2772_DIODE0                  0x01
#define TSL2772_DIODE1                  0x02
#define TSL2772_DIODE_BOTH              0x03


#define TSL2772_100_mA                  0x00
#define TSL2772_50_mA                   0x01
#define TSL2772_25_mA                   0x02
#define TSL2772_13_mA                   0x03


struct tsl2772_settings {
	int als_time;
	int als_gain;
	int als_gain_trim;
	int wait_time;
	int prox_time;
	int prox_gain;
	int als_prox_config;
	int als_cal_target;
	u8 als_persistence;
	bool als_interrupt_en;
	int als_thresh_low;
	int als_thresh_high;
	u8 prox_persistence;
	bool prox_interrupt_en;
	int prox_thres_low;
	int prox_thres_high;
	int prox_pulse_count;
	int prox_max_samples_cal;
	int prox_diode;
	int prox_power;
};


struct tsl2772_platform_data {
	struct tsl2772_lux platform_lux_table[TSL2772_MAX_LUX_TABLE_SIZE];
	struct tsl2772_settings *platform_default_settings;
};

#endif 
