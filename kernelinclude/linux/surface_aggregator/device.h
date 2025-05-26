/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef _LINUX_SURFACE_AGGREGATOR_DEVICE_H
#define _LINUX_SURFACE_AGGREGATOR_DEVICE_H

#include <linux/device.h>
#include <linux/mod_devicetable.h>
#include <linux/property.h>
#include <linux/types.h>

#include <linux/surface_aggregator/controller.h>





enum ssam_device_domain {
	SSAM_DOMAIN_VIRTUAL   = 0x00,
	SSAM_DOMAIN_SERIALHUB = 0x01,
};


enum ssam_virtual_tc {
	SSAM_VIRTUAL_TC_HUB = 0x00,
};


struct ssam_device_uid {
	u8 domain;
	u8 category;
	u8 target;
	u8 instance;
	u8 function;
};


#define SSAM_SSH_TID_ANY	0xffff
#define SSAM_SSH_IID_ANY	0xffff
#define SSAM_SSH_FUN_ANY	0xffff


#define SSAM_DEVICE(d, cat, tid, iid, fun)					\
	.match_flags = (((tid) != SSAM_SSH_TID_ANY) ? SSAM_MATCH_TARGET : 0)	\
		     | (((iid) != SSAM_SSH_IID_ANY) ? SSAM_MATCH_INSTANCE : 0)	\
		     | (((fun) != SSAM_SSH_FUN_ANY) ? SSAM_MATCH_FUNCTION : 0),	\
	.domain   = d,								\
	.category = cat,							\
	.target   = __builtin_choose_expr((tid) != SSAM_SSH_TID_ANY, (tid), 0),	\
	.instance = __builtin_choose_expr((iid) != SSAM_SSH_IID_ANY, (iid), 0),	\
	.function = __builtin_choose_expr((fun) != SSAM_SSH_FUN_ANY, (fun), 0)


