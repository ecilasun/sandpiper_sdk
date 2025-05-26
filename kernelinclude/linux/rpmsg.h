/* SPDX-License-Identifier: BSD-3-Clause */


#ifndef _LINUX_RPMSG_H
#define _LINUX_RPMSG_H

#include <linux/types.h>
#include <linux/device.h>
#include <linux/err.h>
#include <linux/mod_devicetable.h>
#include <linux/kref.h>
#include <linux/mutex.h>
#include <linux/poll.h>
#include <linux/rpmsg/byteorder.h>
#include <uapi/linux/rpmsg.h>

struct rpmsg_device;
struct rpmsg_endpoint;
struct rpmsg_device_ops;
struct rpmsg_endpoint_ops;


struct rpmsg_channel_info {
	char name[RPMSG_NAME_SIZE];
	u32 src;
	u32 dst;
};


struct rpmsg_device {
	struct device dev;
	struct rpmsg_device_id id;
	const char *driver_override;
	u32 src;
	u32 dst;
	struct rpmsg_endpoint *ept;
	bool announce;
	bool little_endian;

	const struct rpmsg_device_ops *ops;
};

typedef int (*rpmsg_rx_cb_t)(struct rpmsg_device *, void *, int, void *, u32);
typedef int (*rpmsg_flowcontrol_cb_t)(struct rpmsg_device *, void *, bool);


struct rpmsg_endpoint {
	struct rpmsg_device *rpdev;
	struct kref refcount;
	rpmsg_rx_cb_t cb;
	rpmsg_flowcontrol_cb_t flow_cb;
	struct mutex cb_lock;
	u32 addr;
	void *priv;

	const struct rpmsg_endpoint_ops *ops;
};


struct rpmsg_driver {
	struct device_driver drv;
	const struct rpmsg_device_id *id_table;
	int (*probe)(struct rpmsg_device *dev);
	void (*remove)(struct rpmsg_device *dev);
	int (*callback)(struct rpmsg_device *, void *, int, void *, u32);
	int (*flowcontrol)(struct rpmsg_device *, void *, bool);
};

static inline u16 rpmsg16_to_cpu(struct rpmsg_device *rpdev, __rpmsg16 val)
{
	if (!rpdev)
		return __rpmsg16_to_cpu(rpmsg_is_little_endian(), val);
	else
		return __rpmsg16_to_cpu(rpdev->little_endian, val);
}

static inline __rpmsg16 cpu_to_rpmsg16(struct rpmsg_device *rpdev, u16 val)
{
	if (!rpdev)
		return __cpu_to_rpmsg16(rpmsg_is_little_endian(), val);
	else
		return __cpu_to_rpmsg16(rpdev->little_endian, val);
}

static inline u32 rpmsg32_to_cpu(struct rpmsg_device *rpdev, __rpmsg32 val)
{
	if (!rpdev)
		return __rpmsg32_to_cpu(rpmsg_is_little_endian(), val);
	else
		return __rpmsg32_to_cpu(rpdev->little_endian, val);
}

static inline __rpmsg32 cpu_to_rpmsg32(struct rpmsg_device *rpdev, u32 val)
{
	if (!rpdev)
		return __cpu_to_rpmsg32(rpmsg_is_little_endian(), val);
	else
		return __cpu_to_rpmsg32(rpdev->little_endian, val);
}

static inline u64 rpmsg64_to_cpu(struct rpmsg_device *rpdev, __rpmsg64 val)
{
	if (!rpdev)
		return __rpmsg64_to_cpu(rpmsg_is_little_endian(), val);
	else
		return __rpmsg64_to_cpu(rpdev->little_endian, val);
}

static inline __rpmsg64 cpu_to_rpmsg64(struct rpmsg_device *rpdev, u64 val)
{
	if (!rpdev)
		return __cpu_to_rpmsg64(rpmsg_is_little_endian(), val);
	else
		return __cpu_to_rpmsg64(rpdev->little_endian, val);
}

#if IS_ENABLED(CONFIG_RPMSG)

int rpmsg_register_device_override(struct rpmsg_device *rpdev,
				   const char *driver_override);
int rpmsg_register_device(struct rpmsg_device *rpdev);
int rpmsg_unregister_device(struct device *parent,
			    struct rpmsg_channel_info *chinfo);
