/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _V4L2_SUBDEV_H
#define _V4L2_SUBDEV_H

#include <linux/types.h>
#include <linux/v4l2-subdev.h>
#include <media/media-entity.h>
#include <media/v4l2-async.h>
#include <media/v4l2-common.h>
#include <media/v4l2-dev.h>
#include <media/v4l2-fh.h>
#include <media/v4l2-mediabus.h>


#define V4L2_SUBDEV_IR_RX_NOTIFY		_IOW('v', 0, u32)
#define V4L2_SUBDEV_IR_RX_FIFO_SERVICE_REQ	0x00000001
#define V4L2_SUBDEV_IR_RX_END_OF_RX_DETECTED	0x00000002
#define V4L2_SUBDEV_IR_RX_HW_FIFO_OVERRUN	0x00000004
#define V4L2_SUBDEV_IR_RX_SW_FIFO_OVERRUN	0x00000008

#define V4L2_SUBDEV_IR_TX_NOTIFY		_IOW('v', 1, u32)
#define V4L2_SUBDEV_IR_TX_FIFO_SERVICE_REQ	0x00000001

#define	V4L2_DEVICE_NOTIFY_EVENT		_IOW('v', 2, struct v4l2_event)

struct v4l2_device;
struct v4l2_ctrl_handler;
struct v4l2_event;
struct v4l2_event_subscription;
struct v4l2_fh;
struct v4l2_subdev;
struct v4l2_subdev_fh;
struct tuner_setup;
struct v4l2_mbus_frame_desc;
struct led_classdev;


struct v4l2_decode_vbi_line {
	u32 is_second_field;
	u8 *p;
	u32 line;
	u32 type;
};






enum v4l2_subdev_io_pin_bits {
	V4L2_SUBDEV_IO_PIN_DISABLE	= 0,
	V4L2_SUBDEV_IO_PIN_OUTPUT	= 1,
	V4L2_SUBDEV_IO_PIN_INPUT	= 2,
	V4L2_SUBDEV_IO_PIN_SET_VALUE	= 3,
	V4L2_SUBDEV_IO_PIN_ACTIVE_LOW	= 4,
};


struct v4l2_subdev_io_pin_config {
	u32 flags;
	u8 pin;
	u8 function;
	u8 value;
	u8 strength;
};


struct v4l2_subdev_core_ops {
	int (*log_status)(struct v4l2_subdev *sd);
	int (*s_io_pin_config)(struct v4l2_subdev *sd, size_t n,
				      struct v4l2_subdev_io_pin_config *pincfg);
	int (*init)(struct v4l2_subdev *sd, u32 val);
	int (*load_fw)(struct v4l2_subdev *sd);
	int (*reset)(struct v4l2_subdev *sd, u32 val);
	int (*s_gpio)(struct v4l2_subdev *sd, u32 val);
	long (*command)(struct v4l2_subdev *sd, unsigned int cmd, void *arg);
	long (*ioctl)(struct v4l2_subdev *sd, unsigned int cmd, void *arg);
#ifdef CONFIG_COMPAT
	long (*compat_ioctl32)(struct v4l2_subdev *sd, unsigned int cmd,
			       unsigned long arg);
#endif
#ifdef CONFIG_VIDEO_ADV_DEBUG
	int (*g_register)(struct v4l2_subdev *sd, struct v4l2_dbg_register *reg);
	int (*s_register)(struct v4l2_subdev *sd, const struct v4l2_dbg_register *reg);
#endif
	int (*s_power)(struct v4l2_subdev *sd, int on);
	int (*interrupt_service_routine)(struct v4l2_subdev *sd,
						u32 status, bool *handled);
	int (*subscribe_event)(struct v4l2_subdev *sd, struct v4l2_fh *fh,
			       struct v4l2_event_subscription *sub);
	int (*unsubscribe_event)(struct v4l2_subdev *sd, struct v4l2_fh *fh,
				 struct v4l2_event_subscription *sub);
};


