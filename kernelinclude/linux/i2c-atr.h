/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_I2C_ATR_H
#define _LINUX_I2C_ATR_H

#include <linux/i2c.h>
#include <linux/types.h>

struct device;
struct fwnode_handle;
struct i2c_atr;


struct i2c_atr_ops {
	int (*attach_client)(struct i2c_atr *atr, u32 chan_id,
			     const struct i2c_client *client, u16 alias);
	void (*detach_client)(struct i2c_atr *atr, u32 chan_id,
			      const struct i2c_client *client);
};


struct i2c_atr *i2c_atr_new(struct i2c_adapter *parent, struct device *dev,
			    const struct i2c_atr_ops *ops, int max_adapters);


void i2c_atr_delete(struct i2c_atr *atr);


int i2c_atr_add_adapter(struct i2c_atr *atr, u32 chan_id,
			struct device *adapter_parent,
			struct fwnode_handle *bus_handle);


void i2c_atr_del_adapter(struct i2c_atr *atr, u32 chan_id);


void i2c_atr_set_driver_data(struct i2c_atr *atr, void *data);


void *i2c_atr_get_driver_data(struct i2c_atr *atr);

#endif 