#define SSAM_VDEV(cat, tid, iid, fun) \
	SSAM_DEVICE(SSAM_DOMAIN_VIRTUAL, SSAM_VIRTUAL_TC_##cat, SSAM_SSH_TID_##tid, iid, fun)


#define SSAM_SDEV(cat, tid, iid, fun) \
	SSAM_DEVICE(SSAM_DOMAIN_SERIALHUB, SSAM_SSH_TC_##cat, SSAM_SSH_TID_##tid, iid, fun)


enum ssam_device_flags {
	SSAM_DEVICE_HOT_REMOVED_BIT = 0,
};


struct ssam_device {
	struct device dev;
	struct ssam_controller *ctrl;

	struct ssam_device_uid uid;

	unsigned long flags;
};


struct ssam_device_driver {
	struct device_driver driver;

	const struct ssam_device_id *match_table;

	int  (*probe)(struct ssam_device *sdev);
	void (*remove)(struct ssam_device *sdev);
};

#ifdef CONFIG_SURFACE_AGGREGATOR_BUS

extern struct bus_type ssam_bus_type;
extern const struct device_type ssam_device_type;


static inline bool is_ssam_device(struct device *d)
{
	return d->type == &ssam_device_type;
}

#else 

static inline bool is_ssam_device(struct device *d)
{
	return false;
}

#endif 


#define to_ssam_device(d)	container_of_const(d, struct ssam_device, dev)


#define to_ssam_device_driver(d)	container_of_const(d, struct ssam_device_driver, driver)

const struct ssam_device_id *ssam_device_id_match(const struct ssam_device_id *table,
						  const struct ssam_device_uid uid);

const struct ssam_device_id *ssam_device_get_match(const struct ssam_device *dev);

const void *ssam_device_get_match_data(const struct ssam_device *dev);

struct ssam_device *ssam_device_alloc(struct ssam_controller *ctrl,
				      struct ssam_device_uid uid);

int ssam_device_add(struct ssam_device *sdev);
void ssam_device_remove(struct ssam_device *sdev);


static inline void ssam_device_mark_hot_removed(struct ssam_device *sdev)
{
	dev_dbg(&sdev->dev, "marking device as hot-removed\n");
	set_bit(SSAM_DEVICE_HOT_REMOVED_BIT, &sdev->flags);
}


static inline bool ssam_device_is_hot_removed(struct ssam_device *sdev)
{
	return test_bit(SSAM_DEVICE_HOT_REMOVED_BIT, &sdev->flags);
}


static inline struct ssam_device *ssam_device_get(struct ssam_device *sdev)
{
	return sdev ? to_ssam_device(get_device(&sdev->dev)) : NULL;
}


static inline void ssam_device_put(struct ssam_device *sdev)
{
	if (sdev)
		put_device(&sdev->dev);
}


static inline void *ssam_device_get_drvdata(struct ssam_device *sdev)
{
	return dev_get_drvdata(&sdev->dev);
}


static inline void ssam_device_set_drvdata(struct ssam_device *sdev, void *data)
{
	dev_set_drvdata(&sdev->dev, data);
}

int __ssam_device_driver_register(struct ssam_device_driver *d, struct module *o);
void ssam_device_driver_unregister(struct ssam_device_driver *d);


#define ssam_device_driver_register(drv) \
	__ssam_device_driver_register(drv, THIS_MODULE)


#define module_ssam_device_driver(drv)			\
	module_driver(drv, ssam_device_driver_register,	\
		      ssam_device_driver_unregister)




#ifdef CONFIG_SURFACE_AGGREGATOR_BUS

int __ssam_register_clients(struct device *parent, struct ssam_controller *ctrl,
			    struct fwnode_handle *node);
void ssam_remove_clients(struct device *dev);

#else 

static inline int __ssam_register_clients(struct device *parent, struct ssam_controller *ctrl,
					  struct fwnode_handle *node)
{
	return 0;
}

static inline void ssam_remove_clients(struct device *dev) {}

#endif 


static inline int ssam_register_clients(struct device *dev, struct ssam_controller *ctrl)
{
	return __ssam_register_clients(dev, ctrl, dev_fwnode(dev));
}


static inline int ssam_device_register_clients(struct ssam_device *sdev)
{
	return ssam_register_clients(&sdev->dev, sdev->ctrl);
}





#define SSAM_DEFINE_SYNC_REQUEST_CL_N(name, spec...)			\
	SSAM_DEFINE_SYNC_REQUEST_MD_N(__raw_##name, spec)		\
	static int name(struct ssam_device *sdev)			\
	{								\
		return __raw_##name(sdev->ctrl, sdev->uid.target,	\
				    sdev->uid.instance);		\
	}


#define SSAM_DEFINE_SYNC_REQUEST_CL_W(name, atype, spec...)		\
	SSAM_DEFINE_SYNC_REQUEST_MD_W(__raw_##name, atype, spec)	\
	static int name(struct ssam_device *sdev, const atype *arg)	\
	{								\
		return __raw_##name(sdev->ctrl, sdev->uid.target,	\
				    sdev->uid.instance, arg);		\
	}


#define SSAM_DEFINE_SYNC_REQUEST_CL_R(name, rtype, spec...)		\
	SSAM_DEFINE_SYNC_REQUEST_MD_R(__raw_##name, rtype, spec)	\
	static int name(struct ssam_device *sdev, rtype *ret)		\
	{								\
		return __raw_##name(sdev->ctrl, sdev->uid.target,	\
				    sdev->uid.instance, ret);		\
	}


#define SSAM_DEFINE_SYNC_REQUEST_CL_WR(name, atype, rtype, spec...)		\
	SSAM_DEFINE_SYNC_REQUEST_MD_WR(__raw_##name, atype, rtype, spec)	\
	static int name(struct ssam_device *sdev, const atype *arg, rtype *ret)	\
	{									\
		return __raw_##name(sdev->ctrl, sdev->uid.target,		\
				    sdev->uid.instance, arg, ret);		\
	}





static inline int ssam_device_notifier_register(struct ssam_device *sdev,
						struct ssam_event_notifier *n)
{
	
	if (ssam_device_is_hot_removed(sdev))
		return -ENODEV;

	return ssam_notifier_register(sdev->ctrl, n);
}


static inline int ssam_device_notifier_unregister(struct ssam_device *sdev,
						  struct ssam_event_notifier *n)
{
	return __ssam_notifier_unregister(sdev->ctrl, n,
					  !ssam_device_is_hot_removed(sdev));
}

#endif 
