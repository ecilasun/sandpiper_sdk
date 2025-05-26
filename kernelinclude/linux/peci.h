/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_PECI_H
#define __LINUX_PECI_H

#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/mutex.h>
#include <linux/types.h>


#define PECI_REQUEST_MAX_BUF_SIZE 32

struct peci_controller;
struct peci_request;


struct peci_controller_ops {
	int (*xfer)(struct peci_controller *controller, u8 addr, struct peci_request *req);
};


struct peci_controller {
	struct device dev;
	const struct peci_controller_ops *ops;
	struct mutex bus_lock; 
	u8 id;
};

struct peci_controller *devm_peci_controller_add(struct device *parent,
						 const struct peci_controller_ops *ops);

static inline struct peci_controller *to_peci_controller(void *d)
{
	return container_of(d, struct peci_controller, dev);
}


struct peci_device {
	struct device dev;
	struct {
		u16 family;
		u8 model;
		u8 peci_revision;
		u8 socket_id;
	} info;
	u8 addr;
	bool deleted;
};

static inline struct peci_device *to_peci_device(struct device *d)
{
	return container_of(d, struct peci_device, dev);
}


struct peci_request {
	struct peci_device *device;
	struct {
		u8 buf[PECI_REQUEST_MAX_BUF_SIZE];
		u8 len;
	} rx, tx;
};

#endif 
