/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _MEDIA_ENTITY_H
#define _MEDIA_ENTITY_H

#include <linux/bitmap.h>
#include <linux/bug.h>
#include <linux/container_of.h>
#include <linux/fwnode.h>
#include <linux/list.h>
#include <linux/media.h>
#include <linux/minmax.h>
#include <linux/types.h>




enum media_gobj_type {
	MEDIA_GRAPH_ENTITY,
	MEDIA_GRAPH_PAD,
	MEDIA_GRAPH_LINK,
	MEDIA_GRAPH_INTF_DEVNODE,
};

#define MEDIA_BITS_PER_TYPE		8
#define MEDIA_BITS_PER_ID		(32 - MEDIA_BITS_PER_TYPE)
#define MEDIA_ID_MASK			 GENMASK_ULL(MEDIA_BITS_PER_ID - 1, 0)




struct media_gobj {
	struct media_device	*mdev;
	u32			id;
	struct list_head	list;
};

#define MEDIA_ENTITY_ENUM_MAX_DEPTH	16


struct media_entity_enum {
	unsigned long *bmap;
	int idx_max;
};


struct media_graph {
	struct {
		struct media_entity *entity;
		struct list_head *link;
	} stack[MEDIA_ENTITY_ENUM_MAX_DEPTH];

	struct media_entity_enum ent_enum;
	int top;
};


struct media_pipeline {
	bool allocated;
	struct media_device *mdev;
	struct list_head pads;
	int start_count;
};


struct media_pipeline_pad {
	struct list_head list;
	struct media_pipeline *pipe;
	struct media_pad *pad;
};


struct media_pipeline_pad_iter {
	struct list_head *cursor;
};


struct media_pipeline_entity_iter {
	struct media_entity_enum ent_enum;
	struct list_head *cursor;
};


struct media_link {
	struct media_gobj graph_obj;
	struct list_head list;
	union {
		struct media_gobj *gobj0;
		struct media_pad *source;
		struct media_interface *intf;
	};
	union {
		struct media_gobj *gobj1;
		struct media_pad *sink;
		struct media_entity *entity;
	};
	struct media_link *reverse;
	unsigned long flags;
	bool is_backlink;
};


enum media_pad_signal_type {
	PAD_SIGNAL_DEFAULT = 0,
	PAD_SIGNAL_ANALOG,
	PAD_SIGNAL_DV,
	PAD_SIGNAL_AUDIO,
};


struct media_pad {
	struct media_gobj graph_obj;	
	struct media_entity *entity;
	u16 index;
	u16 num_links;
	enum media_pad_signal_type sig_type;
	unsigned long flags;

	
	struct media_pipeline *pipe;
};


struct media_entity_operations {
	int (*get_fwnode_pad)(struct media_entity *entity,
			      struct fwnode_endpoint *endpoint);
	int (*link_setup)(struct media_entity *entity,
			  const struct media_pad *local,
			  const struct media_pad *remote, u32 flags);
	int (*link_validate)(struct media_link *link);
	bool (*has_pad_interdep)(struct media_entity *entity, unsigned int pad0,
				 unsigned int pad1);
	bool (*has_route)(struct media_entity *entity, unsigned int pad0,
			  unsigned int pad1);
};


enum media_entity_type {
	MEDIA_ENTITY_TYPE_BASE,
	MEDIA_ENTITY_TYPE_VIDEO_DEVICE,
	MEDIA_ENTITY_TYPE_V4L2_SUBDEV,
};


struct media_entity {
	struct media_gobj graph_obj;	
	const char *name;
	enum media_entity_type obj_type;
	u32 function;
	unsigned long flags;

	u16 num_pads;
	u16 num_links;
	u16 num_backlinks;
	int internal_idx;

	struct media_pad *pads;
	struct list_head links;

	const struct media_entity_operations *ops;

	int use_count;

	union {
		struct {
			u32 major;
			u32 minor;
		} dev;
	} info;
};


#define media_entity_for_each_pad(entity, iter)			\
	for (iter = (entity)->pads;				\
	     iter < &(entity)->pads[(entity)->num_pads];	\
	     ++iter)


struct media_interface {
	struct media_gobj		graph_obj;
	struct list_head		links;
	u32				type;
	u32				flags;
};


struct media_intf_devnode {
	struct media_interface		intf;

	
	u32				major;
	u32				minor;
};


static inline u32 media_entity_id(struct media_entity *entity)
{
	return entity->graph_obj.id;
}


static inline enum media_gobj_type media_type(struct media_gobj *gobj)
{
	return gobj->id >> MEDIA_BITS_PER_ID;
}


