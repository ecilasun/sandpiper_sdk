/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_MULTICOLOR_LEDS_H_INCLUDED
#define _LINUX_MULTICOLOR_LEDS_H_INCLUDED

#include <linux/leds.h>
#include <dt-bindings/leds/common.h>

struct mc_subled {
	unsigned int color_index;
	unsigned int brightness;
	unsigned int intensity;
	unsigned int channel;
};

struct led_classdev_mc {
	
	struct led_classdev led_cdev;
	unsigned int num_colors;

	struct mc_subled *subled_info;
};

static inline struct led_classdev_mc *lcdev_to_mccdev(
						struct led_classdev *led_cdev)
{
	return container_of(led_cdev, struct led_classdev_mc, led_cdev);
}

#if IS_ENABLED(CONFIG_LEDS_CLASS_MULTICOLOR)

int led_classdev_multicolor_register_ext(struct device *parent,
					    struct led_classdev_mc *mcled_cdev,
					    struct led_init_data *init_data);


void led_classdev_multicolor_unregister(struct led_classdev_mc *mcled_cdev);


int led_mc_calc_color_components(struct led_classdev_mc *mcled_cdev,
				 enum led_brightness brightness);

int devm_led_classdev_multicolor_register_ext(struct device *parent,
					  struct led_classdev_mc *mcled_cdev,
					  struct led_init_data *init_data);

void devm_led_classdev_multicolor_unregister(struct device *parent,
					    struct led_classdev_mc *mcled_cdev);
#else

static inline int led_classdev_multicolor_register_ext(struct device *parent,
					    struct led_classdev_mc *mcled_cdev,
					    struct led_init_data *init_data)
{
	return 0;
}

static inline void led_classdev_multicolor_unregister(struct led_classdev_mc *mcled_cdev) {};
static inline int led_mc_calc_color_components(struct led_classdev_mc *mcled_cdev,
					       enum led_brightness brightness)
{
	return 0;
}

static inline int devm_led_classdev_multicolor_register_ext(struct device *parent,
					  struct led_classdev_mc *mcled_cdev,
					  struct led_init_data *init_data)
{
	return 0;
}

static inline void devm_led_classdev_multicolor_unregister(struct device *parent,
					    struct led_classdev_mc *mcled_cdev)
{};

#endif  

static inline int led_classdev_multicolor_register(struct device *parent,
					    struct led_classdev_mc *mcled_cdev)
{
	return led_classdev_multicolor_register_ext(parent, mcled_cdev, NULL);
}

static inline int devm_led_classdev_multicolor_register(struct device *parent,
					     struct led_classdev_mc *mcled_cdev)
{
	return devm_led_classdev_multicolor_register_ext(parent, mcled_cdev,
							 NULL);
}

#endif	
