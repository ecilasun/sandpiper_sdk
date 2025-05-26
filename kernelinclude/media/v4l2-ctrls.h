/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _V4L2_CTRLS_H
#define _V4L2_CTRLS_H

#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/videodev2.h>
#include <media/media-request.h>


struct file;
struct poll_table_struct;
struct v4l2_ctrl;
struct v4l2_ctrl_handler;
struct v4l2_ctrl_helper;
struct v4l2_fh;
struct v4l2_fwnode_device_properties;
struct v4l2_subdev;
struct v4l2_subscribed_event;
struct video_device;


union v4l2_ctrl_ptr {
	s32 *p_s32;
	s64 *p_s64;
	u8 *p_u8;
	u16 *p_u16;
	u32 *p_u32;
	char *p_char;
	struct v4l2_ctrl_mpeg2_sequence *p_mpeg2_sequence;
	struct v4l2_ctrl_mpeg2_picture *p_mpeg2_picture;
	struct v4l2_ctrl_mpeg2_quantisation *p_mpeg2_quantisation;
	struct v4l2_ctrl_fwht_params *p_fwht_params;
	struct v4l2_ctrl_h264_sps *p_h264_sps;
	struct v4l2_ctrl_h264_pps *p_h264_pps;
	struct v4l2_ctrl_h264_scaling_matrix *p_h264_scaling_matrix;
	struct v4l2_ctrl_h264_slice_params *p_h264_slice_params;
	struct v4l2_ctrl_h264_decode_params *p_h264_decode_params;
	struct v4l2_ctrl_h264_pred_weights *p_h264_pred_weights;
	struct v4l2_ctrl_vp8_frame *p_vp8_frame;
	struct v4l2_ctrl_hevc_sps *p_hevc_sps;
	struct v4l2_ctrl_hevc_pps *p_hevc_pps;
	struct v4l2_ctrl_hevc_slice_params *p_hevc_slice_params;
	struct v4l2_ctrl_vp9_compressed_hdr *p_vp9_compressed_hdr_probs;
	struct v4l2_ctrl_vp9_frame *p_vp9_frame;
	struct v4l2_ctrl_hdr10_cll_info *p_hdr10_cll;
	struct v4l2_ctrl_hdr10_mastering_display *p_hdr10_mastering;
	struct v4l2_area *p_area;
	struct v4l2_ctrl_av1_sequence *p_av1_sequence;
	struct v4l2_ctrl_av1_tile_group_entry *p_av1_tile_group_entry;
	struct v4l2_ctrl_av1_frame *p_av1_frame;
	struct v4l2_ctrl_av1_film_grain *p_av1_film_grain;
	void *p;
	const void *p_const;
};


static inline union v4l2_ctrl_ptr v4l2_ctrl_ptr_create(void *ptr)
{
	union v4l2_ctrl_ptr p = { .p = ptr };

	return p;
}


struct v4l2_ctrl_ops {
	int (*g_volatile_ctrl)(struct v4l2_ctrl *ctrl);
	int (*try_ctrl)(struct v4l2_ctrl *ctrl);
	int (*s_ctrl)(struct v4l2_ctrl *ctrl);
};


struct v4l2_ctrl_type_ops {
	bool (*equal)(const struct v4l2_ctrl *ctrl,
		      union v4l2_ctrl_ptr ptr1, union v4l2_ctrl_ptr ptr2);
	void (*init)(const struct v4l2_ctrl *ctrl, u32 from_idx,
		     union v4l2_ctrl_ptr ptr);
	void (*log)(const struct v4l2_ctrl *ctrl);
	int (*validate)(const struct v4l2_ctrl *ctrl, union v4l2_ctrl_ptr ptr);
};


typedef void (*v4l2_ctrl_notify_fnc)(struct v4l2_ctrl *ctrl, void *priv);


struct v4l2_ctrl {
	
	struct list_head node;
	struct list_head ev_subs;
	struct v4l2_ctrl_handler *handler;
	struct v4l2_ctrl **cluster;
	unsigned int ncontrols;

	unsigned int done:1;

	unsigned int is_new:1;
	unsigned int has_changed:1;
	unsigned int is_private:1;
	unsigned int is_auto:1;
	unsigned int is_int:1;
	unsigned int is_string:1;
	unsigned int is_ptr:1;
	unsigned int is_array:1;
	unsigned int is_dyn_array:1;
	unsigned int has_volatiles:1;
	unsigned int call_notify:1;
	unsigned int manual_mode_value:8;

