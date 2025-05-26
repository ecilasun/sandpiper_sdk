/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright 2022 HabanaLabs, Ltd.
 * All Rights Reserved.
 *
 */

#ifndef DRM_ACCEL_H_
#define DRM_ACCEL_H_

#include <drm/drm_file.h>

#define ACCEL_MAJOR		261
#define ACCEL_MAX_MINORS	256


#define DRM_ACCEL_FOPS \
	.open		= accel_open,\
	.release	= drm_release,\
	.unlocked_ioctl	= drm_ioctl,\
	.compat_ioctl	= drm_compat_ioctl,\
	.poll		= drm_poll,\
	.read		= drm_read,\
	.llseek		= noop_llseek, \
	.mmap		= drm_gem_mmap


#define DEFINE_DRM_ACCEL_FOPS(name) \
	static const struct file_operations name = {\
		.owner		= THIS_MODULE,\
		DRM_ACCEL_FOPS,\
	}

#if IS_ENABLED(CONFIG_DRM_ACCEL)

void accel_core_exit(void);
int accel_core_init(void);
void accel_minor_remove(int index);
int accel_minor_alloc(void);
void accel_minor_replace(struct drm_minor *minor, int index);
void accel_set_device_instance_params(struct device *kdev, int index);
int accel_open(struct inode *inode, struct file *filp);
void accel_debugfs_init(struct drm_minor *minor, int minor_id);

#else

static inline void accel_core_exit(void)
{
}

static inline int __init accel_core_init(void)
{
	
	return 0;
}

static inline void accel_minor_remove(int index)
{
}

static inline int accel_minor_alloc(void)
{
	return -EOPNOTSUPP;
}

static inline void accel_minor_replace(struct drm_minor *minor, int index)
{
}

static inline void accel_set_device_instance_params(struct device *kdev, int index)
{
}

static inline void accel_debugfs_init(struct drm_minor *minor, int minor_id)
{
}

#endif 

#endif 
