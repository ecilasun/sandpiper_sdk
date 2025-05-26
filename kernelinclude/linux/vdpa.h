/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_VDPA_H
#define _LINUX_VDPA_H

#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/interrupt.h>
#include <linux/vhost_iotlb.h>
#include <linux/virtio_net.h>
#include <linux/if_ether.h>


struct vdpa_callback {
	irqreturn_t (*callback)(void *data);
	void *private;
	struct eventfd_ctx *trigger;
};


struct vdpa_notification_area {
	resource_size_t addr;
	resource_size_t size;
};


struct vdpa_vq_state_split {
	u16	avail_index;
};


struct vdpa_vq_state_packed {
	u16	last_avail_counter:1;
	u16	last_avail_idx:15;
	u16	last_used_counter:1;
	u16	last_used_idx:15;
};

struct vdpa_vq_state {
	union {
		struct vdpa_vq_state_split split;
		struct vdpa_vq_state_packed packed;
	};
};

struct vdpa_mgmt_dev;


struct vdpa_device {
	struct device dev;
	struct device *dma_dev;
	const char *driver_override;
	const struct vdpa_config_ops *config;
	struct rw_semaphore cf_lock; 
	unsigned int index;
	bool features_valid;
	bool use_va;
	u32 nvqs;
	struct vdpa_mgmt_dev *mdev;
	unsigned int ngroups;
	unsigned int nas;
};


struct vdpa_iova_range {
	u64 first;
	u64 last;
};

struct vdpa_dev_set_config {
	u64 device_features;
	struct {
		u8 mac[ETH_ALEN];
		u16 mtu;
		u16 max_vq_pairs;
	} net;
	u64 mask;
};


struct vdpa_map_file {
	struct file *file;
	u64 offset;
};


struct vdpa_config_ops {
	
	int (*set_vq_address)(struct vdpa_device *vdev,
			      u16 idx, u64 desc_area, u64 driver_area,
			      u64 device_area);
	void (*set_vq_num)(struct vdpa_device *vdev, u16 idx, u32 num);
	void (*kick_vq)(struct vdpa_device *vdev, u16 idx);
	void (*kick_vq_with_data)(struct vdpa_device *vdev, u32 data);
	void (*set_vq_cb)(struct vdpa_device *vdev, u16 idx,
			  struct vdpa_callback *cb);
	void (*set_vq_ready)(struct vdpa_device *vdev, u16 idx, bool ready);
	bool (*get_vq_ready)(struct vdpa_device *vdev, u16 idx);
	int (*set_vq_state)(struct vdpa_device *vdev, u16 idx,
			    const struct vdpa_vq_state *state);
	int (*get_vq_state)(struct vdpa_device *vdev, u16 idx,
			    struct vdpa_vq_state *state);
	int (*get_vendor_vq_stats)(struct vdpa_device *vdev, u16 idx,
				   struct sk_buff *msg,
				   struct netlink_ext_ack *extack);
	struct vdpa_notification_area
	(*get_vq_notification)(struct vdpa_device *vdev, u16 idx);
	
	int (*get_vq_irq)(struct vdpa_device *vdev, u16 idx);

	
	u32 (*get_vq_align)(struct vdpa_device *vdev);
	u32 (*get_vq_group)(struct vdpa_device *vdev, u16 idx);
	u64 (*get_device_features)(struct vdpa_device *vdev);
	u64 (*get_backend_features)(const struct vdpa_device *vdev);
	int (*set_driver_features)(struct vdpa_device *vdev, u64 features);
	u64 (*get_driver_features)(struct vdpa_device *vdev);
	void (*set_config_cb)(struct vdpa_device *vdev,
			      struct vdpa_callback *cb);
	u16 (*get_vq_num_max)(struct vdpa_device *vdev);
	u16 (*get_vq_num_min)(struct vdpa_device *vdev);
	u32 (*get_device_id)(struct vdpa_device *vdev);
	u32 (*get_vendor_id)(struct vdpa_device *vdev);
	u8 (*get_status)(struct vdpa_device *vdev);
	void (*set_status)(struct vdpa_device *vdev, u8 status);
	int (*reset)(struct vdpa_device *vdev);
	int (*suspend)(struct vdpa_device *vdev);
	int (*resume)(struct vdpa_device *vdev);
	size_t (*get_config_size)(struct vdpa_device *vdev);
	void (*get_config)(struct vdpa_device *vdev, unsigned int offset,
			   void *buf, unsigned int len);
	void (*set_config)(struct vdpa_device *vdev, unsigned int offset,
			   const void *buf, unsigned int len);
	u32 (*get_generation)(struct vdpa_device *vdev);
	struct vdpa_iova_range (*get_iova_range)(struct vdpa_device *vdev);
	int (*set_vq_affinity)(struct vdpa_device *vdev, u16 idx,
			       const struct cpumask *cpu_mask);
	const struct cpumask *(*get_vq_affinity)(struct vdpa_device *vdev,
						 u16 idx);

	
	int (*set_map)(struct vdpa_device *vdev, unsigned int asid,
		       struct vhost_iotlb *iotlb);
	int (*dma_map)(struct vdpa_device *vdev, unsigned int asid,
		       u64 iova, u64 size, u64 pa, u32 perm, void *opaque);
	int (*dma_unmap)(struct vdpa_device *vdev, unsigned int asid,
			 u64 iova, u64 size);
	int (*set_group_asid)(struct vdpa_device *vdev, unsigned int group,
			      unsigned int asid);
	struct device *(*get_vq_dma_dev)(struct vdpa_device *vdev, u16 idx);
	int (*bind_mm)(struct vdpa_device *vdev, struct mm_struct *mm);
	void (*unbind_mm)(struct vdpa_device *vdev);

	
	void (*free)(struct vdpa_device *vdev);
};