static inline u32 media_id(struct media_gobj *gobj)
{
	return gobj->id & MEDIA_ID_MASK;
}


static inline u32 media_gobj_gen_id(enum media_gobj_type type, u64 local_id)
{
	u32 id;

	id = type << MEDIA_BITS_PER_ID;
	id |= local_id & MEDIA_ID_MASK;

	return id;
}


static inline bool is_media_entity_v4l2_video_device(struct media_entity *entity)
{
	return entity && entity->obj_type == MEDIA_ENTITY_TYPE_VIDEO_DEVICE;
}


static inline bool is_media_entity_v4l2_subdev(struct media_entity *entity)
{
	return entity && entity->obj_type == MEDIA_ENTITY_TYPE_V4L2_SUBDEV;
}


__must_check int media_entity_enum_init(struct media_entity_enum *ent_enum,
					struct media_device *mdev);


void media_entity_enum_cleanup(struct media_entity_enum *ent_enum);


static inline void media_entity_enum_zero(struct media_entity_enum *ent_enum)
{
	bitmap_zero(ent_enum->bmap, ent_enum->idx_max);
}


static inline void media_entity_enum_set(struct media_entity_enum *ent_enum,
					 struct media_entity *entity)
{
	if (WARN_ON(entity->internal_idx >= ent_enum->idx_max))
		return;

	__set_bit(entity->internal_idx, ent_enum->bmap);
}


static inline void media_entity_enum_clear(struct media_entity_enum *ent_enum,
					   struct media_entity *entity)
{
	if (WARN_ON(entity->internal_idx >= ent_enum->idx_max))
		return;

	__clear_bit(entity->internal_idx, ent_enum->bmap);
}


static inline bool media_entity_enum_test(struct media_entity_enum *ent_enum,
					  struct media_entity *entity)
{
	if (WARN_ON(entity->internal_idx >= ent_enum->idx_max))
		return true;

	return test_bit(entity->internal_idx, ent_enum->bmap);
}


static inline bool
media_entity_enum_test_and_set(struct media_entity_enum *ent_enum,
			       struct media_entity *entity)
{
	if (WARN_ON(entity->internal_idx >= ent_enum->idx_max))
		return true;

	return __test_and_set_bit(entity->internal_idx, ent_enum->bmap);
}


static inline bool media_entity_enum_empty(struct media_entity_enum *ent_enum)
{
	return bitmap_empty(ent_enum->bmap, ent_enum->idx_max);
}


static inline bool media_entity_enum_intersects(
	struct media_entity_enum *ent_enum1,
	struct media_entity_enum *ent_enum2)
{
	WARN_ON(ent_enum1->idx_max != ent_enum2->idx_max);

	return bitmap_intersects(ent_enum1->bmap, ent_enum2->bmap,
				 min(ent_enum1->idx_max, ent_enum2->idx_max));
}


#define gobj_to_entity(gobj) \
		container_of(gobj, struct media_entity, graph_obj)


#define gobj_to_pad(gobj) \
		container_of(gobj, struct media_pad, graph_obj)


#define gobj_to_link(gobj) \
		container_of(gobj, struct media_link, graph_obj)


#define gobj_to_intf(gobj) \
		container_of(gobj, struct media_interface, graph_obj)


#define intf_to_devnode(intf) \
		container_of(intf, struct media_intf_devnode, intf)


void media_gobj_create(struct media_device *mdev,
		    enum media_gobj_type type,
		    struct media_gobj *gobj);


void media_gobj_destroy(struct media_gobj *gobj);


int media_entity_pads_init(struct media_entity *entity, u16 num_pads,
		      struct media_pad *pads);


#if IS_ENABLED(CONFIG_MEDIA_CONTROLLER)
static inline void media_entity_cleanup(struct media_entity *entity) {}
#else
#define media_entity_cleanup(entity) do { } while (false)
#endif


int media_get_pad_index(struct media_entity *entity, u32 pad_type,
			enum media_pad_signal_type sig_type);


__must_check int media_create_pad_link(struct media_entity *source,
			u16 source_pad, struct media_entity *sink,
			u16 sink_pad, u32 flags);


int media_create_pad_links(const struct media_device *mdev,
			   const u32 source_function,
			   struct media_entity *source,
			   const u16 source_pad,
			   const u32 sink_function,
			   struct media_entity *sink,
			   const u16 sink_pad,
			   u32 flags,
			   const bool allow_both_undefined);

void __media_entity_remove_links(struct media_entity *entity);


void media_entity_remove_links(struct media_entity *entity);


int __media_entity_setup_link(struct media_link *link, u32 flags);