int __register_rpmsg_driver(struct rpmsg_driver *drv, struct module *owner);
void unregister_rpmsg_driver(struct rpmsg_driver *drv);
void rpmsg_destroy_ept(struct rpmsg_endpoint *);
struct rpmsg_endpoint *rpmsg_create_ept(struct rpmsg_device *,
					rpmsg_rx_cb_t cb, void *priv,
					struct rpmsg_channel_info chinfo);

int rpmsg_send(struct rpmsg_endpoint *ept, void *data, int len);
int rpmsg_sendto(struct rpmsg_endpoint *ept, void *data, int len, u32 dst);
int rpmsg_send_offchannel(struct rpmsg_endpoint *ept, u32 src, u32 dst,
			  void *data, int len);

int rpmsg_trysend(struct rpmsg_endpoint *ept, void *data, int len);
int rpmsg_trysendto(struct rpmsg_endpoint *ept, void *data, int len, u32 dst);
int rpmsg_trysend_offchannel(struct rpmsg_endpoint *ept, u32 src, u32 dst,
			     void *data, int len);

__poll_t rpmsg_poll(struct rpmsg_endpoint *ept, struct file *filp,
			poll_table *wait);

ssize_t rpmsg_get_mtu(struct rpmsg_endpoint *ept);

int rpmsg_set_flow_control(struct rpmsg_endpoint *ept, bool pause, u32 dst);

#else

static inline int rpmsg_register_device_override(struct rpmsg_device *rpdev,
						 const char *driver_override)
{
	return -ENXIO;
}

static inline int rpmsg_register_device(struct rpmsg_device *rpdev)
{
	return -ENXIO;
}

static inline int rpmsg_unregister_device(struct device *parent,
					  struct rpmsg_channel_info *chinfo)
{
	
	WARN_ON(1);

	return -ENXIO;
}

static inline int __register_rpmsg_driver(struct rpmsg_driver *drv,
					  struct module *owner)
{
	
	WARN_ON(1);

	return -ENXIO;
}

static inline void unregister_rpmsg_driver(struct rpmsg_driver *drv)
{
	
	WARN_ON(1);
}

static inline void rpmsg_destroy_ept(struct rpmsg_endpoint *ept)
{
	
	WARN_ON(1);
}

static inline struct rpmsg_endpoint *rpmsg_create_ept(struct rpmsg_device *rpdev,
						      rpmsg_rx_cb_t cb,
						      void *priv,
						      struct rpmsg_channel_info chinfo)
{
	
	WARN_ON(1);

	return NULL;
}

static inline int rpmsg_send(struct rpmsg_endpoint *ept, void *data, int len)
{
	
	WARN_ON(1);

	return -ENXIO;
}

static inline int rpmsg_sendto(struct rpmsg_endpoint *ept, void *data, int len,
			       u32 dst)
{
	
	WARN_ON(1);

	return -ENXIO;

}

static inline int rpmsg_send_offchannel(struct rpmsg_endpoint *ept, u32 src,
					u32 dst, void *data, int len)
{
	
	WARN_ON(1);

	return -ENXIO;
}

static inline int rpmsg_trysend(struct rpmsg_endpoint *ept, void *data, int len)
{
	
	WARN_ON(1);

	return -ENXIO;
}

static inline int rpmsg_trysendto(struct rpmsg_endpoint *ept, void *data,
				  int len, u32 dst)
{
	
	WARN_ON(1);

	return -ENXIO;
}

static inline int rpmsg_trysend_offchannel(struct rpmsg_endpoint *ept, u32 src,
					   u32 dst, void *data, int len)
{
	
	WARN_ON(1);

	return -ENXIO;
}

static inline __poll_t rpmsg_poll(struct rpmsg_endpoint *ept,
				      struct file *filp, poll_table *wait)
{
	
	WARN_ON(1);

	return 0;
}

static inline ssize_t rpmsg_get_mtu(struct rpmsg_endpoint *ept)
{
	
	WARN_ON(1);

	return -ENXIO;
}

static inline int rpmsg_set_flow_control(struct rpmsg_endpoint *ept, bool pause, u32 dst)
{
	
	WARN_ON(1);

	return -ENXIO;
}

#endif 


#define register_rpmsg_driver(drv) \
	__register_rpmsg_driver(drv, THIS_MODULE)


#define module_rpmsg_driver(__rpmsg_driver) \
	module_driver(__rpmsg_driver, register_rpmsg_driver, \
			unregister_rpmsg_driver)

#endif 
