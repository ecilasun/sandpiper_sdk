/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __LINUX_PINCTRL_PINCONF_GENERIC_H
#define __LINUX_PINCTRL_PINCONF_GENERIC_H

#include <linux/types.h>

#include <linux/pinctrl/machine.h>

struct device_node;

struct pinctrl_dev;
struct pinctrl_map;


enum pin_config_param {
	PIN_CONFIG_BIAS_BUS_HOLD,
	PIN_CONFIG_BIAS_DISABLE,
	PIN_CONFIG_BIAS_HIGH_IMPEDANCE,
	PIN_CONFIG_BIAS_PULL_DOWN,
	PIN_CONFIG_BIAS_PULL_PIN_DEFAULT,
	PIN_CONFIG_BIAS_PULL_UP,
	PIN_CONFIG_DRIVE_OPEN_DRAIN,
	PIN_CONFIG_DRIVE_OPEN_SOURCE,
	PIN_CONFIG_DRIVE_PUSH_PULL,
	PIN_CONFIG_DRIVE_STRENGTH,
	PIN_CONFIG_DRIVE_STRENGTH_UA,
	PIN_CONFIG_INPUT_DEBOUNCE,
	PIN_CONFIG_INPUT_ENABLE,
	PIN_CONFIG_INPUT_SCHMITT,
	PIN_CONFIG_INPUT_SCHMITT_ENABLE,
	PIN_CONFIG_MODE_LOW_POWER,
	PIN_CONFIG_MODE_PWM,
	PIN_CONFIG_OUTPUT,
	PIN_CONFIG_OUTPUT_ENABLE,
	PIN_CONFIG_OUTPUT_IMPEDANCE_OHMS,
	PIN_CONFIG_PERSIST_STATE,
	PIN_CONFIG_POWER_SOURCE,
	PIN_CONFIG_SKEW_DELAY,
	PIN_CONFIG_SLEEP_HARDWARE_STATE,
	PIN_CONFIG_SLEW_RATE,
	PIN_CONFIG_END = 0x7F,
	PIN_CONFIG_MAX = 0xFF,
};


#define PIN_CONF_PACKED(p, a) ((a << 8) | ((unsigned long) p & 0xffUL))



static inline enum pin_config_param pinconf_to_config_param(unsigned long config)
{
	return (enum pin_config_param) (config & 0xffUL);
}

static inline u32 pinconf_to_config_argument(unsigned long config)
{
	return (u32) ((config >> 8) & 0xffffffUL);
}

static inline unsigned long pinconf_to_config_packed(enum pin_config_param param,
						     u32 argument)
{
	return PIN_CONF_PACKED(param, argument);
}

#define PCONFDUMP(a, b, c, d) {					\
	.param = a, .display = b, .format = c, .has_arg = d	\
	}

struct pin_config_item {
	const enum pin_config_param param;
	const char * const display;
	const char * const format;
	bool has_arg;
};

struct pinconf_generic_params {
	const char * const property;
	enum pin_config_param param;
	u32 default_value;
};

int pinconf_generic_dt_subnode_to_map(struct pinctrl_dev *pctldev,
		struct device_node *np, struct pinctrl_map **map,
		unsigned *reserved_maps, unsigned *num_maps,
		enum pinctrl_map_type type);
int pinconf_generic_dt_node_to_map(struct pinctrl_dev *pctldev,
		struct device_node *np_config, struct pinctrl_map **map,
		unsigned *num_maps, enum pinctrl_map_type type);
void pinconf_generic_dt_free_map(struct pinctrl_dev *pctldev,
		struct pinctrl_map *map, unsigned num_maps);

static inline int pinconf_generic_dt_node_to_map_group(struct pinctrl_dev *pctldev,
		struct device_node *np_config, struct pinctrl_map **map,
		unsigned *num_maps)
{
	return pinconf_generic_dt_node_to_map(pctldev, np_config, map, num_maps,
			PIN_MAP_TYPE_CONFIGS_GROUP);
}

static inline int pinconf_generic_dt_node_to_map_pin(struct pinctrl_dev *pctldev,
		struct device_node *np_config, struct pinctrl_map **map,
		unsigned *num_maps)
{
	return pinconf_generic_dt_node_to_map(pctldev, np_config, map, num_maps,
			PIN_MAP_TYPE_CONFIGS_PIN);
}

static inline int pinconf_generic_dt_node_to_map_all(struct pinctrl_dev *pctldev,
		struct device_node *np_config, struct pinctrl_map **map,
		unsigned *num_maps)
{
	
	return pinconf_generic_dt_node_to_map(pctldev, np_config, map, num_maps,
			PIN_MAP_TYPE_INVALID);
}

#endif 
