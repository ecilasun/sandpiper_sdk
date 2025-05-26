/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef _INDUSTRIAL_IO_H_
#define _INDUSTRIAL_IO_H_

#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/iio/types.h>



struct fwnode_reference_args;

enum iio_shared_by {
	IIO_SEPARATE,
	IIO_SHARED_BY_TYPE,
	IIO_SHARED_BY_DIR,
	IIO_SHARED_BY_ALL
};

enum iio_endian {
	IIO_CPU,
	IIO_BE,
	IIO_LE,
};

struct iio_chan_spec;
struct iio_dev;


struct iio_chan_spec_ext_info {
	const char *name;
	enum iio_shared_by shared;
	ssize_t (*read)(struct iio_dev *, uintptr_t private,
			struct iio_chan_spec const *, char *buf);
	ssize_t (*write)(struct iio_dev *, uintptr_t private,
			 struct iio_chan_spec const *, const char *buf,
			 size_t len);
	uintptr_t private;
};


struct iio_enum {
	const char * const *items;
	unsigned int num_items;
	int (*set)(struct iio_dev *, const struct iio_chan_spec *, unsigned int);
	int (*get)(struct iio_dev *, const struct iio_chan_spec *);
};

ssize_t iio_enum_available_read(struct iio_dev *indio_dev,
	uintptr_t priv, const struct iio_chan_spec *chan, char *buf);
ssize_t iio_enum_read(struct iio_dev *indio_dev,
	uintptr_t priv, const struct iio_chan_spec *chan, char *buf);
ssize_t iio_enum_write(struct iio_dev *indio_dev,
	uintptr_t priv, const struct iio_chan_spec *chan, const char *buf,
	size_t len);


#define IIO_ENUM(_name, _shared, _e) \
{ \
	.name = (_name), \
	.shared = (_shared), \
	.read = iio_enum_read, \
	.write = iio_enum_write, \
	.private = (uintptr_t)(_e), \
}


#define IIO_ENUM_AVAILABLE(_name, _shared, _e) \
{ \
	.name = (_name "_available"), \
	.shared = _shared, \
	.read = iio_enum_available_read, \
	.private = (uintptr_t)(_e), \
}


struct iio_mount_matrix {
	const char *rotation[9];
};

ssize_t iio_show_mount_matrix(struct iio_dev *indio_dev, uintptr_t priv,
			      const struct iio_chan_spec *chan, char *buf);
int iio_read_mount_matrix(struct device *dev, struct iio_mount_matrix *matrix);

typedef const struct iio_mount_matrix *
	(iio_get_mount_matrix_t)(const struct iio_dev *indio_dev,
				 const struct iio_chan_spec *chan);


#define IIO_MOUNT_MATRIX(_shared, _get) \
{ \
	.name = "mount_matrix", \
	.shared = (_shared), \
	.read = iio_show_mount_matrix, \
	.private = (uintptr_t)(_get), \
}


struct iio_event_spec {
	enum iio_event_type type;
	enum iio_event_direction dir;
	unsigned long mask_separate;
	unsigned long mask_shared_by_type;
	unsigned long mask_shared_by_dir;
	unsigned long mask_shared_by_all;
};


struct iio_chan_spec {
	enum iio_chan_type	type;
	int			channel;
	int			channel2;
	unsigned long		address;
	int			scan_index;
	struct {
		char	sign;
		u8	realbits;
		u8	storagebits;
		u8	shift;
		u8	repeat;
		enum iio_endian endianness;
	} scan_type;
	long			info_mask_separate;
	long			info_mask_separate_available;
	long			info_mask_shared_by_type;
	long			info_mask_shared_by_type_available;
	long			info_mask_shared_by_dir;
	long			info_mask_shared_by_dir_available;
	long			info_mask_shared_by_all;
	long			info_mask_shared_by_all_available;
	const struct iio_event_spec *event_spec;
	unsigned int		num_event_specs;
	const struct iio_chan_spec_ext_info *ext_info;
	const char		*extend_name;
	const char		*datasheet_name;
	unsigned		modified:1;
	unsigned		indexed:1;
	unsigned		output:1;
	unsigned		differential:1;
};



static inline bool iio_channel_has_info(const struct iio_chan_spec *chan,
	enum iio_chan_info_enum type)
{
	return (chan->info_mask_separate & BIT(type)) |
		(chan->info_mask_shared_by_type & BIT(type)) |
		(chan->info_mask_shared_by_dir & BIT(type)) |
		(chan->info_mask_shared_by_all & BIT(type));
}


