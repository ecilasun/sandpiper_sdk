/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_VIRTIO_RING_H
#define _LINUX_VIRTIO_RING_H

#include <asm/barrier.h>
#include <linux/irqreturn.h>
#include <uapi/linux/virtio_ring.h>



static inline void virtio_mb(bool weak_barriers)
{
	if (weak_barriers)
		virt_mb();
	else
		mb();
}

static inline void virtio_rmb(bool weak_barriers)
{
	if (weak_barriers)
		virt_rmb();
	else
		dma_rmb();
}

static inline void virtio_wmb(bool weak_barriers)
{
	if (weak_barriers)
		virt_wmb();
	else
		dma_wmb();
}

#define virtio_store_mb(weak_barriers, p, v) \
do { \
	if (weak_barriers) { \
		virt_store_mb(*p, v); \
	} else { \
		WRITE_ONCE(*p, v); \
		mb(); \
	} \
} while (0) \

struct virtio_device;
struct virtqueue;
struct device;


struct virtqueue *vring_create_virtqueue(unsigned int index,
					 unsigned int num,
					 unsigned int vring_align,
					 struct virtio_device *vdev,
					 bool weak_barriers,
					 bool may_reduce_num,
					 bool ctx,
					 bool (*notify)(struct virtqueue *vq),
					 void (*callback)(struct virtqueue *vq),
					 const char *name);


struct virtqueue *vring_create_virtqueue_dma(unsigned int index,
					     unsigned int num,
					     unsigned int vring_align,
					     struct virtio_device *vdev,
					     bool weak_barriers,
					     bool may_reduce_num,
					     bool ctx,
					     bool (*notify)(struct virtqueue *vq),
					     void (*callback)(struct virtqueue *vq),
					     const char *name,
					     struct device *dma_dev);


struct virtqueue *vring_new_virtqueue(unsigned int index,
				      unsigned int num,
				      unsigned int vring_align,
				      struct virtio_device *vdev,
				      bool weak_barriers,
				      bool ctx,
				      void *pages,
				      bool (*notify)(struct virtqueue *vq),
				      void (*callback)(struct virtqueue *vq),
				      const char *name);


void vring_del_virtqueue(struct virtqueue *vq);


void vring_transport_features(struct virtio_device *vdev);

irqreturn_t vring_interrupt(int irq, void *_vq);

u32 vring_notification_data(struct virtqueue *_vq);
#endif 
