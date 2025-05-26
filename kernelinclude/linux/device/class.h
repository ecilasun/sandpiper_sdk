// SPDX-License-Identifier: GPL-2.0


#ifndef _DEVICE_CLASS_H_
#define _DEVICE_CLASS_H_

#include <linux/kobject.h>
#include <linux/klist.h>
#include <linux/pm.h>
#include <linux/device/bus.h>

struct device;
struct fwnode_handle;


struct class {
	const char		*name;

	const struct attribute_group	**class_groups;
	const struct attribute_group	**dev_groups;

	int (*dev_uevent)(const struct device *dev, struct kobj_uevent_env *env);
	char *(*devnode)(const struct device *dev, umode_t *mode);

	void (*class_release)(const struct class *class);
	void (*dev_release)(struct device *dev);

	int (*shutdown_pre)(struct device *dev);

	const struct kobj_ns_type_operations *ns_type;
	const void *(*namespace)(const struct device *dev);

	void (*get_ownership)(const struct device *dev, kuid_t *uid, kgid_t *gid);

	const struct dev_pm_ops *pm;
};

struct class_dev_iter {
	struct klist_iter		ki;
	const struct device_type	*type;
	struct subsys_private		*sp;
};

int __must_check class_register(const struct class *class);
void class_unregister(const struct class *class);
bool class_is_registered(const struct class *class);

struct class_compat;
struct class_compat *class_compat_register(const char *name);
void class_compat_unregister(struct class_compat *cls);
int class_compat_create_link(struct class_compat *cls, struct device *dev,
			     struct device *device_link);
void class_compat_remove_link(struct class_compat *cls, struct device *dev,
			      struct device *device_link);

void class_dev_iter_init(struct class_dev_iter *iter, const struct class *class,
			 const struct device *start, const struct device_type *type);
struct device *class_dev_iter_next(struct class_dev_iter *iter);
void class_dev_iter_exit(struct class_dev_iter *iter);

int class_for_each_device(const struct class *class, const struct device *start, void *data,
			  int (*fn)(struct device *dev, void *data));
struct device *class_find_device(const struct class *class, const struct device *start,
				 const void *data, int (*match)(struct device *, const void *));


static inline struct device *class_find_device_by_name(const struct class *class,
						       const char *name)
{
	return class_find_device(class, NULL, name, device_match_name);
}


static inline struct device *class_find_device_by_of_node(const struct class *class,
							  const struct device_node *np)
{
	return class_find_device(class, NULL, np, device_match_of_node);
}


static inline struct device *class_find_device_by_fwnode(const struct class *class,
							 const struct fwnode_handle *fwnode)
{
	return class_find_device(class, NULL, fwnode, device_match_fwnode);
}


static inline struct device *class_find_device_by_devt(const struct class *class,
						       dev_t devt)
{
	return class_find_device(class, NULL, &devt, device_match_devt);
}

#ifdef CONFIG_ACPI
struct acpi_device;

static inline struct device *class_find_device_by_acpi_dev(const struct class *class,
							   const struct acpi_device *adev)
{
	return class_find_device(class, NULL, adev, device_match_acpi_dev);
}
#else
static inline struct device *class_find_device_by_acpi_dev(const struct class *class,
							   const void *adev)
{
	return NULL;
}
#endif

struct class_attribute {
	struct attribute attr;
	ssize_t (*show)(const struct class *class, const struct class_attribute *attr,
			char *buf);
	ssize_t (*store)(const struct class *class, const struct class_attribute *attr,
			 const char *buf, size_t count);
};

#define CLASS_ATTR_RW(_name) \
	struct class_attribute class_attr_##_name = __ATTR_RW(_name)
#define CLASS_ATTR_RO(_name) \
	struct class_attribute class_attr_##_name = __ATTR_RO(_name)
#define CLASS_ATTR_WO(_name) \
	struct class_attribute class_attr_##_name = __ATTR_WO(_name)

int __must_check class_create_file_ns(const struct class *class, const struct class_attribute *attr,
				      const void *ns);
void class_remove_file_ns(const struct class *class, const struct class_attribute *attr,
			  const void *ns);

static inline int __must_check class_create_file(const struct class *class,
						 const struct class_attribute *attr)
{
	return class_create_file_ns(class, attr, NULL);
}

static inline void class_remove_file(const struct class *class,
				     const struct class_attribute *attr)
{
	return class_remove_file_ns(class, attr, NULL);
}


struct class_attribute_string {
	struct class_attribute attr;
	char *str;
};


#define _CLASS_ATTR_STRING(_name, _mode, _str) \
	{ __ATTR(_name, _mode, show_class_attr_string, NULL), _str }
#define CLASS_ATTR_STRING(_name, _mode, _str) \
	struct class_attribute_string class_attr_##_name = \
		_CLASS_ATTR_STRING(_name, _mode, _str)

ssize_t show_class_attr_string(const struct class *class, const struct class_attribute *attr,
			       char *buf);

struct class_interface {
	struct list_head	node;
	const struct class	*class;

	int (*add_dev)		(struct device *dev);
	void (*remove_dev)	(struct device *dev);
};

int __must_check class_interface_register(struct class_interface *);
void class_interface_unregister(struct class_interface *);

struct class * __must_check class_create(const char *name);
void class_destroy(const struct class *cls);

#endif	