struct v4l2_subdev_tuner_ops {
	int (*standby)(struct v4l2_subdev *sd);
	int (*s_radio)(struct v4l2_subdev *sd);
	int (*s_frequency)(struct v4l2_subdev *sd, const struct v4l2_frequency *freq);
	int (*g_frequency)(struct v4l2_subdev *sd, struct v4l2_frequency *freq);
	int (*enum_freq_bands)(struct v4l2_subdev *sd, struct v4l2_frequency_band *band);
	int (*g_tuner)(struct v4l2_subdev *sd, struct v4l2_tuner *vt);
	int (*s_tuner)(struct v4l2_subdev *sd, const struct v4l2_tuner *vt);
	int (*g_modulator)(struct v4l2_subdev *sd, struct v4l2_modulator *vm);
	int (*s_modulator)(struct v4l2_subdev *sd, const struct v4l2_modulator *vm);
	int (*s_type_addr)(struct v4l2_subdev *sd, struct tuner_setup *type);
	int (*s_config)(struct v4l2_subdev *sd, const struct v4l2_priv_tun_config *config);
};


struct v4l2_subdev_audio_ops {
	int (*s_clock_freq)(struct v4l2_subdev *sd, u32 freq);
	int (*s_i2s_clock_freq)(struct v4l2_subdev *sd, u32 freq);
	int (*s_routing)(struct v4l2_subdev *sd, u32 input, u32 output, u32 config);
	int (*s_stream)(struct v4l2_subdev *sd, int enable);
};


struct v4l2_mbus_frame_desc_entry_csi2 {
	u8 vc;
	u8 dt;
};


enum v4l2_mbus_frame_desc_flags {
	V4L2_MBUS_FRAME_DESC_FL_LEN_MAX	= BIT(0),
	V4L2_MBUS_FRAME_DESC_FL_BLOB	= BIT(1),
};


struct v4l2_mbus_frame_desc_entry {
	enum v4l2_mbus_frame_desc_flags flags;
	u32 stream;
	u32 pixelcode;
	u32 length;
	union {
		struct v4l2_mbus_frame_desc_entry_csi2 csi2;
	} bus;
};

 
#define V4L2_FRAME_DESC_ENTRY_MAX	8


enum v4l2_mbus_frame_desc_type {
	V4L2_MBUS_FRAME_DESC_TYPE_UNDEFINED = 0,
	V4L2_MBUS_FRAME_DESC_TYPE_PARALLEL,
	V4L2_MBUS_FRAME_DESC_TYPE_CSI2,
};


struct v4l2_mbus_frame_desc {
	enum v4l2_mbus_frame_desc_type type;
	struct v4l2_mbus_frame_desc_entry entry[V4L2_FRAME_DESC_ENTRY_MAX];
	unsigned short num_entries;
};


enum v4l2_subdev_pre_streamon_flags {
	V4L2_SUBDEV_PRE_STREAMON_FL_MANUAL_LP = BIT(0),
};


struct v4l2_subdev_video_ops {
	int (*s_routing)(struct v4l2_subdev *sd, u32 input, u32 output, u32 config);
	int (*s_crystal_freq)(struct v4l2_subdev *sd, u32 freq, u32 flags);
	int (*g_std)(struct v4l2_subdev *sd, v4l2_std_id *norm);
	int (*s_std)(struct v4l2_subdev *sd, v4l2_std_id norm);
	int (*s_std_output)(struct v4l2_subdev *sd, v4l2_std_id std);
	int (*g_std_output)(struct v4l2_subdev *sd, v4l2_std_id *std);
	int (*querystd)(struct v4l2_subdev *sd, v4l2_std_id *std);
	int (*g_tvnorms)(struct v4l2_subdev *sd, v4l2_std_id *std);
	int (*g_tvnorms_output)(struct v4l2_subdev *sd, v4l2_std_id *std);
	int (*g_input_status)(struct v4l2_subdev *sd, u32 *status);
	int (*s_stream)(struct v4l2_subdev *sd, int enable);
	int (*g_pixelaspect)(struct v4l2_subdev *sd, struct v4l2_fract *aspect);
	int (*g_frame_interval)(struct v4l2_subdev *sd,
				struct v4l2_subdev_frame_interval *interval);
	int (*s_frame_interval)(struct v4l2_subdev *sd,
				struct v4l2_subdev_frame_interval *interval);
	int (*s_dv_timings)(struct v4l2_subdev *sd,
			struct v4l2_dv_timings *timings);
	int (*g_dv_timings)(struct v4l2_subdev *sd,
			struct v4l2_dv_timings *timings);
	int (*query_dv_timings)(struct v4l2_subdev *sd,
			struct v4l2_dv_timings *timings);
	int (*s_rx_buffer)(struct v4l2_subdev *sd, void *buf,
			   unsigned int *size);
	int (*pre_streamon)(struct v4l2_subdev *sd, u32 flags);
	int (*post_streamoff)(struct v4l2_subdev *sd);
};


