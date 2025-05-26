/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __LINUX_GPIO_H
#define __LINUX_GPIO_H

#include <linux/types.h>

struct device;




#define GPIOF_DIR_OUT	(0 << 0)
#define GPIOF_DIR_IN	(1 << 0)

#define GPIOF_INIT_LOW	(0 << 1)
#define GPIOF_INIT_HIGH	(1 << 1)

#define GPIOF_IN		(GPIOF_DIR_IN)
#define GPIOF_OUT_INIT_LOW	(GPIOF_DIR_OUT | GPIOF_INIT_LOW)
#define GPIOF_OUT_INIT_HIGH	(GPIOF_DIR_OUT | GPIOF_INIT_HIGH)


#define GPIOF_ACTIVE_LOW        (1 << 2)


struct gpio {
	unsigned	gpio;
	unsigned long	flags;
	const char	*label;
};

#ifdef CONFIG_GPIOLIB

#include <linux/gpio/consumer.h>


static inline bool gpio_is_valid(int number)
{
	
	return number >= 0;
}




#define GPIO_DYNAMIC_BASE	512


int gpio_request(unsigned gpio, const char *label);
void gpio_free(unsigned gpio);

static inline int gpio_direction_input(unsigned gpio)
{
	return gpiod_direction_input(gpio_to_desc(gpio));
}
static inline int gpio_direction_output(unsigned gpio, int value)
{
	return gpiod_direction_output_raw(gpio_to_desc(gpio), value);
}

static inline int gpio_get_value_cansleep(unsigned gpio)
{
	return gpiod_get_raw_value_cansleep(gpio_to_desc(gpio));
}
static inline void gpio_set_value_cansleep(unsigned gpio, int value)
{
	return gpiod_set_raw_value_cansleep(gpio_to_desc(gpio), value);
}

static inline int gpio_get_value(unsigned gpio)
{
	return gpiod_get_raw_value(gpio_to_desc(gpio));
}
static inline void gpio_set_value(unsigned gpio, int value)
{
	return gpiod_set_raw_value(gpio_to_desc(gpio), value);
}

static inline int gpio_to_irq(unsigned gpio)
{
	return gpiod_to_irq(gpio_to_desc(gpio));
}

int gpio_request_one(unsigned gpio, unsigned long flags, const char *label);
int gpio_request_array(const struct gpio *array, size_t num);
void gpio_free_array(const struct gpio *array, size_t num);



int devm_gpio_request(struct device *dev, unsigned gpio, const char *label);
int devm_gpio_request_one(struct device *dev, unsigned gpio,
			  unsigned long flags, const char *label);

#else 

#include <linux/kernel.h>

#include <asm/bug.h>
#include <asm/errno.h>

static inline bool gpio_is_valid(int number)
{
	return false;
}

static inline int gpio_request(unsigned gpio, const char *label)
{
	return -ENOSYS;
}

static inline int gpio_request_one(unsigned gpio,
					unsigned long flags, const char *label)
{
	return -ENOSYS;
}

static inline int gpio_request_array(const struct gpio *array, size_t num)
{
	return -ENOSYS;
}

static inline void gpio_free(unsigned gpio)
{
	might_sleep();

	
	WARN_ON(1);
}

static inline void gpio_free_array(const struct gpio *array, size_t num)
{
	might_sleep();

	
	WARN_ON(1);
}

static inline int gpio_direction_input(unsigned gpio)
{
	return -ENOSYS;
}

static inline int gpio_direction_output(unsigned gpio, int value)
{
	return -ENOSYS;
}

static inline int gpio_get_value(unsigned gpio)
{
	
	WARN_ON(1);
	return 0;
}

static inline void gpio_set_value(unsigned gpio, int value)
{
	
	WARN_ON(1);
}

static inline int gpio_get_value_cansleep(unsigned gpio)
{
	
	WARN_ON(1);
	return 0;
}

static inline void gpio_set_value_cansleep(unsigned gpio, int value)
{
	
	WARN_ON(1);
}

static inline int gpio_to_irq(unsigned gpio)
{
	
	WARN_ON(1);
	return -EINVAL;
}

static inline int devm_gpio_request(struct device *dev, unsigned gpio,
				    const char *label)
{
	WARN_ON(1);
	return -EINVAL;
}

static inline int devm_gpio_request_one(struct device *dev, unsigned gpio,
					unsigned long flags, const char *label)
{
	WARN_ON(1);
	return -EINVAL;
}

#endif 

#endif 
