/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_I2C_SMBUS_H
#define _LINUX_I2C_SMBUS_H

#include <linux/i2c.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>



struct i2c_smbus_alert_setup {
	int			irq;
};

struct i2c_client *i2c_new_smbus_alert_device(struct i2c_adapter *adapter,
					      struct i2c_smbus_alert_setup *setup);
int i2c_handle_smbus_alert(struct i2c_client *ara);

#if IS_ENABLED(CONFIG_I2C_SMBUS) && IS_ENABLED(CONFIG_I2C_SLAVE)
struct i2c_client *i2c_new_slave_host_notify_device(struct i2c_adapter *adapter);
void i2c_free_slave_host_notify_device(struct i2c_client *client);
#else
static inline struct i2c_client *i2c_new_slave_host_notify_device(struct i2c_adapter *adapter)
{
	return ERR_PTR(-ENOSYS);
}
static inline void i2c_free_slave_host_notify_device(struct i2c_client *client)
{
}
#endif

#if IS_ENABLED(CONFIG_I2C_SMBUS) && IS_ENABLED(CONFIG_DMI)
void i2c_register_spd(struct i2c_adapter *adap);
#else
static inline void i2c_register_spd(struct i2c_adapter *adap) { }
#endif

#endif 
