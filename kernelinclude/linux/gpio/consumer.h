/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_GPIO_CONSUMER_H
#define __LINUX_GPIO_CONSUMER_H

#include <linux/bits.h>
#include <linux/types.h>

struct acpi_device;
struct device;
struct fwnode_handle;

struct gpio_array;
struct gpio_desc;


struct gpio_descs {
	struct gpio_array *info;
	unsigned int ndescs;
	struct gpio_desc *desc[];
};

#define GPIOD_FLAGS_BIT_DIR_SET		BIT(0)
#define GPIOD_FLAGS_BIT_DIR_OUT		BIT(1)
#define GPIOD_FLAGS_BIT_DIR_VAL		BIT(2)
#define GPIOD_FLAGS_BIT_OPEN_DRAIN	BIT(3)
#define GPIOD_FLAGS_BIT_NONEXCLUSIVE	BIT(4)


enum gpiod_flags {
	GPIOD_ASIS	= 0,
	GPIOD_IN	= GPIOD_FLAGS_BIT_DIR_SET,
	GPIOD_OUT_LOW	= GPIOD_FLAGS_BIT_DIR_SET | GPIOD_FLAGS_BIT_DIR_OUT,
	GPIOD_OUT_HIGH	= GPIOD_FLAGS_BIT_DIR_SET | GPIOD_FLAGS_BIT_DIR_OUT |
			  GPIOD_FLAGS_BIT_DIR_VAL,
	GPIOD_OUT_LOW_OPEN_DRAIN = GPIOD_OUT_LOW | GPIOD_FLAGS_BIT_OPEN_DRAIN,
	GPIOD_OUT_HIGH_OPEN_DRAIN = GPIOD_OUT_HIGH | GPIOD_FLAGS_BIT_OPEN_DRAIN,
};

#ifdef CONFIG_GPIOLIB


int gpiod_count(struct device *dev, const char *con_id);


struct gpio_desc *__must_check gpiod_get(struct device *dev,
					 const char *con_id,
					 enum gpiod_flags flags);
struct gpio_desc *__must_check gpiod_get_index(struct device *dev,
					       const char *con_id,
					       unsigned int idx,
					       enum gpiod_flags flags);
struct gpio_desc *__must_check gpiod_get_optional(struct device *dev,
						  const char *con_id,
						  enum gpiod_flags flags);
struct gpio_desc *__must_check gpiod_get_index_optional(struct device *dev,
							const char *con_id,
							unsigned int index,
							enum gpiod_flags flags);
struct gpio_descs *__must_check gpiod_get_array(struct device *dev,
						const char *con_id,
						enum gpiod_flags flags);
struct gpio_descs *__must_check gpiod_get_array_optional(struct device *dev,
							const char *con_id,
							enum gpiod_flags flags);
void gpiod_put(struct gpio_desc *desc);
void gpiod_put_array(struct gpio_descs *descs);

struct gpio_desc *__must_check devm_gpiod_get(struct device *dev,
					      const char *con_id,
					      enum gpiod_flags flags);
struct gpio_desc *__must_check devm_gpiod_get_index(struct device *dev,
						    const char *con_id,
						    unsigned int idx,
						    enum gpiod_flags flags);
struct gpio_desc *__must_check devm_gpiod_get_optional(struct device *dev,
						       const char *con_id,
						       enum gpiod_flags flags);
struct gpio_desc *__must_check
devm_gpiod_get_index_optional(struct device *dev, const char *con_id,
			      unsigned int index, enum gpiod_flags flags);
struct gpio_descs *__must_check devm_gpiod_get_array(struct device *dev,
						     const char *con_id,
						     enum gpiod_flags flags);
struct gpio_descs *__must_check
devm_gpiod_get_array_optional(struct device *dev, const char *con_id,
			      enum gpiod_flags flags);
void devm_gpiod_put(struct device *dev, struct gpio_desc *desc);
void devm_gpiod_unhinge(struct device *dev, struct gpio_desc *desc);
void devm_gpiod_put_array(struct device *dev, struct gpio_descs *descs);