struct v4l2_subdev_vbi_ops {
	int (*decode_vbi_line)(struct v4l2_subdev *sd, struct v4l2_decode_vbi_line *vbi_line);
	int (*s_vbi_data)(struct v4l2_subdev *sd, const struct v4l2_sliced_vbi_data *vbi_data);
	int (*g_vbi_data)(struct v4l2_subdev *sd, struct v4l2_sliced_vbi_data *vbi_data);
	int (*g_sliced_vbi_cap)(struct v4l2_subdev *sd, struct v4l2_sliced_vbi_cap *cap);
	int (*s_raw_fmt)(struct v4l2_subdev *sd, struct v4l2_vbi_format *fmt);
	int (*g_sliced_fmt)(struct v4l2_subdev *sd, struct v4l2_sliced_vbi_format *fmt);
	int (*s_sliced_fmt)(struct v4l2_subdev *sd, struct v4l2_sliced_vbi_format *fmt);
};


struct v4l2_subdev_sensor_ops {
	int (*g_skip_top_lines)(struct v4l2_subdev *sd, u32 *lines);
	int (*g_skip_frames)(struct v4l2_subdev *sd, u32 *frames);
};


enum v4l2_subdev_ir_mode {
	V4L2_SUBDEV_IR_MODE_PULSE_WIDTH,
};


struct v4l2_subdev_ir_parameters {
	unsigned int bytes_per_data_element;
	enum v4l2_subdev_ir_mode mode;

	bool enable;
	bool interrupt_enable;
	bool shutdown;

	bool modulation;
	u32 max_pulse_width;
	unsigned int carrier_freq;
	unsigned int duty_cycle;
	bool invert_level;

	
	bool invert_carrier_sense;

	
	u32 noise_filter_min_width;
	unsigned int carrier_range_lower;
	unsigned int carrier_range_upper;
	u32 resolution;
};


struct v4l2_subdev_ir_ops {
	
	int (*rx_read)(struct v4l2_subdev *sd, u8 *buf, size_t count,
				ssize_t *num);

	int (*rx_g_parameters)(struct v4l2_subdev *sd,
				struct v4l2_subdev_ir_parameters *params);
	int (*rx_s_parameters)(struct v4l2_subdev *sd,
				struct v4l2_subdev_ir_parameters *params);

	
	int (*tx_write)(struct v4l2_subdev *sd, u8 *buf, size_t count,
				ssize_t *num);

	int (*tx_g_parameters)(struct v4l2_subdev *sd,
				struct v4l2_subdev_ir_parameters *params);
	int (*tx_s_parameters)(struct v4l2_subdev *sd,
				struct v4l2_subdev_ir_parameters *params);
};


struct v4l2_subdev_pad_config {
	struct v4l2_mbus_framefmt try_fmt;
	struct v4l2_rect try_crop;
	struct v4l2_rect try_compose;
};


struct v4l2_subdev_stream_config {
	u32 pad;
	u32 stream;
	bool enabled;

	struct v4l2_mbus_framefmt fmt;
	struct v4l2_rect crop;
	struct v4l2_rect compose;
};


struct v4l2_subdev_stream_configs {
	u32 num_configs;
	struct v4l2_subdev_stream_config *configs;
};


struct v4l2_subdev_krouting {
	unsigned int num_routes;
	struct v4l2_subdev_route *routes;
};


struct v4l2_subdev_state {
	
	struct mutex _lock;
	struct mutex *lock;
	struct v4l2_subdev_pad_config *pads;
	struct v4l2_subdev_krouting routing;
	struct v4l2_subdev_stream_configs stream_configs;
};