static inline bool iio_channel_has_available(const struct iio_chan_spec *chan,
					     enum iio_chan_info_enum type)
{
	return (chan->info_mask_separate_available & BIT(type)) |
		(chan->info_mask_shared_by_type_available & BIT(type)) |
		(chan->info_mask_shared_by_dir_available & BIT(type)) |
		(chan->info_mask_shared_by_all_available & BIT(type));
}

#define IIO_CHAN_SOFT_TIMESTAMP(_si) {					\
	.type = IIO_TIMESTAMP,						\
	.channel = -1,							\
	.scan_index = _si,						\
	.scan_type = {							\
		.sign = 's',						\
		.realbits = 64,					\
		.storagebits = 64,					\
		},							\
}

s64 iio_get_time_ns(const struct iio_dev *indio_dev);


#define INDIO_DIRECT_MODE		0x01
#define INDIO_BUFFER_TRIGGERED		0x02
#define INDIO_BUFFER_SOFTWARE		0x04
#define INDIO_BUFFER_HARDWARE		0x08
#define INDIO_EVENT_TRIGGERED		0x10
#define INDIO_HARDWARE_TRIGGERED	0x20

#define INDIO_ALL_BUFFER_MODES					\
	(INDIO_BUFFER_TRIGGERED | INDIO_BUFFER_HARDWARE | INDIO_BUFFER_SOFTWARE)

#define INDIO_ALL_TRIGGERED_MODES	\
	(INDIO_BUFFER_TRIGGERED		\
	 | INDIO_EVENT_TRIGGERED	\
	 | INDIO_HARDWARE_TRIGGERED)

#define INDIO_MAX_RAW_ELEMENTS		4

struct iio_val_int_plus_micro {
	int integer;
	int micro;
};

struct iio_trigger; 


struct iio_info {
	const struct attribute_group	*event_attrs;
	const struct attribute_group	*attrs;

	int (*read_raw)(struct iio_dev *indio_dev,
			struct iio_chan_spec const *chan,
			int *val,
			int *val2,
			long mask);

	int (*read_raw_multi)(struct iio_dev *indio_dev,
			struct iio_chan_spec const *chan,
			int max_len,
			int *vals,
			int *val_len,
			long mask);

	int (*read_avail)(struct iio_dev *indio_dev,
			  struct iio_chan_spec const *chan,
			  const int **vals,
			  int *type,
			  int *length,
			  long mask);

	int (*write_raw)(struct iio_dev *indio_dev,
			 struct iio_chan_spec const *chan,
			 int val,
			 int val2,
			 long mask);

	int (*read_label)(struct iio_dev *indio_dev,
			 struct iio_chan_spec const *chan,
			 char *label);

	int (*write_raw_get_fmt)(struct iio_dev *indio_dev,
			 struct iio_chan_spec const *chan,
			 long mask);

	int (*read_event_config)(struct iio_dev *indio_dev,
				 const struct iio_chan_spec *chan,
				 enum iio_event_type type,
				 enum iio_event_direction dir);

	int (*write_event_config)(struct iio_dev *indio_dev,
				  const struct iio_chan_spec *chan,
				  enum iio_event_type type,
				  enum iio_event_direction dir,
				  int state);

	int (*read_event_value)(struct iio_dev *indio_dev,
				const struct iio_chan_spec *chan,
				enum iio_event_type type,
				enum iio_event_direction dir,
				enum iio_event_info info, int *val, int *val2);

	int (*write_event_value)(struct iio_dev *indio_dev,
				 const struct iio_chan_spec *chan,
				 enum iio_event_type type,
				 enum iio_event_direction dir,
				 enum iio_event_info info, int val, int val2);

	int (*validate_trigger)(struct iio_dev *indio_dev,
				struct iio_trigger *trig);
	int (*update_scan_mode)(struct iio_dev *indio_dev,
				const unsigned long *scan_mask);
	int (*debugfs_reg_access)(struct iio_dev *indio_dev,
				  unsigned reg, unsigned writeval,
				  unsigned *readval);
	int (*fwnode_xlate)(struct iio_dev *indio_dev,
			    const struct fwnode_reference_args *iiospec);
	int (*hwfifo_set_watermark)(struct iio_dev *indio_dev, unsigned val);
	int (*hwfifo_flush_to_buffer)(struct iio_dev *indio_dev,
				      unsigned count);
};


struct iio_buffer_setup_ops {
	int (*preenable)(struct iio_dev *);
	int (*postenable)(struct iio_dev *);
	int (*predisable)(struct iio_dev *);
	int (*postdisable)(struct iio_dev *);
	bool (*validate_scan_mask)(struct iio_dev *indio_dev,
				   const unsigned long *scan_mask);
};


struct iio_dev {
	int				modes;
	struct device			dev;

	struct iio_buffer		*buffer;
	int				scan_bytes;