struct vdpa_device *__vdpa_alloc_device(struct device *parent,
					const struct vdpa_config_ops *config,
					unsigned int ngroups, unsigned int nas,
					size_t size, const char *name,
					bool use_va);


#define vdpa_alloc_device(dev_struct, member, parent, config, ngroups, nas, \
			  name, use_va) \
			  container_of((__vdpa_alloc_device( \
				       parent, config, ngroups, nas, \
				       (sizeof(dev_struct) + \
				       BUILD_BUG_ON_ZERO(offsetof( \
				       dev_struct, member))), name, use_va)), \
				       dev_struct, member)

int vdpa_register_device(struct vdpa_device *vdev, u32 nvqs);
void vdpa_unregister_device(struct vdpa_device *vdev);

int _vdpa_register_device(struct vdpa_device *vdev, u32 nvqs);
void _vdpa_unregister_device(struct vdpa_device *vdev);


struct vdpa_driver {
	struct device_driver driver;
	int (*probe)(struct vdpa_device *vdev);
	void (*remove)(struct vdpa_device *vdev);
};

#define vdpa_register_driver(drv) \
	__vdpa_register_driver(drv, THIS_MODULE)
int __vdpa_register_driver(struct vdpa_driver *drv, struct module *owner);
void vdpa_unregister_driver(struct vdpa_driver *drv);

#define module_vdpa_driver(__vdpa_driver) \
	module_driver(__vdpa_driver, vdpa_register_driver,	\
		      vdpa_unregister_driver)

static inline struct vdpa_driver *drv_to_vdpa(struct device_driver *driver)
{
	return container_of(driver, struct vdpa_driver, driver);
}

static inline struct vdpa_device *dev_to_vdpa(struct device *_dev)
{
	return container_of(_dev, struct vdpa_device, dev);
}

static inline void *vdpa_get_drvdata(const struct vdpa_device *vdev)
{
	return dev_get_drvdata(&vdev->dev);
}

static inline void vdpa_set_drvdata(struct vdpa_device *vdev, void *data)
{
	dev_set_drvdata(&vdev->dev, data);
}

static inline struct device *vdpa_get_dma_dev(struct vdpa_device *vdev)
{
	return vdev->dma_dev;
}

static inline int vdpa_reset(struct vdpa_device *vdev)
{
	const struct vdpa_config_ops *ops = vdev->config;
	int ret;

	down_write(&vdev->cf_lock);
	vdev->features_valid = false;
	ret = ops->reset(vdev);
	up_write(&vdev->cf_lock);
	return ret;
}

static inline int vdpa_set_features_unlocked(struct vdpa_device *vdev, u64 features)
{
	const struct vdpa_config_ops *ops = vdev->config;
	int ret;

	vdev->features_valid = true;
	ret = ops->set_driver_features(vdev, features);

	return ret;
}

static inline int vdpa_set_features(struct vdpa_device *vdev, u64 features)
{
	int ret;

	down_write(&vdev->cf_lock);
	ret = vdpa_set_features_unlocked(vdev, features);
	up_write(&vdev->cf_lock);

	return ret;
}

void vdpa_get_config(struct vdpa_device *vdev, unsigned int offset,
		     void *buf, unsigned int len);
void vdpa_set_config(struct vdpa_device *dev, unsigned int offset,
		     const void *buf, unsigned int length);
void vdpa_set_status(struct vdpa_device *vdev, u8 status);


struct vdpa_mgmtdev_ops {
	int (*dev_add)(struct vdpa_mgmt_dev *mdev, const char *name,
		       const struct vdpa_dev_set_config *config);
	void (*dev_del)(struct vdpa_mgmt_dev *mdev, struct vdpa_device *dev);
};


struct vdpa_mgmt_dev {
	struct device *device;
	const struct vdpa_mgmtdev_ops *ops;
	struct virtio_device_id *id_table;
	u64 config_attr_mask;
	struct list_head list;
	u64 supported_features;
	u32 max_supported_vqs;
};

int vdpa_mgmtdev_register(struct vdpa_mgmt_dev *mdev);
void vdpa_mgmtdev_unregister(struct vdpa_mgmt_dev *mdev);

#endif 
