// SPDX-License-Identifier: GPL-2.0


#ifndef _LINUX_SLIMBUS_H
#define _LINUX_SLIMBUS_H
#include <linux/device.h>
#include <linux/module.h>
#include <linux/completion.h>
#include <linux/mod_devicetable.h>

extern struct bus_type slimbus_bus;


struct slim_eaddr {
	u8 instance;
	u8 dev_index;
	u16 prod_code;
	u16 manf_id;
} __packed;


enum slim_device_status {
	SLIM_DEVICE_STATUS_DOWN = 0,
	SLIM_DEVICE_STATUS_UP,
	SLIM_DEVICE_STATUS_RESERVED,
};

struct slim_controller;


struct slim_device {
	struct device		dev;
	struct slim_eaddr	e_addr;
	struct slim_controller	*ctrl;
	enum slim_device_status	status;
	u8			laddr;
	bool			is_laddr_valid;
	struct list_head	stream_list;
	spinlock_t stream_list_lock;
};

#define to_slim_device(d) container_of(d, struct slim_device, dev)



struct slim_driver {
	int	(*probe)(struct slim_device *sl);
	void	(*remove)(struct slim_device *sl);
	void	(*shutdown)(struct slim_device *sl);
	int	(*device_status)(struct slim_device *sl,
				 enum slim_device_status s);
	struct device_driver		driver;
	const struct slim_device_id	*id_table;
};
#define to_slim_driver(d) container_of(d, struct slim_driver, driver)


struct slim_val_inf {
	u16			start_offset;
	u8			num_bytes;
	u8			*rbuf;
	const u8		*wbuf;
	struct	completion	*comp;
};

#define SLIM_DEVICE_MAX_CHANNELS	256

#define SLIM_DEVICE_MAX_PORTS		32


struct slim_stream_config {
	unsigned int rate;
	unsigned int bps;
	
	unsigned int ch_count;
	unsigned int *chs;
	
	unsigned long port_mask;
	int direction;
};


#define slim_driver_register(drv) \
	__slim_driver_register(drv, THIS_MODULE)
int __slim_driver_register(struct slim_driver *drv, struct module *owner);
void slim_driver_unregister(struct slim_driver *drv);


#define module_slim_driver(__slim_driver) \
	module_driver(__slim_driver, slim_driver_register, \
			slim_driver_unregister)

static inline void *slim_get_devicedata(const struct slim_device *dev)
{
	return dev_get_drvdata(&dev->dev);
}

static inline void slim_set_devicedata(struct slim_device *dev, void *data)
{
	dev_set_drvdata(&dev->dev, data);
}

struct slim_device *of_slim_get_device(struct slim_controller *ctrl,
				       struct device_node *np);
struct slim_device *slim_get_device(struct slim_controller *ctrl,
				    struct slim_eaddr *e_addr);
int slim_get_logical_addr(struct slim_device *sbdev);


#define SLIM_MSG_MC_REQUEST_INFORMATION          0x20
#define SLIM_MSG_MC_REQUEST_CLEAR_INFORMATION    0x21
#define SLIM_MSG_MC_REPLY_INFORMATION            0x24
#define SLIM_MSG_MC_CLEAR_INFORMATION            0x28
#define SLIM_MSG_MC_REPORT_INFORMATION           0x29


#define SLIM_MSG_MC_REQUEST_VALUE                0x60
#define SLIM_MSG_MC_REQUEST_CHANGE_VALUE         0x61
#define SLIM_MSG_MC_REPLY_VALUE                  0x64
#define SLIM_MSG_MC_CHANGE_VALUE                 0x68

int slim_xfer_msg(struct slim_device *sbdev, struct slim_val_inf *msg,
		  u8 mc);
int slim_readb(struct slim_device *sdev, u32 addr);
int slim_writeb(struct slim_device *sdev, u32 addr, u8 value);
int slim_read(struct slim_device *sdev, u32 addr, size_t count, u8 *val);
int slim_write(struct slim_device *sdev, u32 addr, size_t count, u8 *val);


struct slim_stream_runtime;
struct slim_stream_runtime *slim_stream_allocate(struct slim_device *dev,
						 const char *sname);
int slim_stream_prepare(struct slim_stream_runtime *stream,
			struct slim_stream_config *c);
int slim_stream_enable(struct slim_stream_runtime *stream);
int slim_stream_disable(struct slim_stream_runtime *stream);
int slim_stream_unprepare(struct slim_stream_runtime *stream);
int slim_stream_free(struct slim_stream_runtime *stream);

#endif 