struct v4l2_subdev_pad_ops {
	int (*init_cfg)(struct v4l2_subdev *sd,
			struct v4l2_subdev_state *state);
	int (*enum_mbus_code)(struct v4l2_subdev *sd,
			      struct v4l2_subdev_state *state,
			      struct v4l2_subdev_mbus_code_enum *code);
	int (*enum_frame_size)(struct v4l2_subdev *sd,
			       struct v4l2_subdev_state *state,
			       struct v4l2_subdev_frame_size_enum *fse);
	int (*enum_frame_interval)(struct v4l2_subdev *sd,
				   struct v4l2_subdev_state *state,
				   struct v4l2_subdev_frame_interval_enum *fie);
	int (*get_fmt)(struct v4l2_subdev *sd,
		       struct v4l2_subdev_state *state,
		       struct v4l2_subdev_format *format);
	int (*set_fmt)(struct v4l2_subdev *sd,
		       struct v4l2_subdev_state *state,
		       struct v4l2_subdev_format *format);
	int (*get_selection)(struct v4l2_subdev *sd,
			     struct v4l2_subdev_state *state,
			     struct v4l2_subdev_selection *sel);
	int (*set_selection)(struct v4l2_subdev *sd,
			     struct v4l2_subdev_state *state,
			     struct v4l2_subdev_selection *sel);
	int (*get_edid)(struct v4l2_subdev *sd, struct v4l2_edid *edid);
	int (*set_edid)(struct v4l2_subdev *sd, struct v4l2_edid *edid);
	int (*dv_timings_cap)(struct v4l2_subdev *sd,
			      struct v4l2_dv_timings_cap *cap);
	int (*enum_dv_timings)(struct v4l2_subdev *sd,
			       struct v4l2_enum_dv_timings *timings);
#ifdef CONFIG_MEDIA_CONTROLLER
	int (*link_validate)(struct v4l2_subdev *sd, struct media_link *link,
			     struct v4l2_subdev_format *source_fmt,
			     struct v4l2_subdev_format *sink_fmt);
#endif 
	int (*get_frame_desc)(struct v4l2_subdev *sd, unsigned int pad,
			      struct v4l2_mbus_frame_desc *fd);
	int (*set_frame_desc)(struct v4l2_subdev *sd, unsigned int pad,
			      struct v4l2_mbus_frame_desc *fd);
	int (*get_mbus_config)(struct v4l2_subdev *sd, unsigned int pad,
			       struct v4l2_mbus_config *config);
	int (*set_routing)(struct v4l2_subdev *sd,
			   struct v4l2_subdev_state *state,
			   enum v4l2_subdev_format_whence which,
			   struct v4l2_subdev_krouting *route);
	int (*enable_streams)(struct v4l2_subdev *sd,
			      struct v4l2_subdev_state *state, u32 pad,
			      u64 streams_mask);
	int (*disable_streams)(struct v4l2_subdev *sd,
			       struct v4l2_subdev_state *state, u32 pad,
			       u64 streams_mask);
};


struct v4l2_subdev_ops {
	const struct v4l2_subdev_core_ops	*core;
	const struct v4l2_subdev_tuner_ops	*tuner;
	const struct v4l2_subdev_audio_ops	*audio;
	const struct v4l2_subdev_video_ops	*video;
	const struct v4l2_subdev_vbi_ops	*vbi;
	const struct v4l2_subdev_ir_ops		*ir;
	const struct v4l2_subdev_sensor_ops	*sensor;
	const struct v4l2_subdev_pad_ops	*pad;
};


struct v4l2_subdev_internal_ops {
	int (*registered)(struct v4l2_subdev *sd);
	void (*unregistered)(struct v4l2_subdev *sd);
	int (*open)(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh);
	int (*close)(struct v4l2_subdev *sd, struct v4l2_subdev_fh *fh);
	void (*release)(struct v4l2_subdev *sd);
};

#define V4L2_SUBDEV_NAME_SIZE 32


#define V4L2_SUBDEV_FL_IS_I2C			(1U << 0)

#define V4L2_SUBDEV_FL_IS_SPI			(1U << 1)

#define V4L2_SUBDEV_FL_HAS_DEVNODE		(1U << 2)

#define V4L2_SUBDEV_FL_HAS_EVENTS		(1U << 3)

#define V4L2_SUBDEV_FL_STREAMS			(1U << 4)

struct regulator_bulk_data;


struct v4l2_subdev_platform_data {
	struct regulator_bulk_data *regulators;
	int num_regulators;

	void *host_priv;
};