int gpiod_get_direction(struct gpio_desc *desc);
int gpiod_direction_input(struct gpio_desc *desc);
int gpiod_direction_output(struct gpio_desc *desc, int value);
int gpiod_direction_output_raw(struct gpio_desc *desc, int value);
int gpiod_enable_hw_timestamp_ns(struct gpio_desc *desc, unsigned long flags);
int gpiod_disable_hw_timestamp_ns(struct gpio_desc *desc, unsigned long flags);


int gpiod_get_value(const struct gpio_desc *desc);
int gpiod_get_array_value(unsigned int array_size,
			  struct gpio_desc **desc_array,
			  struct gpio_array *array_info,
			  unsigned long *value_bitmap);
void gpiod_set_value(struct gpio_desc *desc, int value);
int gpiod_set_array_value(unsigned int array_size,
			  struct gpio_desc **desc_array,
			  struct gpio_array *array_info,
			  unsigned long *value_bitmap);
int gpiod_get_raw_value(const struct gpio_desc *desc);
int gpiod_get_raw_array_value(unsigned int array_size,
			      struct gpio_desc **desc_array,
			      struct gpio_array *array_info,
			      unsigned long *value_bitmap);
void gpiod_set_raw_value(struct gpio_desc *desc, int value);
int gpiod_set_raw_array_value(unsigned int array_size,
			      struct gpio_desc **desc_array,
			      struct gpio_array *array_info,
			      unsigned long *value_bitmap);


int gpiod_get_value_cansleep(const struct gpio_desc *desc);
int gpiod_get_array_value_cansleep(unsigned int array_size,
				   struct gpio_desc **desc_array,
				   struct gpio_array *array_info,
				   unsigned long *value_bitmap);
void gpiod_set_value_cansleep(struct gpio_desc *desc, int value);
int gpiod_set_array_value_cansleep(unsigned int array_size,
				   struct gpio_desc **desc_array,
				   struct gpio_array *array_info,
				   unsigned long *value_bitmap);
int gpiod_get_raw_value_cansleep(const struct gpio_desc *desc);
int gpiod_get_raw_array_value_cansleep(unsigned int array_size,
				       struct gpio_desc **desc_array,
				       struct gpio_array *array_info,
				       unsigned long *value_bitmap);
void gpiod_set_raw_value_cansleep(struct gpio_desc *desc, int value);
int gpiod_set_raw_array_value_cansleep(unsigned int array_size,
				       struct gpio_desc **desc_array,
				       struct gpio_array *array_info,
				       unsigned long *value_bitmap);

int gpiod_set_config(struct gpio_desc *desc, unsigned long config);
int gpiod_set_debounce(struct gpio_desc *desc, unsigned int debounce);
int gpiod_set_transitory(struct gpio_desc *desc, bool transitory);
void gpiod_toggle_active_low(struct gpio_desc *desc);

int gpiod_is_active_low(const struct gpio_desc *desc);
int gpiod_cansleep(const struct gpio_desc *desc);

int gpiod_to_irq(const struct gpio_desc *desc);
int gpiod_set_consumer_name(struct gpio_desc *desc, const char *name);


struct gpio_desc *gpio_to_desc(unsigned gpio);
int desc_to_gpio(const struct gpio_desc *desc);

struct gpio_desc *fwnode_gpiod_get_index(struct fwnode_handle *fwnode,
					 const char *con_id, int index,
					 enum gpiod_flags flags,
					 const char *label);
struct gpio_desc *devm_fwnode_gpiod_get_index(struct device *dev,
					      struct fwnode_handle *child,
					      const char *con_id, int index,
					      enum gpiod_flags flags,
					      const char *label);

#else 

#include <linux/err.h>
#include <linux/kernel.h>

#include <asm/bug.h>

static inline int gpiod_count(struct device *dev, const char *con_id)
{
	return 0;
}

