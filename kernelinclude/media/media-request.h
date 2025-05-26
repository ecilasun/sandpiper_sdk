// SPDX-License-Identifier: GPL-2.0


#ifndef MEDIA_REQUEST_H
#define MEDIA_REQUEST_H

#include <linux/list.h>
#include <linux/slab.h>
#include <linux/spinlock.h>
#include <linux/refcount.h>

#include <media/media-device.h>


enum media_request_state {
	MEDIA_REQUEST_STATE_IDLE,
	MEDIA_REQUEST_STATE_VALIDATING,
	MEDIA_REQUEST_STATE_QUEUED,
	MEDIA_REQUEST_STATE_COMPLETE,
	MEDIA_REQUEST_STATE_CLEANING,
	MEDIA_REQUEST_STATE_UPDATING,
	NR_OF_MEDIA_REQUEST_STATE,
};

struct media_request_object;


struct media_request {
	struct media_device *mdev;
	struct kref kref;
	char debug_str[TASK_COMM_LEN + 11];
	enum media_request_state state;
	unsigned int updating_count;
	unsigned int access_count;
	struct list_head objects;
	unsigned int num_incomplete_objects;
	wait_queue_head_t poll_wait;
	spinlock_t lock;
};

#ifdef CONFIG_MEDIA_CONTROLLER


static inline int __must_check
media_request_lock_for_access(struct media_request *req)
{
	unsigned long flags;
	int ret = -EBUSY;

	spin_lock_irqsave(&req->lock, flags);
	if (req->state == MEDIA_REQUEST_STATE_COMPLETE) {
		req->access_count++;
		ret = 0;
	}
	spin_unlock_irqrestore(&req->lock, flags);

	return ret;
}


static inline void media_request_unlock_for_access(struct media_request *req)
{
	unsigned long flags;

	spin_lock_irqsave(&req->lock, flags);
	if (!WARN_ON(!req->access_count))
		req->access_count--;
	spin_unlock_irqrestore(&req->lock, flags);
}


static inline int __must_check
media_request_lock_for_update(struct media_request *req)
{
	unsigned long flags;
	int ret = 0;

	spin_lock_irqsave(&req->lock, flags);
	if (req->state == MEDIA_REQUEST_STATE_IDLE ||
	    req->state == MEDIA_REQUEST_STATE_UPDATING) {
		req->state = MEDIA_REQUEST_STATE_UPDATING;
		req->updating_count++;
	} else {
		ret = -EBUSY;
	}
	spin_unlock_irqrestore(&req->lock, flags);

	return ret;
}


static inline void media_request_unlock_for_update(struct media_request *req)
{
	unsigned long flags;

	spin_lock_irqsave(&req->lock, flags);
	WARN_ON(req->updating_count <= 0);
	if (!--req->updating_count)
		req->state = MEDIA_REQUEST_STATE_IDLE;
	spin_unlock_irqrestore(&req->lock, flags);
}


static inline void media_request_get(struct media_request *req)
{
	kref_get(&req->kref);
}


void media_request_put(struct media_request *req);


struct media_request *
media_request_get_by_fd(struct media_device *mdev, int request_fd);


int media_request_alloc(struct media_device *mdev,
			int *alloc_fd);

#else

static inline void media_request_get(struct media_request *req)
{
}

static inline void media_request_put(struct media_request *req)
{
}

static inline struct media_request *
media_request_get_by_fd(struct media_device *mdev, int request_fd)
{
	return ERR_PTR(-EBADR);
}

#endif


struct media_request_object_ops {
	int (*prepare)(struct media_request_object *object);
	void (*unprepare)(struct media_request_object *object);
	void (*queue)(struct media_request_object *object);
	void (*unbind)(struct media_request_object *object);
	void (*release)(struct media_request_object *object);
};


struct media_request_object {
	const struct media_request_object_ops *ops;
	void *priv;
	struct media_request *req;
	struct list_head list;
	struct kref kref;
	bool completed;
};

#ifdef CONFIG_MEDIA_CONTROLLER


static inline void media_request_object_get(struct media_request_object *obj)
{
	kref_get(&obj->kref);
}


void media_request_object_put(struct media_request_object *obj);


struct media_request_object *
media_request_object_find(struct media_request *req,
			  const struct media_request_object_ops *ops,
			  void *priv);


void media_request_object_init(struct media_request_object *obj);


int media_request_object_bind(struct media_request *req,
			      const struct media_request_object_ops *ops,
			      void *priv, bool is_buffer,
			      struct media_request_object *obj);


void media_request_object_unbind(struct media_request_object *obj);


void media_request_object_complete(struct media_request_object *obj);

#else

static inline int __must_check
media_request_lock_for_access(struct media_request *req)
{
	return -EINVAL;
}

static inline void media_request_unlock_for_access(struct media_request *req)
{
}

static inline int __must_check
media_request_lock_for_update(struct media_request *req)
{
	return -EINVAL;
}

static inline void media_request_unlock_for_update(struct media_request *req)
{
}

static inline void media_request_object_get(struct media_request_object *obj)
{
}

static inline void media_request_object_put(struct media_request_object *obj)
{
}

static inline struct media_request_object *
media_request_object_find(struct media_request *req,
			  const struct media_request_object_ops *ops,
			  void *priv)
{
	return NULL;
}

static inline void media_request_object_init(struct media_request_object *obj)
{
	obj->ops = NULL;
	obj->req = NULL;
}

static inline int media_request_object_bind(struct media_request *req,
			       const struct media_request_object_ops *ops,
			       void *priv, bool is_buffer,
			       struct media_request_object *obj)
{
	return 0;
}

static inline void media_request_object_unbind(struct media_request_object *obj)
{
}

static inline void media_request_object_complete(struct media_request_object *obj)
{
}

#endif

#endif
