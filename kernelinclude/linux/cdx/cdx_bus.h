/* SPDX-License-Identifier: GPL-2.0
 *
 * CDX bus public interface
 *
 * Copyright (C) 2022-2023, Advanced Micro Devices, Inc.
 *
 */

#ifndef _CDX_BUS_H_
#define _CDX_BUS_H_

#include <linux/device.h>
#include <linux/list.h>
#include <linux/mod_devicetable.h>

#define MAX_CDX_DEV_RESOURCES	4
#define CDX_CONTROLLER_ID_SHIFT 4
#define CDX_BUS_NUM_MASK 0xF


struct cdx_controller;

enum {
	CDX_DEV_MSI_CONF,
	CDX_DEV_BUS_MASTER_CONF,
	CDX_DEV_RESET_CONF,
	CDX_DEV_MSI_ENABLE,
};

struct cdx_msi_config {
	u16 msi_index;
	u32 data;
	u64 addr;
};

struct cdx_device_config {
	u8 type;
	struct cdx_msi_config msi;
	bool bus_master_enable;
	bool msi_enable;
};

typedef int (*cdx_bus_enable_cb)(struct cdx_controller *cdx, u8 bus_num);

typedef int (*cdx_bus_disable_cb)(struct cdx_controller *cdx, u8 bus_num);

typedef int (*cdx_scan_cb)(struct cdx_controller *cdx);

typedef int (*cdx_dev_configure_cb)(struct cdx_controller *cdx,
				    u8 bus_num, u8 dev_num,
				    struct cdx_device_config *dev_config);


#define CDX_DEVICE(vend, dev) \
	.vendor = (vend), .device = (dev), \
	.subvendor = CDX_ANY_ID, .subdevice = CDX_ANY_ID


#define CDX_DEVICE_DRIVER_OVERRIDE(vend, dev, driver_override) \
	.vendor = (vend), .device = (dev), .subvendor = CDX_ANY_ID,\
	.subdevice = CDX_ANY_ID, .override_only = (driver_override)


struct cdx_ops {
	cdx_bus_enable_cb bus_enable;
	cdx_bus_disable_cb bus_disable;
	cdx_scan_cb scan;
	cdx_dev_configure_cb dev_configure;
};


struct cdx_controller {
	struct device *dev;
	void *priv;
	struct irq_domain *msi_domain;
	u32 id;
	bool controller_registered;
	struct cdx_ops *ops;
};


struct cdx_device {
	struct device dev;
	struct cdx_controller *cdx;
	u16 vendor;
	u16 device;
	u16 subsystem_vendor;
	u16 subsystem_device;
	u32 class;
	u8 revision;
	u8 bus_num;
	u8 dev_num;
	struct resource res[MAX_CDX_DEV_RESOURCES];
	struct bin_attribute *res_attr[MAX_CDX_DEV_RESOURCES];
	struct dentry *debugfs_dir;
	u8 res_count;
	u64 dma_mask;
	u16 flags;
	u32 req_id;
	bool is_bus;
	bool enabled;
	u32 msi_dev_id;
	u32 num_msi;
	const char *driver_override;
	struct mutex irqchip_lock; 
	bool msi_write_pending;
};

#define to_cdx_device(_dev) \
	container_of(_dev, struct cdx_device, dev)

#define cdx_resource_start(dev, num)	((dev)->res[(num)].start)
#define cdx_resource_end(dev, num)	((dev)->res[(num)].end)
#define cdx_resource_flags(dev, num)	((dev)->res[(num)].flags)
#define cdx_resource_len(dev, num) \
	((cdx_resource_start((dev), (num)) == 0 &&	\
	  cdx_resource_end((dev), (num)) ==		\
	  cdx_resource_start((dev), (num))) ? 0 :	\
	 (cdx_resource_end((dev), (num)) -		\
	  cdx_resource_start((dev), (num)) + 1))

struct cdx_driver {
	struct device_driver driver;
	const struct cdx_device_id *match_id_table;
	int (*probe)(struct cdx_device *dev);
	int (*remove)(struct cdx_device *dev);
	void (*shutdown)(struct cdx_device *dev);
	void (*reset_prepare)(struct cdx_device *dev);
	void (*reset_done)(struct cdx_device *dev);
	bool driver_managed_dma;
};

#define to_cdx_driver(_drv) \
	container_of(_drv, struct cdx_driver, driver)


#define cdx_driver_register(drv) \
	__cdx_driver_register(drv, THIS_MODULE)


int __must_check __cdx_driver_register(struct cdx_driver *cdx_driver,
				       struct module *owner);


void cdx_driver_unregister(struct cdx_driver *cdx_driver);

extern struct bus_type cdx_bus_type;


int cdx_msi_domain_alloc_irqs(struct device *dev, unsigned int irq_count);


#define cdx_msi_domain_free_irqs msi_domain_free_irqs_all


int cdx_dev_reset(struct device *dev);


int cdx_set_master(struct cdx_device *cdx_dev);


int cdx_clear_master(struct cdx_device *cdx_dev);


int cdx_enable_msi(struct cdx_device *cdx_dev);


void cdx_disable_msi(struct cdx_device *cdx_dev);

#endif 
