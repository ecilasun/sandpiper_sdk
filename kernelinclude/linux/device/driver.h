// SPDX-License-Identifier: GPL-2.0


#ifndef _DEVICE_DRIVER_H_
#define _DEVICE_DRIVER_H_

#include <linux/kobject.h>
#include <linux/klist.h>
#include <linux/pm.h>
#include <linux/device/bus.h>
#include <linux/module.h>


enum probe_type {
	PROBE_DEFAULT_STRATEGY,
	PROBE_PREFER_ASYNCHRONOUS,
	PROBE_FORCE_SYNCHRONOUS,
};


struct device_driver {
	const char		*name;
	const struct bus_type	*bus;

	struct module		*owner;
	const char		*mod_name;	

	bool suppress_bind_attrs;	
	enum probe_type probe_type;

	const struct of_device_id	*of_match_table;
	const struct acpi_device_id	*acpi_match_table;

	int (*probe) (struct device *dev);
	void (*sync_state)(struct device *dev);
	int (*remove) (struct device *dev);
	void (*shutdown) (struct device *dev);
	int (*suspend) (struct device *dev, pm_message_t state);
	int (*resume) (struct device *dev);
	const struct attribute_group **groups;
	const struct attribute_group **dev_groups;

	const struct dev_pm_ops *pm;
	void (*coredump) (struct device *dev);

	struct driver_private *p;
};


int __must_check driver_register(struct device_driver *drv);
void driver_unregister(struct device_driver *drv);

struct device_driver *driver_find(const char *name, const struct bus_type *bus);
bool __init driver_probe_done(void);
void wait_for_device_probe(void);
void __init wait_for_init_devices_probe(void);



struct driver_attribute {
	struct attribute attr;
	ssize_t (*show)(struct device_driver *driver, char *buf);
	ssize_t (*store)(struct device_driver *driver, const char *buf,
			 size_t count);
};

#define DRIVER_ATTR_RW(_name) \
	struct driver_attribute driver_attr_##_name = __ATTR_RW(_name)
#define DRIVER_ATTR_RO(_name) \
	struct driver_attribute driver_attr_##_name = __ATTR_RO(_name)
#define DRIVER_ATTR_WO(_name) \
	struct driver_attribute driver_attr_##_name = __ATTR_WO(_name)

int __must_check driver_create_file(struct device_driver *driver,
				    const struct driver_attribute *attr);
void driver_remove_file(struct device_driver *driver,
			const struct driver_attribute *attr);

int driver_set_override(struct device *dev, const char **override,
			const char *s, size_t len);
int __must_check driver_for_each_device(struct device_driver *drv, struct device *start,
					void *data, int (*fn)(struct device *dev, void *));
struct device *driver_find_device(struct device_driver *drv,
				  struct device *start, const void *data,
				  int (*match)(struct device *dev, const void *data));


static inline struct device *driver_find_device_by_name(struct device_driver *drv,
							const char *name)
{
	return driver_find_device(drv, NULL, name, device_match_name);
}


static inline struct device *
driver_find_device_by_of_node(struct device_driver *drv,
			      const struct device_node *np)
{
	return driver_find_device(drv, NULL, np, device_match_of_node);
}


static inline struct device *
driver_find_device_by_fwnode(struct device_driver *drv,
			     const struct fwnode_handle *fwnode)
{
	return driver_find_device(drv, NULL, fwnode, device_match_fwnode);
}


static inline struct device *driver_find_device_by_devt(struct device_driver *drv,
							dev_t devt)
{
	return driver_find_device(drv, NULL, &devt, device_match_devt);
}

static inline struct device *driver_find_next_device(struct device_driver *drv,
						     struct device *start)
{
	return driver_find_device(drv, start, NULL, device_match_any);
}

#ifdef CONFIG_ACPI

static inline struct device *
driver_find_device_by_acpi_dev(struct device_driver *drv,
			       const struct acpi_device *adev)
{
	return driver_find_device(drv, NULL, adev, device_match_acpi_dev);
}
#else
static inline struct device *
driver_find_device_by_acpi_dev(struct device_driver *drv, const void *adev)
{
	return NULL;
}
#endif

void driver_deferred_probe_add(struct device *dev);
int driver_deferred_probe_check_state(struct device *dev);
void driver_init(void);


#define module_driver(__driver, __register, __unregister, ...) \
static int __init __driver##_init(void) \
{ \
	return __register(&(__driver) , ##__VA_ARGS__); \
} \
module_init(__driver##_init); \
static void __exit __driver##_exit(void) \
{ \
	__unregister(&(__driver) , ##__VA_ARGS__); \
} \
module_exit(__driver##_exit);


#define builtin_driver(__driver, __register, ...) \
static int __init __driver##_init(void) \
{ \
	return __register(&(__driver) , ##__VA_ARGS__); \
} \
device_initcall(__driver##_init);

#endif	