	const unsigned long		*available_scan_masks;
	unsigned			masklength;
	const unsigned long		*active_scan_mask;
	bool				scan_timestamp;
	struct iio_trigger		*trig;
	struct iio_poll_func		*pollfunc;
	struct iio_poll_func		*pollfunc_event;

	struct iio_chan_spec const	*channels;
	int				num_channels;

	const char			*name;
	const char			*label;
	const struct iio_info		*info;
	const struct iio_buffer_setup_ops	*setup_ops;

	void				*priv;
};

int iio_device_id(struct iio_dev *indio_dev);
int iio_device_get_current_mode(struct iio_dev *indio_dev);
bool iio_buffer_enabled(struct iio_dev *indio_dev);

const struct iio_chan_spec
*iio_find_channel_from_si(struct iio_dev *indio_dev, int si);

#define iio_device_register(indio_dev) \
	__iio_device_register((indio_dev), THIS_MODULE)
int __iio_device_register(struct iio_dev *indio_dev, struct module *this_mod);
void iio_device_unregister(struct iio_dev *indio_dev);

#define devm_iio_device_register(dev, indio_dev) \
	__devm_iio_device_register((dev), (indio_dev), THIS_MODULE)
int __devm_iio_device_register(struct device *dev, struct iio_dev *indio_dev,
			       struct module *this_mod);
int iio_push_event(struct iio_dev *indio_dev, u64 ev_code, s64 timestamp);
int iio_device_claim_direct_mode(struct iio_dev *indio_dev);
void iio_device_release_direct_mode(struct iio_dev *indio_dev);
int iio_device_claim_buffer_mode(struct iio_dev *indio_dev);
void iio_device_release_buffer_mode(struct iio_dev *indio_dev);

extern struct bus_type iio_bus_type;


static inline void iio_device_put(struct iio_dev *indio_dev)
{
	if (indio_dev)
		put_device(&indio_dev->dev);
}

clockid_t iio_device_get_clock(const struct iio_dev *indio_dev);
int iio_device_set_clock(struct iio_dev *indio_dev, clockid_t clock_id);


static inline struct iio_dev *dev_to_iio_dev(struct device *dev)
{
	return container_of(dev, struct iio_dev, dev);
}


static inline struct iio_dev *iio_device_get(struct iio_dev *indio_dev)
{
	return indio_dev ? dev_to_iio_dev(get_device(&indio_dev->dev)) : NULL;
}


static inline void iio_device_set_parent(struct iio_dev *indio_dev,
					 struct device *parent)
{
	indio_dev->dev.parent = parent;
}


static inline void iio_device_set_drvdata(struct iio_dev *indio_dev, void *data)
{
	dev_set_drvdata(&indio_dev->dev, data);
}


static inline void *iio_device_get_drvdata(const struct iio_dev *indio_dev)
{
	return dev_get_drvdata(&indio_dev->dev);
}


#define IIO_DMA_MINALIGN ARCH_DMA_MINALIGN
struct iio_dev *iio_device_alloc(struct device *parent, int sizeof_priv);


static inline void *iio_priv(const struct iio_dev *indio_dev)
{
	return indio_dev->priv;
}

void iio_device_free(struct iio_dev *indio_dev);
struct iio_dev *devm_iio_device_alloc(struct device *parent, int sizeof_priv);

#define devm_iio_trigger_alloc(parent, fmt, ...) \
	__devm_iio_trigger_alloc((parent), THIS_MODULE, (fmt), ##__VA_ARGS__)
__printf(3, 4)
struct iio_trigger *__devm_iio_trigger_alloc(struct device *parent,
					     struct module *this_mod,
					     const char *fmt, ...);

#if defined(CONFIG_DEBUG_FS)
struct dentry *iio_get_debugfs_dentry(struct iio_dev *indio_dev);
#else
static inline struct dentry *iio_get_debugfs_dentry(struct iio_dev *indio_dev)
{
	return NULL;
}
#endif

ssize_t iio_format_value(char *buf, unsigned int type, int size, int *vals);

int iio_str_to_fixpoint(const char *str, int fract_mult, int *integer,
	int *fract);


#define IIO_DEGREE_TO_RAD(deg) (((deg) * 314159ULL + 9000000ULL) / 18000000ULL)


#define IIO_RAD_TO_DEGREE(rad) \
	(((rad) * 18000000ULL + 314159ULL / 2) / 314159ULL)


#define IIO_G_TO_M_S_2(g) ((g) * 980665ULL / 100000ULL)


#define IIO_M_S_2_TO_G(ms2) (((ms2) * 100000ULL + 980665ULL / 2) / 980665ULL)

#endif 