static inline struct gpio_desc *__must_check gpiod_get(struct device *dev,
						       const char *con_id,
						       enum gpiod_flags flags)
{
	return ERR_PTR(-ENOSYS);
}
static inline struct gpio_desc *__must_check
gpiod_get_index(struct device *dev,
		const char *con_id,
		unsigned int idx,
		enum gpiod_flags flags)
{
	return ERR_PTR(-ENOSYS);
}

static inline struct gpio_desc *__must_check
gpiod_get_optional(struct device *dev, const char *con_id,
		   enum gpiod_flags flags)
{
	return NULL;
}

static inline struct gpio_desc *__must_check
gpiod_get_index_optional(struct device *dev, const char *con_id,
			 unsigned int index, enum gpiod_flags flags)
{
	return NULL;
}

static inline struct gpio_descs *__must_check
gpiod_get_array(struct device *dev, const char *con_id,
		enum gpiod_flags flags)
{
	return ERR_PTR(-ENOSYS);
}

static inline struct gpio_descs *__must_check
gpiod_get_array_optional(struct device *dev, const char *con_id,
			 enum gpiod_flags flags)
{
	return NULL;
}

static inline void gpiod_put(struct gpio_desc *desc)
{
	might_sleep();

	
	WARN_ON(desc);
}

static inline void devm_gpiod_unhinge(struct device *dev,
				      struct gpio_desc *desc)
{
	might_sleep();

	
	WARN_ON(desc);
}

static inline void gpiod_put_array(struct gpio_descs *descs)
{
	might_sleep();

	
	WARN_ON(descs);
}

static inline struct gpio_desc *__must_check
devm_gpiod_get(struct device *dev,
		 const char *con_id,
		 enum gpiod_flags flags)
{
	return ERR_PTR(-ENOSYS);
}
static inline
struct gpio_desc *__must_check
devm_gpiod_get_index(struct device *dev,
		       const char *con_id,
		       unsigned int idx,
		       enum gpiod_flags flags)
{
	return ERR_PTR(-ENOSYS);
}

static inline struct gpio_desc *__must_check
devm_gpiod_get_optional(struct device *dev, const char *con_id,
			  enum gpiod_flags flags)
{
	return NULL;
}

static inline struct gpio_desc *__must_check
devm_gpiod_get_index_optional(struct device *dev, const char *con_id,
				unsigned int index, enum gpiod_flags flags)
{
	return NULL;
}

static inline struct gpio_descs *__must_check
devm_gpiod_get_array(struct device *dev, const char *con_id,
		     enum gpiod_flags flags)
{
	return ERR_PTR(-ENOSYS);
}

static inline struct gpio_descs *__must_check
devm_gpiod_get_array_optional(struct device *dev, const char *con_id,
			      enum gpiod_flags flags)
{
	return NULL;
}

static inline void devm_gpiod_put(struct device *dev, struct gpio_desc *desc)
{
	might_sleep();

	
	WARN_ON(desc);
}

static inline void devm_gpiod_put_array(struct device *dev,
					struct gpio_descs *descs)
{
	might_sleep();

	
	WARN_ON(descs);
}


