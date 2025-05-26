/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _INDUSTRIAL_IO_OPAQUE_H_
#define _INDUSTRIAL_IO_OPAQUE_H_


struct iio_dev_opaque {
	struct iio_dev			indio_dev;
	int				currentmode;
	int				id;
	struct module			*driver_module;
	struct mutex			mlock;
	struct lock_class_key		mlock_key;
	struct mutex			info_exist_lock;
	bool				trig_readonly;
	struct iio_event_interface	*event_interface;
	struct iio_buffer		**attached_buffers;
	unsigned int			attached_buffers_cnt;
	struct iio_ioctl_handler	*buffer_ioctl_handler;
	struct list_head		buffer_list;
	struct list_head		channel_attr_list;
	struct attribute_group		chan_attr_group;
	struct list_head		ioctl_handlers;
	const struct attribute_group	**groups;
	int				groupcounter;
	struct attribute_group		legacy_scan_el_group;
	struct attribute_group		legacy_buffer_group;
	void				*bounce_buffer;
	size_t				bounce_buffer_size;

	unsigned int			scan_index_timestamp;
	clockid_t			clock_id;
	struct cdev			chrdev;
	unsigned long			flags;

#if defined(CONFIG_DEBUG_FS)
	struct dentry			*debugfs_dentry;
	unsigned			cached_reg_addr;
	char				read_buf[20];
	unsigned int			read_buf_len;
#endif
};

#define to_iio_dev_opaque(_indio_dev)		\
	container_of((_indio_dev), struct iio_dev_opaque, indio_dev)

#endif
