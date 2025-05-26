/* SPDX-License-Identifier: GPL-2.0 */


#ifndef I3C_DEV_H
#define I3C_DEV_H

#include <linux/bitops.h>
#include <linux/device.h>
#include <linux/i2c.h>
#include <linux/kconfig.h>
#include <linux/mod_devicetable.h>
#include <linux/module.h>


enum i3c_error_code {
	I3C_ERROR_UNKNOWN = 0,
	I3C_ERROR_M0 = 1,
	I3C_ERROR_M1,
	I3C_ERROR_M2,
};


enum i3c_hdr_mode {
	I3C_HDR_DDR,
	I3C_HDR_TSP,
	I3C_HDR_TSL,
};


struct i3c_priv_xfer {
	u8 rnw;
	u16 len;
	u16 actual_len;
	union {
		void *in;
		const void *out;
	} data;
	enum i3c_error_code err;
};


enum i3c_dcr {
	I3C_DCR_GENERIC_DEVICE = 0,
};

#define I3C_PID_MANUF_ID(pid)		(((pid) & GENMASK_ULL(47, 33)) >> 33)
#define I3C_PID_RND_LOWER_32BITS(pid)	(!!((pid) & BIT_ULL(32)))
#define I3C_PID_RND_VAL(pid)		((pid) & GENMASK_ULL(31, 0))
#define I3C_PID_PART_ID(pid)		(((pid) & GENMASK_ULL(31, 16)) >> 16)
#define I3C_PID_INSTANCE_ID(pid)	(((pid) & GENMASK_ULL(15, 12)) >> 12)
#define I3C_PID_EXTRA_INFO(pid)		((pid) & GENMASK_ULL(11, 0))

#define I3C_BCR_DEVICE_ROLE(bcr)	((bcr) & GENMASK(7, 6))
#define I3C_BCR_I3C_SLAVE		(0 << 6)
#define I3C_BCR_I3C_MASTER		(1 << 6)
#define I3C_BCR_HDR_CAP			BIT(5)
#define I3C_BCR_BRIDGE			BIT(4)
#define I3C_BCR_OFFLINE_CAP		BIT(3)
#define I3C_BCR_IBI_PAYLOAD		BIT(2)
#define I3C_BCR_IBI_REQ_CAP		BIT(1)
#define I3C_BCR_MAX_DATA_SPEED_LIM	BIT(0)


struct i3c_device_info {
	u64 pid;
	u8 bcr;
	u8 dcr;
	u8 static_addr;
	u8 dyn_addr;
	u8 hdr_cap;
	u8 max_read_ds;
	u8 max_write_ds;
	u8 max_ibi_len;
	u32 max_read_turnaround;
	u16 max_read_len;
	u16 max_write_len;
};


struct i3c_device;


#define I3C_MATCH_MANUF_AND_PART (I3C_MATCH_MANUF | I3C_MATCH_PART)

#define I3C_DEVICE(_manufid, _partid, _drvdata)				\
	{								\
		.match_flags = I3C_MATCH_MANUF_AND_PART,		\
		.manuf_id = _manufid,					\
		.part_id = _partid,					\
		.data = _drvdata,					\
	}

#define I3C_DEVICE_EXTRA_INFO(_manufid, _partid, _info, _drvdata)	\
	{								\
		.match_flags = I3C_MATCH_MANUF_AND_PART |		\
			       I3C_MATCH_EXTRA_INFO,			\
		.manuf_id = _manufid,					\
		.part_id = _partid,					\
		.extra_info = _info,					\
		.data = _drvdata,					\
	}

#define I3C_CLASS(_dcr, _drvdata)					\
	{								\
		.match_flags = I3C_MATCH_DCR,				\
		.dcr = _dcr,						\
	}


struct i3c_driver {
	struct device_driver driver;
	int (*probe)(struct i3c_device *dev);
	void (*remove)(struct i3c_device *dev);
	const struct i3c_device_id *id_table;
};

static inline struct i3c_driver *drv_to_i3cdrv(struct device_driver *drv)
{
	return container_of(drv, struct i3c_driver, driver);
}

struct device *i3cdev_to_dev(struct i3c_device *i3cdev);


#define dev_to_i3cdev(__dev)	container_of_const(__dev, struct i3c_device, dev)

const struct i3c_device_id *
i3c_device_match_id(struct i3c_device *i3cdev,
		    const struct i3c_device_id *id_table);

static inline void i3cdev_set_drvdata(struct i3c_device *i3cdev,
				      void *data)
{
	struct device *dev = i3cdev_to_dev(i3cdev);

	dev_set_drvdata(dev, data);
}

static inline void *i3cdev_get_drvdata(struct i3c_device *i3cdev)
{
	struct device *dev = i3cdev_to_dev(i3cdev);

	return dev_get_drvdata(dev);
}

int i3c_driver_register_with_owner(struct i3c_driver *drv,
				   struct module *owner);
void i3c_driver_unregister(struct i3c_driver *drv);

#define i3c_driver_register(__drv)		\
	i3c_driver_register_with_owner(__drv, THIS_MODULE)


#define module_i3c_driver(__drv)		\
	module_driver(__drv, i3c_driver_register, i3c_driver_unregister)


static inline int i3c_i2c_driver_register(struct i3c_driver *i3cdrv,
					  struct i2c_driver *i2cdrv)
{
	int ret;

	ret = i2c_add_driver(i2cdrv);
	if (ret || !IS_ENABLED(CONFIG_I3C))
		return ret;

	ret = i3c_driver_register(i3cdrv);
	if (ret)
		i2c_del_driver(i2cdrv);

	return ret;
}


static inline void i3c_i2c_driver_unregister(struct i3c_driver *i3cdrv,
					     struct i2c_driver *i2cdrv)
{
	if (IS_ENABLED(CONFIG_I3C))
		i3c_driver_unregister(i3cdrv);

	i2c_del_driver(i2cdrv);
}


#define module_i3c_i2c_driver(__i3cdrv, __i2cdrv)	\
	module_driver(__i3cdrv,				\
		      i3c_i2c_driver_register,		\
		      i3c_i2c_driver_unregister,	\
		      __i2cdrv)

int i3c_device_do_priv_xfers(struct i3c_device *dev,
			     struct i3c_priv_xfer *xfers,
			     int nxfers);

int i3c_device_do_setdasa(struct i3c_device *dev);

void i3c_device_get_info(const struct i3c_device *dev, struct i3c_device_info *info);

struct i3c_ibi_payload {
	unsigned int len;
	const void *data;
};


struct i3c_ibi_setup {
	unsigned int max_payload_len;
	unsigned int num_slots;
	void (*handler)(struct i3c_device *dev,
			const struct i3c_ibi_payload *payload);
};

int i3c_device_request_ibi(struct i3c_device *dev,
			   const struct i3c_ibi_setup *setup);
void i3c_device_free_ibi(struct i3c_device *dev);
int i3c_device_enable_ibi(struct i3c_device *dev);
int i3c_device_disable_ibi(struct i3c_device *dev);

#endif 
