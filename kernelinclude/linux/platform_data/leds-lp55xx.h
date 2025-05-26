/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _LEDS_LP55XX_H
#define _LEDS_LP55XX_H

#include <linux/gpio/consumer.h>
#include <linux/led-class-multicolor.h>


#define LP55XX_CLOCK_AUTO	0
#define LP55XX_CLOCK_INT	1
#define LP55XX_CLOCK_EXT	2

#define LP55XX_MAX_GROUPED_CHAN	4

struct lp55xx_led_config {
	const char *name;
	const char *default_trigger;
	u8 chan_nr;
	u8 led_current; 
	u8 max_current;
	int num_colors;
	unsigned int max_channel;
	int color_id[LED_COLOR_ID_MAX];
	int output_num[LED_COLOR_ID_MAX];
};

struct lp55xx_predef_pattern {
	const u8 *r;
	const u8 *g;
	const u8 *b;
	u8 size_r;
	u8 size_g;
	u8 size_b;
};

enum lp8501_pwr_sel {
	LP8501_ALL_VDD,		
	LP8501_6VDD_3VOUT,	
	LP8501_3VDD_6VOUT,	
	LP8501_ALL_VOUT,	
};


struct lp55xx_platform_data {

	
	struct lp55xx_led_config *led_config;
	u8 num_channels;
	const char *label;

	
	u8 clock_mode;

	
	u32 charge_pump_mode;

	
	struct gpio_desc *enable_gpiod;

	
	struct lp55xx_predef_pattern *patterns;
	unsigned int num_patterns;

	
	enum lp8501_pwr_sel pwr_sel;
};

#endif 