struct v4l2_subdev {
#if defined(CONFIG_MEDIA_CONTROLLER)
	struct media_entity entity;
#endif
	struct list_head list;
	struct module *owner;
	bool owner_v4l2_dev;
	u32 flags;
	struct v4l2_device *v4l2_dev;
	const struct v4l2_subdev_ops *ops;
	const struct v4l2_subdev_internal_ops *internal_ops;
	struct v4l2_ctrl_handler *ctrl_handler;
	char name[V4L2_SUBDEV_NAME_SIZE];
	u32 grp_id;
	void *dev_priv;
	void *host_priv;
	struct video_device *devnode;
	struct device *dev;
	struct fwnode_handle *fwnode;
	struct list_head async_list;
	struct list_head async_subdev_endpoint_list;
	struct v4l2_async_notifier *subdev_notifier;
	struct list_head asc_list;
	struct v4l2_subdev_platform_data *pdata;
	struct mutex *state_lock;

	

	struct led_classdev *privacy_led;

	
	struct v4l2_subdev_state *active_state;
	u64 enabled_streams;
};



#define media_entity_to_v4l2_subdev(ent)				\
({									\
	typeof(ent) __me_sd_ent = (ent);				\
									\
	__me_sd_ent ?							\
		container_of(__me_sd_ent, struct v4l2_subdev, entity) :	\
		NULL;							\
})


#define vdev_to_v4l2_subdev(vdev) \
	((struct v4l2_subdev *)video_get_drvdata(vdev))


struct v4l2_subdev_fh {
	struct v4l2_fh vfh;
	struct module *owner;
#if defined(CONFIG_VIDEO_V4L2_SUBDEV_API)
	struct v4l2_subdev_state *state;
	u64 client_caps;
#endif
};


#define to_v4l2_subdev_fh(fh)	\
	container_of(fh, struct v4l2_subdev_fh, vfh)

#if defined(CONFIG_VIDEO_V4L2_SUBDEV_API)


static inline struct v4l2_mbus_framefmt *
v4l2_subdev_get_pad_format(struct v4l2_subdev *sd,
			   struct v4l2_subdev_state *state,
			   unsigned int pad)
{
	if (WARN_ON(!state))
		return NULL;
	if (WARN_ON(pad >= sd->entity.num_pads))
		pad = 0;
	return &state->pads[pad].try_fmt;
}


static inline struct v4l2_rect *
v4l2_subdev_get_pad_crop(struct v4l2_subdev *sd,
			 struct v4l2_subdev_state *state,
			 unsigned int pad)
{
	if (WARN_ON(!state))
		return NULL;
	if (WARN_ON(pad >= sd->entity.num_pads))
		pad = 0;
	return &state->pads[pad].try_crop;
}


static inline struct v4l2_rect *
v4l2_subdev_get_pad_compose(struct v4l2_subdev *sd,
			    struct v4l2_subdev_state *state,
			    unsigned int pad)
{
	if (WARN_ON(!state))
		return NULL;
	if (WARN_ON(pad >= sd->entity.num_pads))
		pad = 0;
	return &state->pads[pad].try_compose;
}


#define v4l2_subdev_get_try_format(sd, state, pad) \
	v4l2_subdev_get_pad_format(sd, state, pad)

#define v4l2_subdev_get_try_crop(sd, state, pad) \
	v4l2_subdev_get_pad_crop(sd, state, pad)

#define v4l2_subdev_get_try_compose(sd, state, pad) \
	v4l2_subdev_get_pad_compose(sd, state, pad)

#endif 

extern const struct v4l2_file_operations v4l2_subdev_fops;


static inline void v4l2_set_subdevdata(struct v4l2_subdev *sd, void *p)
{
	sd->dev_priv = p;
}


static inline void *v4l2_get_subdevdata(const struct v4l2_subdev *sd)
{
	return sd->dev_priv;
}


static inline void v4l2_set_subdev_hostdata(struct v4l2_subdev *sd, void *p)
{
	sd->host_priv = p;
}


static inline void *v4l2_get_subdev_hostdata(const struct v4l2_subdev *sd)
{
	return sd->host_priv;
}

#ifdef CONFIG_MEDIA_CONTROLLER


int v4l2_subdev_get_fwnode_pad_1_to_1(struct media_entity *entity,
				      struct fwnode_endpoint *endpoint);


