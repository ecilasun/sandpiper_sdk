/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_GREYBUS_H
#define __LINUX_GREYBUS_H

#ifdef __KERNEL__

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/pm_runtime.h>
#include <linux/idr.h>

#include <linux/greybus/greybus_id.h>
#include <linux/greybus/greybus_manifest.h>
#include <linux/greybus/greybus_protocols.h>
#include <linux/greybus/manifest.h>
#include <linux/greybus/hd.h>
#include <linux/greybus/svc.h>
#include <linux/greybus/control.h>
#include <linux/greybus/module.h>
#include <linux/greybus/interface.h>
#include <linux/greybus/bundle.h>
#include <linux/greybus/connection.h>
#include <linux/greybus/operation.h>


#define GREYBUS_VERSION_MAJOR	0x00
#define GREYBUS_VERSION_MINOR	0x01

#define GREYBUS_ID_MATCH_DEVICE \
	(GREYBUS_ID_MATCH_VENDOR | GREYBUS_ID_MATCH_PRODUCT)

#define GREYBUS_DEVICE(v, p)					\
	.match_flags	= GREYBUS_ID_MATCH_DEVICE,		\
	.vendor		= (v),					\
	.product	= (p),

#define GREYBUS_DEVICE_CLASS(c)					\
	.match_flags	= GREYBUS_ID_MATCH_CLASS,		\
	.class		= (c),


#define CPORT_ID_MAX	4095		
#define CPORT_ID_BAD	U16_MAX

struct greybus_driver {
	const char *name;

	int (*probe)(struct gb_bundle *bundle,
		     const struct greybus_bundle_id *id);
	void (*disconnect)(struct gb_bundle *bundle);

	const struct greybus_bundle_id *id_table;

	struct device_driver driver;
};
#define to_greybus_driver(d) container_of(d, struct greybus_driver, driver)

static inline void greybus_set_drvdata(struct gb_bundle *bundle, void *data)
{
	dev_set_drvdata(&bundle->dev, data);
}

static inline void *greybus_get_drvdata(struct gb_bundle *bundle)
{
	return dev_get_drvdata(&bundle->dev);
}


int greybus_register_driver(struct greybus_driver *driver,
			    struct module *module, const char *mod_name);
void greybus_deregister_driver(struct greybus_driver *driver);


#define greybus_register(driver) \
	greybus_register_driver(driver, THIS_MODULE, KBUILD_MODNAME)
#define greybus_deregister(driver) \
	greybus_deregister_driver(driver)


#define module_greybus_driver(__greybus_driver)	\
	module_driver(__greybus_driver, greybus_register, greybus_deregister)

int greybus_disabled(void);

void gb_debugfs_init(void);
void gb_debugfs_cleanup(void);
struct dentry *gb_debugfs_get(void);

extern struct bus_type greybus_bus_type;

extern struct device_type greybus_hd_type;
extern struct device_type greybus_module_type;
extern struct device_type greybus_interface_type;
extern struct device_type greybus_control_type;
extern struct device_type greybus_bundle_type;
extern struct device_type greybus_svc_type;

static inline int is_gb_host_device(const struct device *dev)
{
	return dev->type == &greybus_hd_type;
}

static inline int is_gb_module(const struct device *dev)
{
	return dev->type == &greybus_module_type;
}

static inline int is_gb_interface(const struct device *dev)
{
	return dev->type == &greybus_interface_type;
}

static inline int is_gb_control(const struct device *dev)
{
	return dev->type == &greybus_control_type;
}

static inline int is_gb_bundle(const struct device *dev)
{
	return dev->type == &greybus_bundle_type;
}

static inline int is_gb_svc(const struct device *dev)
{
	return dev->type == &greybus_svc_type;
}

static inline bool cport_id_valid(struct gb_host_device *hd, u16 cport_id)
{
	return cport_id != CPORT_ID_BAD && cport_id < hd->num_cports;
}

#endif 
#endif 