static inline int gpiod_get_direction(const struct gpio_desc *desc)
{
	
	WARN_ON(desc);
	return -ENOSYS;
}
static inline int gpiod_direction_input(struct gpio_desc *desc)
{
	
	WARN_ON(desc);
	return -ENOSYS;
}
static inline int gpiod_direction_output(struct gpio_desc *desc, int value)
{
	
	WARN_ON(desc);
	return -ENOSYS;
}
static inline int gpiod_direction_output_raw(struct gpio_desc *desc, int value)
{
	
	WARN_ON(desc);
	return -ENOSYS;
}
static inline int gpiod_enable_hw_timestamp_ns(struct gpio_desc *desc,
					       unsigned long flags)
{
	WARN_ON(desc);
	return -ENOSYS;
}
static inline int gpiod_disable_hw_timestamp_ns(struct gpio_desc *desc,
						unsigned long flags)
{
	WARN_ON(desc);
	return -ENOSYS;
}
static inline int gpiod_get_value(const struct gpio_desc *desc)
{
	
	WARN_ON(desc);
	return 0;
}
static inline int gpiod_get_array_value(unsigned int array_size,
					struct gpio_desc **desc_array,
					struct gpio_array *array_info,
					unsigned long *value_bitmap)
{
	
	WARN_ON(desc_array);
	return 0;
}
static inline void gpiod_set_value(struct gpio_desc *desc, int value)
{
	
	WARN_ON(desc);
}
static inline int gpiod_set_array_value(unsigned int array_size,
					struct gpio_desc **desc_array,
					struct gpio_array *array_info,
					unsigned long *value_bitmap)
{
	
	WARN_ON(desc_array);
	return 0;
}
static inline int gpiod_get_raw_value(const struct gpio_desc *desc)
{
	
	WARN_ON(desc);
	return 0;
}
static inline int gpiod_get_raw_array_value(unsigned int array_size,
					    struct gpio_desc **desc_array,
					    struct gpio_array *array_info,
					    unsigned long *value_bitmap)
{
	
	WARN_ON(desc_array);
	return 0;
}
static inline void gpiod_set_raw_value(struct gpio_desc *desc, int value)
{
	
	WARN_ON(desc);
}
static inline int gpiod_set_raw_array_value(unsigned int array_size,
					    struct gpio_desc **desc_array,
					    struct gpio_array *array_info,
					    unsigned long *value_bitmap)
{
	
	WARN_ON(desc_array);
	return 0;
}

static inline int gpiod_get_value_cansleep(const struct gpio_desc *desc)
{
	
	WARN_ON(desc);
	return 0;
}
static inline int gpiod_get_array_value_cansleep(unsigned int array_size,
				     struct gpio_desc **desc_array,
				     struct gpio_array *array_info,
				     unsigned long *value_bitmap)
{
	
	WARN_ON(desc_array);
	return 0;
}
static inline void gpiod_set_value_cansleep(struct gpio_desc *desc, int value)
{
	
	WARN_ON(desc);
}
static inline int gpiod_set_array_value_cansleep(unsigned int array_size,
					    struct gpio_desc **desc_array,
					    struct gpio_array *array_info,
					    unsigned long *value_bitmap)
{
	
	WARN_ON(desc_array);
	return 0;
}
static inline int gpiod_get_raw_value_cansleep(const struct gpio_desc *desc)
{
	
	WARN_ON(desc);
	return 0;
}
static inline int gpiod_get_raw_array_value_cansleep(unsigned int array_size,
					       struct gpio_desc **desc_array,
					       struct gpio_array *array_info,
					       unsigned long *value_bitmap)
{
	
	WARN_ON(desc_array);
	return 0;
}
static inline void gpiod_set_raw_value_cansleep(struct gpio_desc *desc,
						int value)
{
	
	WARN_ON(desc);
}
static inline int gpiod_set_raw_array_value_cansleep(unsigned int array_size,
						struct gpio_desc **desc_array,
						struct gpio_array *array_info,
						unsigned long *value_bitmap)
{
	
	WARN_ON(desc_array);
	return 0;
}

static inline int gpiod_set_config(struct gpio_desc *desc, unsigned long config)
{
	
	WARN_ON(desc);
	return -ENOSYS;
}

static inline int gpiod_set_debounce(struct gpio_desc *desc, unsigned int debounce)
{
	
	WARN_ON(desc);
	return -ENOSYS;
}

static inline int gpiod_set_transitory(struct gpio_desc *desc, bool transitory)
{
	
	WARN_ON(desc);
	return -ENOSYS;
}

static inline void gpiod_toggle_active_low(struct gpio_desc *desc)
{
	
	WARN_ON(desc);
}

static inline int gpiod_is_active_low(const struct gpio_desc *desc)
{
	
	WARN_ON(desc);
	return 0;
}
static inline int gpiod_cansleep(const struct gpio_desc *desc)
{
	
	WARN_ON(desc);
	return 0;
}