int v4l2_subdev_link_validate_default(struct v4l2_subdev *sd,
				      struct media_link *link,
				      struct v4l2_subdev_format *source_fmt,
				      struct v4l2_subdev_format *sink_fmt);


int v4l2_subdev_link_validate(struct media_link *link);


bool v4l2_subdev_has_pad_interdep(struct media_entity *entity,
				  unsigned int pad0, unsigned int pad1);


struct v4l2_subdev_state *__v4l2_subdev_state_alloc(struct v4l2_subdev *sd,
						    const char *lock_name,
						    struct lock_class_key *key);


void __v4l2_subdev_state_free(struct v4l2_subdev_state *state);


#define v4l2_subdev_init_finalize(sd)                                          \
	({                                                                     \
		static struct lock_class_key __key;                            \
		const char *name = KBUILD_BASENAME                             \
			":" __stringify(__LINE__) ":sd->active_state->lock";   \
		__v4l2_subdev_init_finalize(sd, name, &__key);                 \
	})

int __v4l2_subdev_init_finalize(struct v4l2_subdev *sd, const char *name,
				struct lock_class_key *key);


void v4l2_subdev_cleanup(struct v4l2_subdev *sd);


static inline void v4l2_subdev_lock_state(struct v4l2_subdev_state *state)
{
	mutex_lock(state->lock);
}


static inline void v4l2_subdev_unlock_state(struct v4l2_subdev_state *state)
{
	mutex_unlock(state->lock);
}


static inline struct v4l2_subdev_state *
v4l2_subdev_get_unlocked_active_state(struct v4l2_subdev *sd)
{
	if (sd->active_state)
		lockdep_assert_not_held(sd->active_state->lock);
	return sd->active_state;
}


static inline struct v4l2_subdev_state *
v4l2_subdev_get_locked_active_state(struct v4l2_subdev *sd)
{
	if (sd->active_state)
		lockdep_assert_held(sd->active_state->lock);
	return sd->active_state;
}


static inline struct v4l2_subdev_state *
v4l2_subdev_lock_and_get_active_state(struct v4l2_subdev *sd)
{
	if (sd->active_state)
		v4l2_subdev_lock_state(sd->active_state);
	return sd->active_state;
}

#if defined(CONFIG_VIDEO_V4L2_SUBDEV_API)


int v4l2_subdev_get_fmt(struct v4l2_subdev *sd, struct v4l2_subdev_state *state,
			struct v4l2_subdev_format *format);


int v4l2_subdev_set_routing(struct v4l2_subdev *sd,
			    struct v4l2_subdev_state *state,
			    const struct v4l2_subdev_krouting *routing);

struct v4l2_subdev_route *
__v4l2_subdev_next_active_route(const struct v4l2_subdev_krouting *routing,
				struct v4l2_subdev_route *route);


#define for_each_active_route(routing, route) \
	for ((route) = NULL;                  \
	     ((route) = __v4l2_subdev_next_active_route((routing), (route)));)


int v4l2_subdev_set_routing_with_fmt(struct v4l2_subdev *sd,
				     struct v4l2_subdev_state *state,
				     const struct v4l2_subdev_krouting *routing,
				     const struct v4l2_mbus_framefmt *fmt);


struct v4l2_mbus_framefmt *
v4l2_subdev_state_get_stream_format(struct v4l2_subdev_state *state,
				    unsigned int pad, u32 stream);


struct v4l2_rect *
v4l2_subdev_state_get_stream_crop(struct v4l2_subdev_state *state,
				  unsigned int pad, u32 stream);


struct v4l2_rect *
v4l2_subdev_state_get_stream_compose(struct v4l2_subdev_state *state,
				     unsigned int pad, u32 stream);


int v4l2_subdev_routing_find_opposite_end(const struct v4l2_subdev_krouting *routing,
					  u32 pad, u32 stream, u32 *other_pad,
					  u32 *other_stream);


struct v4l2_mbus_framefmt *
v4l2_subdev_state_get_opposite_stream_format(struct v4l2_subdev_state *state,
					     u32 pad, u32 stream);


u64 v4l2_subdev_state_xlate_streams(const struct v4l2_subdev_state *state,
				    u32 pad0, u32 pad1, u64 *streams);


