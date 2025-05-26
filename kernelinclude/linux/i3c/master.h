/* SPDX-License-Identifier: GPL-2.0 */


#ifndef I3C_MASTER_H
#define I3C_MASTER_H

#include <asm/bitsperlong.h>

#include <linux/bitops.h>
#include <linux/i2c.h>
#include <linux/i3c/ccc.h>
#include <linux/i3c/device.h>
#include <linux/rwsem.h>
#include <linux/spinlock.h>
#include <linux/workqueue.h>

#define I3C_HOT_JOIN_ADDR		0x2
#define I3C_BROADCAST_ADDR		0x7e
#define I3C_MAX_ADDR			GENMASK(6, 0)

struct i2c_client;

struct i3c_master_controller;
struct i3c_bus;
struct i3c_device;


struct i3c_i2c_dev_desc {
	struct list_head node;
	struct i3c_master_controller *master;
	void *master_priv;
};

#define I3C_LVR_I2C_INDEX_MASK		GENMASK(7, 5)
#define I3C_LVR_I2C_INDEX(x)		((x) << 5)
#define I3C_LVR_I2C_FM_MODE		BIT(4)

#define I2C_MAX_ADDR			GENMASK(6, 0)


struct i2c_dev_boardinfo {
	struct list_head node;
	struct i2c_board_info base;
	u8 lvr;
};


struct i2c_dev_desc {
	struct i3c_i2c_dev_desc common;
	struct i2c_client *dev;
	u16 addr;
	u8 lvr;
};


struct i3c_ibi_slot {
	struct work_struct work;
	struct i3c_dev_desc *dev;
	unsigned int len;
	void *data;
};


struct i3c_device_ibi_info {
	struct completion all_ibis_handled;
	atomic_t pending_ibis;
	unsigned int max_payload_len;
	unsigned int num_slots;
	unsigned int enabled;
	void (*handler)(struct i3c_device *dev,
			const struct i3c_ibi_payload *payload);
};


struct i3c_dev_boardinfo {
	struct list_head node;
	u8 init_dyn_addr;
	u8 static_addr;
	u64 pid;
	struct device_node *of_node;
};


struct i3c_dev_desc {
	struct i3c_i2c_dev_desc common;
	struct i3c_device_info info;
	struct mutex ibi_lock;
	struct i3c_device_ibi_info *ibi;
	struct i3c_device *dev;
	const struct i3c_dev_boardinfo *boardinfo;
};


struct i3c_device {
	struct device dev;
	struct i3c_dev_desc *desc;
	struct i3c_bus *bus;
};


#define I3C_BUS_MAX_DEVS		11

#define I3C_BUS_MAX_I3C_SCL_RATE	12900000
#define I3C_BUS_TYP_I3C_SCL_RATE	12500000
#define I3C_BUS_I2C_FM_PLUS_SCL_RATE	1000000
#define I3C_BUS_I2C_FM_SCL_RATE		400000
#define I3C_BUS_TLOW_OD_MIN_NS		200


enum i3c_bus_mode {
	I3C_BUS_MODE_PURE,
	I3C_BUS_MODE_MIXED_FAST,
	I3C_BUS_MODE_MIXED_LIMITED,
	I3C_BUS_MODE_MIXED_SLOW,
};


enum i3c_addr_slot_status {
	I3C_ADDR_SLOT_FREE,
	I3C_ADDR_SLOT_RSVD,
	I3C_ADDR_SLOT_I2C_DEV,
	I3C_ADDR_SLOT_I3C_DEV,
	I3C_ADDR_SLOT_STATUS_MASK = 3,
};


struct i3c_bus {
	struct i3c_dev_desc *cur_master;
	int id;
	unsigned long addrslots[((I2C_MAX_ADDR + 1) * 2) / BITS_PER_LONG];
	enum i3c_bus_mode mode;
	struct {
		unsigned long i3c;
		unsigned long i2c;
	} scl_rate;
	struct {
		struct list_head i3c;
		struct list_head i2c;
	} devs;
	struct rw_semaphore lock;
};