	const struct v4l2_ctrl_ops *ops;
	const struct v4l2_ctrl_type_ops *type_ops;
	u32 id;
	const char *name;
	enum v4l2_ctrl_type type;
	s64 minimum, maximum, default_value;
	u32 elems;
	u32 elem_size;
	u32 new_elems;
	u32 dims[V4L2_CTRL_MAX_DIMS];
	u32 nr_of_dims;
	union {
		u64 step;
		u64 menu_skip_mask;
	};
	union {
		const char * const *qmenu;
		const s64 *qmenu_int;
	};
	unsigned long flags;
	void *priv;
	void *p_array;
	u32 p_array_alloc_elems;
	s32 val;
	struct {
		s32 val;
	} cur;

	union v4l2_ctrl_ptr p_def;
	union v4l2_ctrl_ptr p_new;
	union v4l2_ctrl_ptr p_cur;
};


struct v4l2_ctrl_ref {
	struct list_head node;
	struct v4l2_ctrl_ref *next;
	struct v4l2_ctrl *ctrl;
	struct v4l2_ctrl_helper *helper;
	bool from_other_dev;
	bool req_done;
	bool p_req_valid;
	bool p_req_array_enomem;
	u32 p_req_array_alloc_elems;
	u32 p_req_elems;
	union v4l2_ctrl_ptr p_req;
};


struct v4l2_ctrl_handler {
	struct mutex _lock;
	struct mutex *lock;
	struct list_head ctrls;
	struct list_head ctrl_refs;
	struct v4l2_ctrl_ref *cached;
	struct v4l2_ctrl_ref **buckets;
	v4l2_ctrl_notify_fnc notify;
	void *notify_priv;
	u16 nr_of_buckets;
	int error;
	bool request_is_queued;
	struct list_head requests;
	struct list_head requests_queued;
	struct media_request_object req_obj;
};


struct v4l2_ctrl_config {
	const struct v4l2_ctrl_ops *ops;
	const struct v4l2_ctrl_type_ops *type_ops;
	u32 id;
	const char *name;
	enum v4l2_ctrl_type type;
	s64 min;
	s64 max;
	u64 step;
	s64 def;
	union v4l2_ctrl_ptr p_def;
	u32 dims[V4L2_CTRL_MAX_DIMS];
	u32 elem_size;
	u32 flags;
	u64 menu_skip_mask;
	const char * const *qmenu;
	const s64 *qmenu_int;
	unsigned int is_private:1;
};


void v4l2_ctrl_fill(u32 id, const char **name, enum v4l2_ctrl_type *type,
		    s64 *min, s64 *max, u64 *step, s64 *def, u32 *flags);



int v4l2_ctrl_handler_init_class(struct v4l2_ctrl_handler *hdl,
				 unsigned int nr_of_controls_hint,
				 struct lock_class_key *key, const char *name);

#ifdef CONFIG_LOCKDEP