static inline int gpiod_to_irq(const struct gpio_desc *desc)
{
	
	WARN_ON(desc);
	return -EINVAL;
}

static inline int gpiod_set_consumer_name(struct gpio_desc *desc,
					  const char *name)
{
	
	WARN_ON(desc);
	return -EINVAL;
}

static inline struct gpio_desc *gpio_to_desc(unsigned gpio)
{
	return NULL;
}

static inline int desc_to_gpio(const struct gpio_desc *desc)
{
	
	WARN_ON(desc);
	return -EINVAL;
}

static inline
struct gpio_desc *fwnode_gpiod_get_index(struct fwnode_handle *fwnode,
					 const char *con_id, int index,
					 enum gpiod_flags flags,
					 const char *label)
{
	return ERR_PTR(-ENOSYS);
}

static inline
struct gpio_desc *devm_fwnode_gpiod_get_index(struct device *dev,
					      struct fwnode_handle *fwnode,
					      const char *con_id, int index,
					      enum gpiod_flags flags,
					      const char *label)
{
	return ERR_PTR(-ENOSYS);
}

#endif 

static inline
struct gpio_desc *devm_fwnode_gpiod_get(struct device *dev,
					struct fwnode_handle *fwnode,
					const char *con_id,
					enum gpiod_flags flags,
					const char *label)
{
	return devm_fwnode_gpiod_get_index(dev, fwnode, con_id, 0,
					   flags, label);
}

struct acpi_gpio_params {
	unsigned int crs_entry_index;
	unsigned int line_index;
	bool active_low;
};

struct acpi_gpio_mapping {
	const char *name;
	const struct acpi_gpio_params *data;
	unsigned int size;


#define ACPI_GPIO_QUIRK_NO_IO_RESTRICTION	BIT(0)

#define ACPI_GPIO_QUIRK_ONLY_GPIOIO		BIT(1)

#define ACPI_GPIO_QUIRK_ABSOLUTE_NUMBER		BIT(2)

	unsigned int quirks;
};

#if IS_ENABLED(CONFIG_GPIOLIB) && IS_ENABLED(CONFIG_ACPI)

int acpi_dev_add_driver_gpios(struct acpi_device *adev,
			      const struct acpi_gpio_mapping *gpios);
void acpi_dev_remove_driver_gpios(struct acpi_device *adev);

int devm_acpi_dev_add_driver_gpios(struct device *dev,
				   const struct acpi_gpio_mapping *gpios);

struct gpio_desc *acpi_get_and_request_gpiod(char *path, unsigned int pin, char *label);

#else  

#include <linux/err.h>

static inline int acpi_dev_add_driver_gpios(struct acpi_device *adev,
			      const struct acpi_gpio_mapping *gpios)
{
	return -ENXIO;
}
static inline void acpi_dev_remove_driver_gpios(struct acpi_device *adev) {}

static inline int devm_acpi_dev_add_driver_gpios(struct device *dev,
			      const struct acpi_gpio_mapping *gpios)
{
	return -ENXIO;
}

static inline struct gpio_desc *acpi_get_and_request_gpiod(char *path, unsigned int pin,
							   char *label)
{
	return ERR_PTR(-ENOSYS);
}

#endif 


#if IS_ENABLED(CONFIG_GPIOLIB) && IS_ENABLED(CONFIG_GPIO_SYSFS)

int gpiod_export(struct gpio_desc *desc, bool direction_may_change);
int gpiod_export_link(struct device *dev, const char *name,
		      struct gpio_desc *desc);
void gpiod_unexport(struct gpio_desc *desc);

#else  

#include <asm/errno.h>

static inline int gpiod_export(struct gpio_desc *desc,
			       bool direction_may_change)
{
	return -ENOSYS;
}

static inline int gpiod_export_link(struct device *dev, const char *name,
				    struct gpio_desc *desc)
{
	return -ENOSYS;
}

static inline void gpiod_unexport(struct gpio_desc *desc)
{
}

#endif 

#endif