struct i3c_master_controller_ops {
	int (*bus_init)(struct i3c_master_controller *master);
	void (*bus_cleanup)(struct i3c_master_controller *master);
	int (*attach_i3c_dev)(struct i3c_dev_desc *dev);
	int (*reattach_i3c_dev)(struct i3c_dev_desc *dev, u8 old_dyn_addr);
	void (*detach_i3c_dev)(struct i3c_dev_desc *dev);
	int (*do_daa)(struct i3c_master_controller *master);
	bool (*supports_ccc_cmd)(struct i3c_master_controller *master,
				 const struct i3c_ccc_cmd *cmd);
	int (*send_ccc_cmd)(struct i3c_master_controller *master,
			    struct i3c_ccc_cmd *cmd);
	int (*priv_xfers)(struct i3c_dev_desc *dev,
			  struct i3c_priv_xfer *xfers,
			  int nxfers);
	int (*attach_i2c_dev)(struct i2c_dev_desc *dev);
	void (*detach_i2c_dev)(struct i2c_dev_desc *dev);
	int (*i2c_xfers)(struct i2c_dev_desc *dev,
			 const struct i2c_msg *xfers, int nxfers);
	int (*request_ibi)(struct i3c_dev_desc *dev,
			   const struct i3c_ibi_setup *req);
	void (*free_ibi)(struct i3c_dev_desc *dev);
	int (*enable_ibi)(struct i3c_dev_desc *dev);
	int (*disable_ibi)(struct i3c_dev_desc *dev);
	void (*recycle_ibi_slot)(struct i3c_dev_desc *dev,
				 struct i3c_ibi_slot *slot);
};


struct i3c_master_controller {
	struct device dev;
	struct i3c_dev_desc *this;
	struct i2c_adapter i2c;
	const struct i3c_master_controller_ops *ops;
	unsigned int secondary : 1;
	unsigned int init_done : 1;
	struct {
		struct list_head i3c;
		struct list_head i2c;
	} boardinfo;
	struct i3c_bus bus;
	struct workqueue_struct *wq;
};


#define i3c_bus_for_each_i2cdev(bus, dev)				\
	list_for_each_entry(dev, &(bus)->devs.i2c, common.node)


#define i3c_bus_for_each_i3cdev(bus, dev)				\
	list_for_each_entry(dev, &(bus)->devs.i3c, common.node)

int i3c_master_do_i2c_xfers(struct i3c_master_controller *master,
			    const struct i2c_msg *xfers,
			    int nxfers);

int i3c_master_disec_locked(struct i3c_master_controller *master, u8 addr,
			    u8 evts);
int i3c_master_enec_locked(struct i3c_master_controller *master, u8 addr,
			   u8 evts);
int i3c_master_entdaa_locked(struct i3c_master_controller *master);
int i3c_master_defslvs_locked(struct i3c_master_controller *master);

int i3c_master_get_free_addr(struct i3c_master_controller *master,
			     u8 start_addr);

int i3c_master_add_i3c_dev_locked(struct i3c_master_controller *master,
				  u8 addr);
int i3c_master_do_daa(struct i3c_master_controller *master);

int i3c_master_set_info(struct i3c_master_controller *master,
			const struct i3c_device_info *info);

int i3c_master_register(struct i3c_master_controller *master,
			struct device *parent,
			const struct i3c_master_controller_ops *ops,
			bool secondary);
void i3c_master_unregister(struct i3c_master_controller *master);


static inline void *i3c_dev_get_master_data(const struct i3c_dev_desc *dev)
{
	return dev->common.master_priv;
}


static inline void i3c_dev_set_master_data(struct i3c_dev_desc *dev,
					   void *data)
{
	dev->common.master_priv = data;
}


static inline void *i2c_dev_get_master_data(const struct i2c_dev_desc *dev)
{
	return dev->common.master_priv;
}


static inline void i2c_dev_set_master_data(struct i2c_dev_desc *dev,
					   void *data)
{
	dev->common.master_priv = data;
}


static inline struct i3c_master_controller *
i3c_dev_get_master(struct i3c_dev_desc *dev)
{
	return dev->common.master;
}


static inline struct i3c_master_controller *
i2c_dev_get_master(struct i2c_dev_desc *dev)
{
	return dev->common.master;
}


static inline struct i3c_bus *
i3c_master_get_bus(struct i3c_master_controller *master)
{
	return &master->bus;
}

struct i3c_generic_ibi_pool;

struct i3c_generic_ibi_pool *
i3c_generic_ibi_alloc_pool(struct i3c_dev_desc *dev,
			   const struct i3c_ibi_setup *req);
void i3c_generic_ibi_free_pool(struct i3c_generic_ibi_pool *pool);

struct i3c_ibi_slot *
i3c_generic_ibi_get_free_slot(struct i3c_generic_ibi_pool *pool);
void i3c_generic_ibi_recycle_slot(struct i3c_generic_ibi_pool *pool,
				  struct i3c_ibi_slot *slot);

void i3c_master_queue_ibi(struct i3c_dev_desc *dev, struct i3c_ibi_slot *slot);

struct i3c_ibi_slot *i3c_master_get_free_ibi_slot(struct i3c_dev_desc *dev);

#endif 
