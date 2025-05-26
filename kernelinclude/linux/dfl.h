/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_DFL_H
#define __LINUX_DFL_H

#include <linux/device.h>
#include <linux/mod_devicetable.h>


enum dfl_id_type {
	FME_ID = 0,
	PORT_ID = 1,
	DFL_ID_MAX,
};


struct dfl_device {
	struct device dev;
	int id;
	u16 type;
	u16 feature_id;
	u8 revision;
	struct resource mmio_res;
	int *irqs;
	unsigned int num_irqs;
	struct dfl_fpga_cdev *cdev;
	const struct dfl_device_id *id_entry;
	u8 dfh_version;
	unsigned int param_size;
	void *params;
};


struct dfl_driver {
	struct device_driver drv;
	const struct dfl_device_id *id_table;

	int (*probe)(struct dfl_device *dfl_dev);
	void (*remove)(struct dfl_device *dfl_dev);
};

#define to_dfl_dev(d) container_of(d, struct dfl_device, dev)
#define to_dfl_drv(d) container_of(d, struct dfl_driver, drv)


#define dfl_driver_register(drv) \
	__dfl_driver_register(drv, THIS_MODULE)
int __dfl_driver_register(struct dfl_driver *dfl_drv, struct module *owner);
void dfl_driver_unregister(struct dfl_driver *dfl_drv);


#define module_dfl_driver(__dfl_driver) \
	module_driver(__dfl_driver, dfl_driver_register, \
		      dfl_driver_unregister)

void *dfh_find_param(struct dfl_device *dfl_dev, int param_id, size_t *pcount);
#endif 
