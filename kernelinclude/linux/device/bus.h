// SPDX-License-Identifier: GPL-2.0


#ifndef _DEVICE_BUS_H_
#define _DEVICE_BUS_H_

#include <linux/kobject.h>
#include <linux/klist.h>
#include <linux/pm.h>

struct device_driver;
struct fwnode_handle;


struct bus_type {
	const char		*name;
	const char		*dev_name;
	const struct attribute_group **bus_groups;
	const struct attribute_group **dev_groups;
	const struct attribute_group **drv_groups;

	int (*match)(struct device *dev, struct device_driver *drv);
	int (*uevent)(const struct device *dev, struct kobj_uevent_env *env);
	int (*probe)(struct device *dev);
	void (*sync_state)(struct device *dev);
	void (*remove)(struct device *dev);
	void (*shutdown)(struct device *dev);

	int (*online)(struct device *dev);
	int (*offline)(struct device *dev);

	int (*suspend)(struct device *dev, pm_message_t state);
	int (*resume)(struct device *dev);

	int (*num_vf)(struct device *dev);

	int (*dma_configure)(struct device *dev);
	void (*dma_cleanup)(struct device *dev);

	const struct dev_pm_ops *pm;

	const struct iommu_ops *iommu_ops;

	bool need_parent_lock;
};

int __must_check bus_register(const struct bus_type *bus);

void bus_unregister(const struct bus_type *bus);

int __must_check bus_rescan_devices(const struct bus_type *bus);

struct bus_attribute {
	struct attribute	attr;
	ssize_t (*show)(const struct bus_type *bus, char *buf);
	ssize_t (*store)(const struct bus_type *bus, const char *buf, size_t count);
};

#define BUS_ATTR_RW(_name) \
	struct bus_attribute bus_attr_##_name = __ATTR_RW(_name)
#define BUS_ATTR_RO(_name) \
	struct bus_attribute bus_attr_##_name = __ATTR_RO(_name)
#define BUS_ATTR_WO(_name) \
	struct bus_attribute bus_attr_##_name = __ATTR_WO(_name)

int __must_check bus_create_file(const struct bus_type *bus, struct bus_attribute *attr);
void bus_remove_file(const struct bus_type *bus, struct bus_attribute *attr);


int device_match_name(struct device *dev, const void *name);
int device_match_of_node(struct device *dev, const void *np);
int device_match_fwnode(struct device *dev, const void *fwnode);
int device_match_devt(struct device *dev, const void *pdevt);
int device_match_acpi_dev(struct device *dev, const void *adev);
int device_match_acpi_handle(struct device *dev, const void *handle);
int device_match_any(struct device *dev, const void *unused);


int bus_for_each_dev(const struct bus_type *bus, struct device *start, void *data,
		     int (*fn)(struct device *dev, void *data));
struct device *bus_find_device(const struct bus_type *bus, struct device *start,
			       const void *data,
			       int (*match)(struct device *dev, const void *data));

static inline struct device *bus_find_device_by_name(const struct bus_type *bus,
						     struct device *start,
						     const char *name)
{
	return bus_find_device(bus, start, name, device_match_name);
}


static inline struct device *
bus_find_device_by_of_node(const struct bus_type *bus, const struct device_node *np)
{
	return bus_find_device(bus, NULL, np, device_match_of_node);
}


static inline struct device *
bus_find_device_by_fwnode(const struct bus_type *bus, const struct fwnode_handle *fwnode)
{
	return bus_find_device(bus, NULL, fwnode, device_match_fwnode);
}


static inline struct device *bus_find_device_by_devt(const struct bus_type *bus,
						     dev_t devt)
{
	return bus_find_device(bus, NULL, &devt, device_match_devt);
}


static inline struct device *
bus_find_next_device(const struct bus_type *bus,struct device *cur)
{
	return bus_find_device(bus, cur, NULL, device_match_any);
}

#ifdef CONFIG_ACPI
struct acpi_device;


static inline struct device *
bus_find_device_by_acpi_dev(const struct bus_type *bus, const struct acpi_device *adev)
{
	return bus_find_device(bus, NULL, adev, device_match_acpi_dev);
}
#else
static inline struct device *
bus_find_device_by_acpi_dev(const struct bus_type *bus, const void *adev)
{
	return NULL;
}
#endif

int bus_for_each_drv(const struct bus_type *bus, struct device_driver *start,
		     void *data, int (*fn)(struct device_driver *, void *));
void bus_sort_breadthfirst(struct bus_type *bus,
			   int (*compare)(const struct device *a,
					  const struct device *b));

struct notifier_block;

int bus_register_notifier(const struct bus_type *bus, struct notifier_block *nb);
int bus_unregister_notifier(const struct bus_type *bus, struct notifier_block *nb);


enum bus_notifier_event {
	BUS_NOTIFY_ADD_DEVICE,
	BUS_NOTIFY_DEL_DEVICE,
	BUS_NOTIFY_REMOVED_DEVICE,
	BUS_NOTIFY_BIND_DRIVER,
	BUS_NOTIFY_BOUND_DRIVER,
	BUS_NOTIFY_UNBIND_DRIVER,
	BUS_NOTIFY_UNBOUND_DRIVER,
	BUS_NOTIFY_DRIVER_NOT_BOUND,
};

struct kset *bus_get_kset(const struct bus_type *bus);
struct device *bus_get_dev_root(const struct bus_type *bus);

#endif
