/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _IIO_INKERN_CONSUMER_H_
#define _IIO_INKERN_CONSUMER_H_

#include <linux/types.h>
#include <linux/iio/types.h>

struct iio_dev;
struct iio_chan_spec;
struct device;
struct fwnode_handle;


struct iio_channel {
	struct iio_dev *indio_dev;
	const struct iio_chan_spec *channel;
	void *data;
};


struct iio_channel *iio_channel_get(struct device *dev,
				    const char *consumer_channel);


void iio_channel_release(struct iio_channel *chan);


struct iio_channel *devm_iio_channel_get(struct device *dev,
					 const char *consumer_channel);

struct iio_channel *iio_channel_get_all(struct device *dev);


void iio_channel_release_all(struct iio_channel *chan);


struct iio_channel *devm_iio_channel_get_all(struct device *dev);


struct iio_channel *fwnode_iio_channel_get_by_name(struct fwnode_handle *fwnode,
						   const char *name);


struct iio_channel *devm_fwnode_iio_channel_get_by_name(struct device *dev,
							struct fwnode_handle *fwnode,
							const char *consumer_channel);

struct iio_cb_buffer;

struct iio_cb_buffer *iio_channel_get_all_cb(struct device *dev,
					     int (*cb)(const void *data,
						       void *private),
					     void *private);

int iio_channel_cb_set_buffer_watermark(struct iio_cb_buffer *cb_buffer,
					size_t watermark);


void iio_channel_release_all_cb(struct iio_cb_buffer *cb_buffer);


int iio_channel_start_all_cb(struct iio_cb_buffer *cb_buff);


void iio_channel_stop_all_cb(struct iio_cb_buffer *cb_buff);


struct iio_channel
*iio_channel_cb_get_channels(const struct iio_cb_buffer *cb_buffer);


struct iio_dev
*iio_channel_cb_get_iio_dev(const struct iio_cb_buffer *cb_buffer);


int iio_read_channel_raw(struct iio_channel *chan,
			 int *val);


int iio_read_channel_average_raw(struct iio_channel *chan, int *val);


int iio_read_channel_processed(struct iio_channel *chan, int *val);


int iio_read_channel_processed_scale(struct iio_channel *chan, int *val,
				     unsigned int scale);


int iio_write_channel_attribute(struct iio_channel *chan, int val,
				int val2, enum iio_chan_info_enum attribute);


int iio_read_channel_attribute(struct iio_channel *chan, int *val,
			       int *val2, enum iio_chan_info_enum attribute);


int iio_write_channel_raw(struct iio_channel *chan, int val);


int iio_read_max_channel_raw(struct iio_channel *chan, int *val);


int iio_read_min_channel_raw(struct iio_channel *chan, int *val);


int iio_read_avail_channel_raw(struct iio_channel *chan,
			       const int **vals, int *length);


int iio_read_avail_channel_attribute(struct iio_channel *chan,
				     const int **vals, int *type, int *length,
				     enum iio_chan_info_enum attribute);


int iio_get_channel_type(struct iio_channel *channel,
			 enum iio_chan_type *type);


int iio_read_channel_offset(struct iio_channel *chan, int *val,
			   int *val2);


int iio_read_channel_scale(struct iio_channel *chan, int *val,
			   int *val2);


int iio_convert_raw_to_processed(struct iio_channel *chan, int raw,
	int *processed, unsigned int scale);


unsigned int iio_get_channel_ext_info_count(struct iio_channel *chan);


ssize_t iio_read_channel_ext_info(struct iio_channel *chan,
				  const char *attr, char *buf);


ssize_t iio_write_channel_ext_info(struct iio_channel *chan, const char *attr,
				   const char *buf, size_t len);

#endif
