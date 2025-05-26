/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _KUNIT_RESOURCE_H
#define _KUNIT_RESOURCE_H

#include <kunit/test.h>

#include <linux/kref.h>
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/spinlock.h>

struct kunit_resource;

typedef int (*kunit_resource_init_t)(struct kunit_resource *, void *);
typedef void (*kunit_resource_free_t)(struct kunit_resource *);


struct kunit_resource {
	void *data;
	const char *name;
	kunit_resource_free_t free;

	
	struct kref refcount;
	struct list_head node;
	bool should_kfree;
};


static inline void kunit_get_resource(struct kunit_resource *res)
{
	kref_get(&res->refcount);
}


static inline void kunit_release_resource(struct kref *kref)
{
	struct kunit_resource *res = container_of(kref, struct kunit_resource,
						  refcount);

	if (res->free)
		res->free(res);

	
	if (res->should_kfree)
		kfree(res);
}


static inline void kunit_put_resource(struct kunit_resource *res)
{
	kref_put(&res->refcount, kunit_release_resource);
}


int __kunit_add_resource(struct kunit *test,
			 kunit_resource_init_t init,
			 kunit_resource_free_t free,
			 struct kunit_resource *res,
			 void *data);


static inline int kunit_add_resource(struct kunit *test,
				     kunit_resource_init_t init,
				     kunit_resource_free_t free,
				     struct kunit_resource *res,
				     void *data)
{
	res->should_kfree = false;
	return __kunit_add_resource(test, init, free, res, data);
}

static inline struct kunit_resource *
kunit_find_named_resource(struct kunit *test, const char *name);


static inline int kunit_add_named_resource(struct kunit *test,
					   kunit_resource_init_t init,
					   kunit_resource_free_t free,
					   struct kunit_resource *res,
					   const char *name,
					   void *data)
{
	struct kunit_resource *existing;

	if (!name)
		return -EINVAL;

	existing = kunit_find_named_resource(test, name);
	if (existing) {
		kunit_put_resource(existing);
		return -EEXIST;
	}

	res->name = name;
	res->should_kfree = false;

	return __kunit_add_resource(test, init, free, res, data);
}


static inline struct kunit_resource *
kunit_alloc_and_get_resource(struct kunit *test,
			     kunit_resource_init_t init,
			     kunit_resource_free_t free,
			     gfp_t internal_gfp,
			     void *context)
{
	struct kunit_resource *res;
	int ret;

	res = kzalloc(sizeof(*res), internal_gfp);
	if (!res)
		return NULL;

	res->should_kfree = true;

	ret = __kunit_add_resource(test, init, free, res, context);
	if (!ret) {
		
		kunit_get_resource(res);
		return res;
	}
	return NULL;
}


static inline void *kunit_alloc_resource(struct kunit *test,
					 kunit_resource_init_t init,
					 kunit_resource_free_t free,
					 gfp_t internal_gfp,
					 void *context)
{
	struct kunit_resource *res;

	res = kzalloc(sizeof(*res), internal_gfp);
	if (!res)
		return NULL;

	res->should_kfree = true;
	if (!__kunit_add_resource(test, init, free, res, context))
		return res->data;

	return NULL;
}

typedef bool (*kunit_resource_match_t)(struct kunit *test,
				       struct kunit_resource *res,
				       void *match_data);


static inline bool kunit_resource_name_match(struct kunit *test,
					     struct kunit_resource *res,
					     void *match_name)
{
	return res->name && strcmp(res->name, match_name) == 0;
}


static inline struct kunit_resource *
kunit_find_resource(struct kunit *test,
		    kunit_resource_match_t match,
		    void *match_data)
{
	struct kunit_resource *res, *found = NULL;
	unsigned long flags;

	spin_lock_irqsave(&test->lock, flags);

	list_for_each_entry_reverse(res, &test->resources, node) {
		if (match(test, res, (void *)match_data)) {
			found = res;
			kunit_get_resource(found);
			break;
		}
	}

	spin_unlock_irqrestore(&test->lock, flags);

	return found;
}


static inline struct kunit_resource *
kunit_find_named_resource(struct kunit *test,
			  const char *name)
{
	return kunit_find_resource(test, kunit_resource_name_match,
				   (void *)name);
}


int kunit_destroy_resource(struct kunit *test,
			   kunit_resource_match_t match,
			   void *match_data);

static inline int kunit_destroy_named_resource(struct kunit *test,
					       const char *name)
{
	return kunit_destroy_resource(test, kunit_resource_name_match,
				      (void *)name);
}


void kunit_remove_resource(struct kunit *test, struct kunit_resource *res);


typedef void (kunit_action_t)(void *);


int kunit_add_action(struct kunit *test, kunit_action_t *action, void *ctx);


int kunit_add_action_or_reset(struct kunit *test, kunit_action_t *action,
			      void *ctx);


void kunit_remove_action(struct kunit *test,
			 kunit_action_t *action,
			 void *ctx);


void kunit_release_action(struct kunit *test,
			  kunit_action_t *action,
			  void *ctx);
#endif 
