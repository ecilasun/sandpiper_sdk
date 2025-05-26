/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_VIRTIO_H
#define _LINUX_VIRTIO_H

#include <linux/types.h>
#include <linux/scatterlist.h>
#include <linux/spinlock.h>
#include <linux/device.h>
#include <linux/mod_devicetable.h>
#include <linux/gfp.h>
#include <linux/dma-mapping.h>


struct virtqueue {
	struct list_head list;
	void (*callback)(struct virtqueue *vq);
	const char *name;
	struct virtio_device *vdev;
	unsigned int index;
	unsigned int num_free;
	unsigned int num_max;
	bool reset;
	void *priv;
};

int virtqueue_add_outbuf(struct virtqueue *vq,
			 struct scatterlist sg[], unsigned int num,
			 void *data,
			 gfp_t gfp);

int virtqueue_add_inbuf(struct virtqueue *vq,
			struct scatterlist sg[], unsigned int num,
			void *data,
			gfp_t gfp);

int virtqueue_add_inbuf_ctx(struct virtqueue *vq,
			    struct scatterlist sg[], unsigned int num,
			    void *data,
			    void *ctx,
			    gfp_t gfp);

int virtqueue_add_sgs(struct virtqueue *vq,
		      struct scatterlist *sgs[],
		      unsigned int out_sgs,
		      unsigned int in_sgs,
		      void *data,
		      gfp_t gfp);

struct device *virtqueue_dma_dev(struct virtqueue *vq);

bool virtqueue_kick(struct virtqueue *vq);

bool virtqueue_kick_prepare(struct virtqueue *vq);

bool virtqueue_notify(struct virtqueue *vq);

void *virtqueue_get_buf(struct virtqueue *vq, unsigned int *len);

void *virtqueue_get_buf_ctx(struct virtqueue *vq, unsigned int *len,
			    void **ctx);

void virtqueue_disable_cb(struct virtqueue *vq);

bool virtqueue_enable_cb(struct virtqueue *vq);

unsigned virtqueue_enable_cb_prepare(struct virtqueue *vq);

int virtqueue_set_dma_premapped(struct virtqueue *_vq);

bool virtqueue_poll(struct virtqueue *vq, unsigned);

bool virtqueue_enable_cb_delayed(struct virtqueue *vq);

void *virtqueue_detach_unused_buf(struct virtqueue *vq);

unsigned int virtqueue_get_vring_size(const struct virtqueue *vq);

bool virtqueue_is_broken(const struct virtqueue *vq);

const struct vring *virtqueue_get_vring(const struct virtqueue *vq);
dma_addr_t virtqueue_get_desc_addr(const struct virtqueue *vq);
dma_addr_t virtqueue_get_avail_addr(const struct virtqueue *vq);
dma_addr_t virtqueue_get_used_addr(const struct virtqueue *vq);

int virtqueue_resize(struct virtqueue *vq, u32 num,
		     void (*recycle)(struct virtqueue *vq, void *buf));
int virtqueue_reset(struct virtqueue *vq,
		    void (*recycle)(struct virtqueue *vq, void *buf));


struct virtio_device {
	int index;
	bool failed;
	bool config_enabled;
	bool config_change_pending;
	spinlock_t config_lock;
	spinlock_t vqs_list_lock;
	struct device dev;
	struct virtio_device_id id;
	const struct virtio_config_ops *config;
	const struct vringh_config_ops *vringh_config;
	struct list_head vqs;
	u64 features;
	void *priv;
};

#define dev_to_virtio(_dev)	container_of_const(_dev, struct virtio_device, dev)

void virtio_add_status(struct virtio_device *dev, unsigned int status);
int register_virtio_device(struct virtio_device *dev);
void unregister_virtio_device(struct virtio_device *dev);
bool is_virtio_device(struct device *dev);

void virtio_break_device(struct virtio_device *dev);
void __virtio_unbreak_device(struct virtio_device *dev);

void __virtqueue_break(struct virtqueue *_vq);
void __virtqueue_unbreak(struct virtqueue *_vq);

void virtio_config_changed(struct virtio_device *dev);
#ifdef CONFIG_PM_SLEEP
int virtio_device_freeze(struct virtio_device *dev);
int virtio_device_restore(struct virtio_device *dev);
#endif
void virtio_reset_device(struct virtio_device *dev);

size_t virtio_max_dma_size(const struct virtio_device *vdev);

#define virtio_device_for_each_vq(vdev, vq) \
	list_for_each_entry(vq, &vdev->vqs, list)


struct virtio_driver {
	struct device_driver driver;
	const struct virtio_device_id *id_table;
	const unsigned int *feature_table;
	unsigned int feature_table_size;
	const unsigned int *feature_table_legacy;
	unsigned int feature_table_size_legacy;
	int (*validate)(struct virtio_device *dev);
	int (*probe)(struct virtio_device *dev);
	void (*scan)(struct virtio_device *dev);
	void (*remove)(struct virtio_device *dev);
	void (*config_changed)(struct virtio_device *dev);
	int (*freeze)(struct virtio_device *dev);
	int (*restore)(struct virtio_device *dev);
};

static inline struct virtio_driver *drv_to_virtio(struct device_driver *drv)
{
	return container_of(drv, struct virtio_driver, driver);
}

int register_virtio_driver(struct virtio_driver *drv);
void unregister_virtio_driver(struct virtio_driver *drv);


#define module_virtio_driver(__virtio_driver) \
	module_driver(__virtio_driver, register_virtio_driver, \
			unregister_virtio_driver)

dma_addr_t virtqueue_dma_map_single_attrs(struct virtqueue *_vq, void *ptr, size_t size,
					  enum dma_data_direction dir, unsigned long attrs);
void virtqueue_dma_unmap_single_attrs(struct virtqueue *_vq, dma_addr_t addr,
				      size_t size, enum dma_data_direction dir,
				      unsigned long attrs);
int virtqueue_dma_mapping_error(struct virtqueue *_vq, dma_addr_t addr);

bool virtqueue_dma_need_sync(struct virtqueue *_vq, dma_addr_t addr);
void virtqueue_dma_sync_single_range_for_cpu(struct virtqueue *_vq, dma_addr_t addr,
					     unsigned long offset, size_t size,
					     enum dma_data_direction dir);
void virtqueue_dma_sync_single_range_for_device(struct virtqueue *_vq, dma_addr_t addr,
						unsigned long offset, size_t size,
						enum dma_data_direction dir);
#endif 
