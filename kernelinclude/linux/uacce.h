/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LINUX_UACCE_H
#define _LINUX_UACCE_H

#include <linux/cdev.h>
#include <uapi/misc/uacce/uacce.h>

#define UACCE_NAME		"uacce"
#define UACCE_MAX_REGION	2
#define UACCE_MAX_NAME_SIZE	64
#define UACCE_MAX_ERR_THRESHOLD	65535

struct uacce_queue;
struct uacce_device;


struct uacce_qfile_region {
	enum uacce_qfrt type;
};


struct uacce_ops {
	int (*get_available_instances)(struct uacce_device *uacce);
	int (*get_queue)(struct uacce_device *uacce, unsigned long arg,
			 struct uacce_queue *q);
	void (*put_queue)(struct uacce_queue *q);
	int (*start_queue)(struct uacce_queue *q);
	void (*stop_queue)(struct uacce_queue *q);
	int (*is_q_updated)(struct uacce_queue *q);
	int (*mmap)(struct uacce_queue *q, struct vm_area_struct *vma,
		    struct uacce_qfile_region *qfr);
	long (*ioctl)(struct uacce_queue *q, unsigned int cmd,
		      unsigned long arg);
	enum uacce_dev_state (*get_isolate_state)(struct uacce_device *uacce);
	int (*isolate_err_threshold_write)(struct uacce_device *uacce, u32 num);
	u32 (*isolate_err_threshold_read)(struct uacce_device *uacce);
};


struct uacce_interface {
	char name[UACCE_MAX_NAME_SIZE];
	unsigned int flags;
	const struct uacce_ops *ops;
};

enum uacce_dev_state {
	UACCE_DEV_NORMAL,
	UACCE_DEV_ISOLATE,
};

enum uacce_q_state {
	UACCE_Q_ZOMBIE = 0,
	UACCE_Q_INIT,
	UACCE_Q_STARTED,
};


struct uacce_queue {
	struct uacce_device *uacce;
	void *priv;
	wait_queue_head_t wait;
	struct list_head list;
	struct uacce_qfile_region *qfrs[UACCE_MAX_REGION];
	struct mutex mutex;
	enum uacce_q_state state;
	u32 pasid;
	struct iommu_sva *handle;
	struct address_space *mapping;
};


struct uacce_device {
	const char *algs;
	const char *api_ver;
	const struct uacce_ops *ops;
	unsigned long qf_pg_num[UACCE_MAX_REGION];
	struct device *parent;
	bool is_vf;
	u32 flags;
	u32 dev_id;
	struct cdev *cdev;
	struct device dev;
	struct mutex mutex;
	void *priv;
	struct list_head queues;
};

#if IS_ENABLED(CONFIG_UACCE)

struct uacce_device *uacce_alloc(struct device *parent,
				 struct uacce_interface *interface);
int uacce_register(struct uacce_device *uacce);
void uacce_remove(struct uacce_device *uacce);

#else 

static inline
struct uacce_device *uacce_alloc(struct device *parent,
				 struct uacce_interface *interface)
{
	return ERR_PTR(-ENODEV);
}

static inline int uacce_register(struct uacce_device *uacce)
{
	return -EINVAL;
}

static inline void uacce_remove(struct uacce_device *uacce) {}

#endif 

#endif 