#define v4l2_ctrl_handler_init(hdl, nr_of_controls_hint)		\
(									\
	({								\
		static struct lock_class_key _key;			\
		v4l2_ctrl_handler_init_class(hdl, nr_of_controls_hint,	\
					&_key,				\
					KBUILD_BASENAME ":"		\
					__stringify(__LINE__) ":"	\
					"(" #hdl ")->_lock");		\
	})								\
)
#else
#define v4l2_ctrl_handler_init(hdl, nr_of_controls_hint)		\
	v4l2_ctrl_handler_init_class(hdl, nr_of_controls_hint, NULL, NULL)
#endif


void v4l2_ctrl_handler_free(struct v4l2_ctrl_handler *hdl);


static inline void v4l2_ctrl_lock(struct v4l2_ctrl *ctrl)
{
	mutex_lock(ctrl->handler->lock);
}


static inline void v4l2_ctrl_unlock(struct v4l2_ctrl *ctrl)
{
	mutex_unlock(ctrl->handler->lock);
}


int __v4l2_ctrl_handler_setup(struct v4l2_ctrl_handler *hdl);


int v4l2_ctrl_handler_setup(struct v4l2_ctrl_handler *hdl);


void v4l2_ctrl_handler_log_status(struct v4l2_ctrl_handler *hdl,
				  const char *prefix);


struct v4l2_ctrl *v4l2_ctrl_new_custom(struct v4l2_ctrl_handler *hdl,
				       const struct v4l2_ctrl_config *cfg,
				       void *priv);


struct v4l2_ctrl *v4l2_ctrl_new_std(struct v4l2_ctrl_handler *hdl,
				    const struct v4l2_ctrl_ops *ops,
				    u32 id, s64 min, s64 max, u64 step,
				    s64 def);


struct v4l2_ctrl *v4l2_ctrl_new_std_menu(struct v4l2_ctrl_handler *hdl,
					 const struct v4l2_ctrl_ops *ops,
					 u32 id, u8 max, u64 mask, u8 def);


struct v4l2_ctrl *v4l2_ctrl_new_std_menu_items(struct v4l2_ctrl_handler *hdl,
					       const struct v4l2_ctrl_ops *ops,
					       u32 id, u8 max,
					       u64 mask, u8 def,
					       const char * const *qmenu);


struct v4l2_ctrl *v4l2_ctrl_new_std_compound(struct v4l2_ctrl_handler *hdl,
					     const struct v4l2_ctrl_ops *ops,
					     u32 id,
					     const union v4l2_ctrl_ptr p_def);


struct v4l2_ctrl *v4l2_ctrl_new_int_menu(struct v4l2_ctrl_handler *hdl,
					 const struct v4l2_ctrl_ops *ops,
					 u32 id, u8 max, u8 def,
					 const s64 *qmenu_int);



typedef bool (*v4l2_ctrl_filter)(const struct v4l2_ctrl *ctrl);


int v4l2_ctrl_add_handler(struct v4l2_ctrl_handler *hdl,
			  struct v4l2_ctrl_handler *add,
			  v4l2_ctrl_filter filter,
			  bool from_other_dev);


bool v4l2_ctrl_radio_filter(const struct v4l2_ctrl *ctrl);


void v4l2_ctrl_cluster(unsigned int ncontrols, struct v4l2_ctrl **controls);



void v4l2_ctrl_auto_cluster(unsigned int ncontrols,
			    struct v4l2_ctrl **controls,
			    u8 manual_val, bool set_volatile);



struct v4l2_ctrl *v4l2_ctrl_find(struct v4l2_ctrl_handler *hdl, u32 id);


void v4l2_ctrl_activate(struct v4l2_ctrl *ctrl, bool active);


void __v4l2_ctrl_grab(struct v4l2_ctrl *ctrl, bool grabbed);


static inline void v4l2_ctrl_grab(struct v4l2_ctrl *ctrl, bool grabbed)
{
	if (!ctrl)
		return;

	v4l2_ctrl_lock(ctrl);
	__v4l2_ctrl_grab(ctrl, grabbed);
	v4l2_ctrl_unlock(ctrl);
}


int __v4l2_ctrl_modify_range(struct v4l2_ctrl *ctrl,
			     s64 min, s64 max, u64 step, s64 def);


static inline int v4l2_ctrl_modify_range(struct v4l2_ctrl *ctrl,
					 s64 min, s64 max, u64 step, s64 def)
{
	int rval;

	v4l2_ctrl_lock(ctrl);
	rval = __v4l2_ctrl_modify_range(ctrl, min, max, step, def);
	v4l2_ctrl_unlock(ctrl);

	return rval;
}


int __v4l2_ctrl_modify_dimensions(struct v4l2_ctrl *ctrl,
				  u32 dims[V4L2_CTRL_MAX_DIMS]);


static inline int v4l2_ctrl_modify_dimensions(struct v4l2_ctrl *ctrl,
					      u32 dims[V4L2_CTRL_MAX_DIMS])
{
	int rval;

	v4l2_ctrl_lock(ctrl);
	rval = __v4l2_ctrl_modify_dimensions(ctrl, dims);
	v4l2_ctrl_unlock(ctrl);

	return rval;
}


void v4l2_ctrl_notify(struct v4l2_ctrl *ctrl, v4l2_ctrl_notify_fnc notify,
		      void *priv);


const char *v4l2_ctrl_get_name(u32 id);


const char * const *v4l2_ctrl_get_menu(u32 id);


const s64 *v4l2_ctrl_get_int_menu(u32 id, u32 *len);


s32 v4l2_ctrl_g_ctrl(struct v4l2_ctrl *ctrl);


int __v4l2_ctrl_s_ctrl(struct v4l2_ctrl *ctrl, s32 val);


static inline int v4l2_ctrl_s_ctrl(struct v4l2_ctrl *ctrl, s32 val)
{
	int rval;

	v4l2_ctrl_lock(ctrl);
	rval = __v4l2_ctrl_s_ctrl(ctrl, val);
	v4l2_ctrl_unlock(ctrl);

	return rval;
}


s64 v4l2_ctrl_g_ctrl_int64(struct v4l2_ctrl *ctrl);


int __v4l2_ctrl_s_ctrl_int64(struct v4l2_ctrl *ctrl, s64 val);


static inline int v4l2_ctrl_s_ctrl_int64(struct v4l2_ctrl *ctrl, s64 val)
{
	int rval;

	v4l2_ctrl_lock(ctrl);
	rval = __v4l2_ctrl_s_ctrl_int64(ctrl, val);
	v4l2_ctrl_unlock(ctrl);

	return rval;
}


int __v4l2_ctrl_s_ctrl_string(struct v4l2_ctrl *ctrl, const char *s);


static inline int v4l2_ctrl_s_ctrl_string(struct v4l2_ctrl *ctrl, const char *s)
{
	int rval;

	v4l2_ctrl_lock(ctrl);
	rval = __v4l2_ctrl_s_ctrl_string(ctrl, s);
	v4l2_ctrl_unlock(ctrl);

	return rval;
}


int __v4l2_ctrl_s_ctrl_compound(struct v4l2_ctrl *ctrl,
				enum v4l2_ctrl_type type, const void *p);


static inline int v4l2_ctrl_s_ctrl_compound(struct v4l2_ctrl *ctrl,
					    enum v4l2_ctrl_type type,
					    const void *p)
{
	int rval;

	v4l2_ctrl_lock(ctrl);
	rval = __v4l2_ctrl_s_ctrl_compound(ctrl, type, p);
	v4l2_ctrl_unlock(ctrl);

	return rval;
}


#define __v4l2_ctrl_s_ctrl_area(ctrl, area) \
	__v4l2_ctrl_s_ctrl_compound((ctrl), V4L2_CTRL_TYPE_AREA, (area))
#define v4l2_ctrl_s_ctrl_area(ctrl, area) \
	v4l2_ctrl_s_ctrl_compound((ctrl), V4L2_CTRL_TYPE_AREA, (area))


extern const struct v4l2_subscribed_event_ops v4l2_ctrl_sub_ev_ops;


void v4l2_ctrl_replace(struct v4l2_event *old, const struct v4l2_event *new);


void v4l2_ctrl_merge(const struct v4l2_event *old, struct v4l2_event *new);


int v4l2_ctrl_log_status(struct file *file, void *fh);


int v4l2_ctrl_subscribe_event(struct v4l2_fh *fh,
				const struct v4l2_event_subscription *sub);


__poll_t v4l2_ctrl_poll(struct file *file, struct poll_table_struct *wait);


int v4l2_ctrl_request_setup(struct media_request *req,
			     struct v4l2_ctrl_handler *parent);


void v4l2_ctrl_request_complete(struct media_request *req,
				struct v4l2_ctrl_handler *parent);


struct v4l2_ctrl_handler *v4l2_ctrl_request_hdl_find(struct media_request *req,
					struct v4l2_ctrl_handler *parent);


static inline void v4l2_ctrl_request_hdl_put(struct v4l2_ctrl_handler *hdl)
{
	if (hdl)
		media_request_object_put(&hdl->req_obj);
}


struct v4l2_ctrl *
v4l2_ctrl_request_hdl_ctrl_find(struct v4l2_ctrl_handler *hdl, u32 id);




int v4l2_queryctrl(struct v4l2_ctrl_handler *hdl, struct v4l2_queryctrl *qc);


int v4l2_query_ext_ctrl(struct v4l2_ctrl_handler *hdl,
			struct v4l2_query_ext_ctrl *qc);


int v4l2_querymenu(struct v4l2_ctrl_handler *hdl, struct v4l2_querymenu *qm);


int v4l2_g_ctrl(struct v4l2_ctrl_handler *hdl, struct v4l2_control *ctrl);


int v4l2_s_ctrl(struct v4l2_fh *fh, struct v4l2_ctrl_handler *hdl,
		struct v4l2_control *ctrl);


int v4l2_g_ext_ctrls(struct v4l2_ctrl_handler *hdl, struct video_device *vdev,
		     struct media_device *mdev, struct v4l2_ext_controls *c);


int v4l2_try_ext_ctrls(struct v4l2_ctrl_handler *hdl,
		       struct video_device *vdev,
		       struct media_device *mdev,
		       struct v4l2_ext_controls *c);


int v4l2_s_ext_ctrls(struct v4l2_fh *fh, struct v4l2_ctrl_handler *hdl,
		     struct video_device *vdev,
		     struct media_device *mdev,
		     struct v4l2_ext_controls *c);


int v4l2_ctrl_subdev_subscribe_event(struct v4l2_subdev *sd, struct v4l2_fh *fh,
				     struct v4l2_event_subscription *sub);


int v4l2_ctrl_subdev_log_status(struct v4l2_subdev *sd);


int v4l2_ctrl_new_fwnode_properties(struct v4l2_ctrl_handler *hdl,
				    const struct v4l2_ctrl_ops *ctrl_ops,
				    const struct v4l2_fwnode_device_properties *p);


bool v4l2_ctrl_type_op_equal(const struct v4l2_ctrl *ctrl,
			     union v4l2_ctrl_ptr ptr1, union v4l2_ctrl_ptr ptr2);


void v4l2_ctrl_type_op_init(const struct v4l2_ctrl *ctrl, u32 from_idx,
			    union v4l2_ctrl_ptr ptr);


void v4l2_ctrl_type_op_log(const struct v4l2_ctrl *ctrl);


int v4l2_ctrl_type_op_validate(const struct v4l2_ctrl *ctrl, union v4l2_ctrl_ptr ptr);

#endif