enum v4l2_subdev_routing_restriction {
	V4L2_SUBDEV_ROUTING_NO_1_TO_N = BIT(0),
	V4L2_SUBDEV_ROUTING_NO_N_TO_1 = BIT(1),
	V4L2_SUBDEV_ROUTING_NO_SINK_STREAM_MIX = BIT(2),
	V4L2_SUBDEV_ROUTING_NO_SOURCE_STREAM_MIX = BIT(3),
	V4L2_SUBDEV_ROUTING_NO_SINK_MULTIPLEXING = BIT(4),
	V4L2_SUBDEV_ROUTING_NO_SOURCE_MULTIPLEXING = BIT(5),
	V4L2_SUBDEV_ROUTING_ONLY_1_TO_1 =
		V4L2_SUBDEV_ROUTING_NO_1_TO_N |
		V4L2_SUBDEV_ROUTING_NO_N_TO_1,
	V4L2_SUBDEV_ROUTING_NO_STREAM_MIX =
		V4L2_SUBDEV_ROUTING_NO_SINK_STREAM_MIX |
		V4L2_SUBDEV_ROUTING_NO_SOURCE_STREAM_MIX,
	V4L2_SUBDEV_ROUTING_NO_MULTIPLEXING =
		V4L2_SUBDEV_ROUTING_NO_SINK_MULTIPLEXING |
		V4L2_SUBDEV_ROUTING_NO_SOURCE_MULTIPLEXING,
};


int v4l2_subdev_routing_validate(struct v4l2_subdev *sd,
				 const struct v4l2_subdev_krouting *routing,
				 enum v4l2_subdev_routing_restriction disallow);


int v4l2_subdev_enable_streams(struct v4l2_subdev *sd, u32 pad,
			       u64 streams_mask);


int v4l2_subdev_disable_streams(struct v4l2_subdev *sd, u32 pad,
				u64 streams_mask);


int v4l2_subdev_s_stream_helper(struct v4l2_subdev *sd, int enable);

#endif 

#endif 


void v4l2_subdev_init(struct v4l2_subdev *sd,
		      const struct v4l2_subdev_ops *ops);

extern const struct v4l2_subdev_ops v4l2_subdev_call_wrappers;


#define v4l2_subdev_call(sd, o, f, args...)				\
	({								\
		struct v4l2_subdev *__sd = (sd);			\
		int __result;						\
		if (!__sd)						\
			__result = -ENODEV;				\
		else if (!(__sd->ops->o && __sd->ops->o->f))		\
			__result = -ENOIOCTLCMD;			\
		else if (v4l2_subdev_call_wrappers.o &&			\
			 v4l2_subdev_call_wrappers.o->f)		\
			__result = v4l2_subdev_call_wrappers.o->f(	\
							__sd, ##args);	\
		else							\
			__result = __sd->ops->o->f(__sd, ##args);	\
		__result;						\
	})


#define v4l2_subdev_call_state_active(sd, o, f, args...)		\
	({								\
		int __result;						\
		struct v4l2_subdev_state *state;			\
		state = v4l2_subdev_get_unlocked_active_state(sd);	\
		if (state)						\
			v4l2_subdev_lock_state(state);			\
		__result = v4l2_subdev_call(sd, o, f, state, ##args);	\
		if (state)						\
			v4l2_subdev_unlock_state(state);		\
		__result;						\
	})


#define v4l2_subdev_call_state_try(sd, o, f, args...)                 \
	({                                                            \
		int __result;                                         \
		static struct lock_class_key __key;                   \
		const char *name = KBUILD_BASENAME                    \
			":" __stringify(__LINE__) ":state->lock";     \
		struct v4l2_subdev_state *state =                     \
			__v4l2_subdev_state_alloc(sd, name, &__key);  \
		v4l2_subdev_lock_state(state);                        \
		__result = v4l2_subdev_call(sd, o, f, state, ##args); \
		v4l2_subdev_unlock_state(state);                      \
		__v4l2_subdev_state_free(state);                      \
		__result;                                             \
	})


#define v4l2_subdev_has_op(sd, o, f) \
	((sd)->ops->o && (sd)->ops->o->f)


void v4l2_subdev_notify_event(struct v4l2_subdev *sd,
			      const struct v4l2_event *ev);

#endif 