int media_entity_setup_link(struct media_link *link, u32 flags);


struct media_link *media_entity_find_link(struct media_pad *source,
		struct media_pad *sink);


struct media_pad *media_pad_remote_pad_first(const struct media_pad *pad);


struct media_pad *media_pad_remote_pad_unique(const struct media_pad *pad);


struct media_pad *
media_entity_remote_pad_unique(const struct media_entity *entity,
			       unsigned int type);


static inline struct media_pad *
media_entity_remote_source_pad_unique(const struct media_entity *entity)
{
	return media_entity_remote_pad_unique(entity, MEDIA_PAD_FL_SOURCE);
}


static inline bool media_pad_is_streaming(const struct media_pad *pad)
{
	return pad->pipe;
}


static inline bool media_entity_is_streaming(const struct media_entity *entity)
{
	struct media_pad *pad;

	media_entity_for_each_pad(entity, pad) {
		if (media_pad_is_streaming(pad))
			return true;
	}

	return false;
}


struct media_pipeline *media_entity_pipeline(struct media_entity *entity);


struct media_pipeline *media_pad_pipeline(struct media_pad *pad);


int media_entity_get_fwnode_pad(struct media_entity *entity,
				const struct fwnode_handle *fwnode,
				unsigned long direction_flags);


__must_check int media_graph_walk_init(
	struct media_graph *graph, struct media_device *mdev);


void media_graph_walk_cleanup(struct media_graph *graph);

bool media_entity_has_route(struct media_entity *entity, unsigned int sink,
			    unsigned int source);


void media_graph_walk_start(struct media_graph *graph,
			    struct media_entity *entity);


struct media_entity *media_graph_walk_next(struct media_graph *graph);


__must_check int media_pipeline_start(struct media_pad *pad,
				      struct media_pipeline *pipe);

__must_check int __media_pipeline_start(struct media_pad *pad,
					struct media_pipeline *pipe);


void media_pipeline_stop(struct media_pad *pad);


void __media_pipeline_stop(struct media_pad *pad);

struct media_pad *
__media_pipeline_pad_iter_next(struct media_pipeline *pipe,
			       struct media_pipeline_pad_iter *iter,
			       struct media_pad *pad);


#define media_pipeline_for_each_pad(pipe, iter, pad)			\
	for (pad = __media_pipeline_pad_iter_next((pipe), iter, NULL);	\
	     pad != NULL;						\
	     pad = __media_pipeline_pad_iter_next((pipe), iter, pad))


int media_pipeline_entity_iter_init(struct media_pipeline *pipe,
				    struct media_pipeline_entity_iter *iter);


void media_pipeline_entity_iter_cleanup(struct media_pipeline_entity_iter *iter);

struct media_entity *
__media_pipeline_entity_iter_next(struct media_pipeline *pipe,
				  struct media_pipeline_entity_iter *iter,
				  struct media_entity *entity);


#define media_pipeline_for_each_entity(pipe, iter, entity)			\
	for (entity = __media_pipeline_entity_iter_next((pipe), iter, NULL);	\
	     entity != NULL;							\
	     entity = __media_pipeline_entity_iter_next((pipe), iter, entity))


__must_check int media_pipeline_alloc_start(struct media_pad *pad);


struct media_intf_devnode *
__must_check media_devnode_create(struct media_device *mdev,
				  u32 type, u32 flags,
				  u32 major, u32 minor);

void media_devnode_remove(struct media_intf_devnode *devnode);


struct media_link *
__must_check media_create_intf_link(struct media_entity *entity,
				    struct media_interface *intf,
				    u32 flags);

void __media_remove_intf_link(struct media_link *link);


void media_remove_intf_link(struct media_link *link);


void __media_remove_intf_links(struct media_interface *intf);


void media_remove_intf_links(struct media_interface *intf);



#define media_entity_call(entity, operation, args...)			\
	(((entity)->ops && (entity)->ops->operation) ?			\
	 (entity)->ops->operation((entity) , ##args) : -ENOIOCTLCMD)


struct media_link *
media_create_ancillary_link(struct media_entity *primary,
			    struct media_entity *ancillary);


struct media_link *__media_entity_next_link(struct media_entity *entity,
					    struct media_link *link,
					    unsigned long link_type);


#define for_each_media_entity_data_link(entity, link)			\
	for (link = __media_entity_next_link(entity, NULL,		\
					     MEDIA_LNK_FL_DATA_LINK);	\
	     link;							\
	     link = __media_entity_next_link(entity, link,		\
					     MEDIA_LNK_FL_DATA_LINK))

#endif
