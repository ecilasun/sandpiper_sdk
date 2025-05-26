/* SPDX-License-Identifier: GPL-2.0 OR Linux-OpenIB */


#ifndef _UVERBS_TYPES_
#define _UVERBS_TYPES_

#include <linux/kernel.h>
#include <rdma/ib_verbs.h>

struct uverbs_obj_type;
struct uverbs_api_object;

enum rdma_lookup_mode {
	UVERBS_LOOKUP_READ,
	UVERBS_LOOKUP_WRITE,
	
	UVERBS_LOOKUP_DESTROY,
};


struct uverbs_obj_type_class {
	struct ib_uobject *(*alloc_begin)(const struct uverbs_api_object *obj,
					  struct uverbs_attr_bundle *attrs);
	
	void (*alloc_commit)(struct ib_uobject *uobj);
	
	void (*alloc_abort)(struct ib_uobject *uobj);

	struct ib_uobject *(*lookup_get)(const struct uverbs_api_object *obj,
					 struct ib_uverbs_file *ufile, s64 id,
					 enum rdma_lookup_mode mode);
	void (*lookup_put)(struct ib_uobject *uobj, enum rdma_lookup_mode mode);
	
	int __must_check (*destroy_hw)(struct ib_uobject *uobj,
				       enum rdma_remove_reason why,
				       struct uverbs_attr_bundle *attrs);
	void (*remove_handle)(struct ib_uobject *uobj);
	void (*swap_uobjects)(struct ib_uobject *obj_old,
			      struct ib_uobject *obj_new);
};

struct uverbs_obj_type {
	const struct uverbs_obj_type_class * const type_class;
	size_t	     obj_size;
};



struct uverbs_obj_idr_type {
	
	struct uverbs_obj_type  type;

	
	int __must_check (*destroy_object)(struct ib_uobject *uobj,
					   enum rdma_remove_reason why,
					   struct uverbs_attr_bundle *attrs);
};

struct ib_uobject *rdma_lookup_get_uobject(const struct uverbs_api_object *obj,
					   struct ib_uverbs_file *ufile, s64 id,
					   enum rdma_lookup_mode mode,
					   struct uverbs_attr_bundle *attrs);
void rdma_lookup_put_uobject(struct ib_uobject *uobj,
			     enum rdma_lookup_mode mode);
struct ib_uobject *rdma_alloc_begin_uobject(const struct uverbs_api_object *obj,
					    struct uverbs_attr_bundle *attrs);
void rdma_alloc_abort_uobject(struct ib_uobject *uobj,
			      struct uverbs_attr_bundle *attrs,
			      bool hw_obj_valid);
void rdma_alloc_commit_uobject(struct ib_uobject *uobj,
			       struct uverbs_attr_bundle *attrs);
void rdma_assign_uobject(struct ib_uobject *to_uobj,
			 struct ib_uobject *new_uobj,
			 struct uverbs_attr_bundle *attrs);


static inline void uverbs_uobject_get(struct ib_uobject *uobject)
{
	kref_get(&uobject->ref);
}
void uverbs_uobject_put(struct ib_uobject *uobject);

struct uverbs_obj_fd_type {
	
	struct uverbs_obj_type  type;
	void (*destroy_object)(struct ib_uobject *uobj,
			       enum rdma_remove_reason why);
	const struct file_operations	*fops;
	const char			*name;
	int				flags;
};

extern const struct uverbs_obj_type_class uverbs_idr_class;
extern const struct uverbs_obj_type_class uverbs_fd_class;
int uverbs_uobject_fd_release(struct inode *inode, struct file *filp);

#define UVERBS_BUILD_BUG_ON(cond) (sizeof(char[1 - 2 * !!(cond)]) -	\
				   sizeof(char))
#define UVERBS_TYPE_ALLOC_FD(_obj_size, _destroy_object, _fops, _name, _flags) \
	((&((const struct uverbs_obj_fd_type)				\
	 {.type = {							\
		.type_class = &uverbs_fd_class,				\
		.obj_size = (_obj_size) +				\
			UVERBS_BUILD_BUG_ON((_obj_size) <               \
					    sizeof(struct ib_uobject)), \
	 },								\
	 .destroy_object = _destroy_object,				\
	 .fops = _fops,							\
	 .name = _name,							\
	 .flags = _flags}))->type)
#define UVERBS_TYPE_ALLOC_IDR_SZ(_size, _destroy_object)	\
	((&((const struct uverbs_obj_idr_type)				\
	 {.type = {							\
		.type_class = &uverbs_idr_class,			\
		.obj_size = (_size) +					\
			UVERBS_BUILD_BUG_ON((_size) <			\
					    sizeof(struct ib_uobject))	\
	 },								\
	 .destroy_object = _destroy_object,}))->type)
#define UVERBS_TYPE_ALLOC_IDR(_destroy_object)			\
	 UVERBS_TYPE_ALLOC_IDR_SZ(sizeof(struct ib_uobject),	\
				  _destroy_object)

#endif
