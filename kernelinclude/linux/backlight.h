/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_BACKLIGHT_H
#define _LINUX_BACKLIGHT_H

#include <linux/device.h>
#include <linux/fb.h>
#include <linux/mutex.h>
#include <linux/notifier.h>


enum backlight_update_reason {
	
	BACKLIGHT_UPDATE_HOTKEY,

	
	BACKLIGHT_UPDATE_SYSFS,
};


enum backlight_type {
	
	BACKLIGHT_RAW = 1,

	
	BACKLIGHT_PLATFORM,

	
	BACKLIGHT_FIRMWARE,

	
	BACKLIGHT_TYPE_MAX,
};


enum backlight_notification {
	
	BACKLIGHT_REGISTERED,

	
	BACKLIGHT_UNREGISTERED,
};


enum backlight_scale {
	
	BACKLIGHT_SCALE_UNKNOWN = 0,

	
	BACKLIGHT_SCALE_LINEAR,

	
	BACKLIGHT_SCALE_NON_LINEAR,
};

struct backlight_device;
struct fb_info;


struct backlight_ops {
	
	unsigned int options;

#define BL_CORE_SUSPENDRESUME	(1 << 0)

	
	int (*update_status)(struct backlight_device *);

	
	int (*get_brightness)(struct backlight_device *);

	
	int (*check_fb)(struct backlight_device *bd, struct fb_info *info);
};


struct backlight_properties {
	
	int brightness;

	
	int max_brightness;

	
	int power;

	
	int fb_blank;

	
	enum backlight_type type;

	
	unsigned int state;

#define BL_CORE_SUSPENDED	(1 << 0)	
#define BL_CORE_FBBLANK		(1 << 1)	

	
	enum backlight_scale scale;
};


struct backlight_device {
	
	struct backlight_properties props;

	
	struct mutex update_lock;

	
	struct mutex ops_lock;

	
	const struct backlight_ops *ops;

	
	struct notifier_block fb_notif;

	
	struct list_head entry;

	
	struct device dev;

	
	bool fb_bl_on[FB_MAX];

	
	int use_count;
};


static inline int backlight_update_status(struct backlight_device *bd)
{
	int ret = -ENOENT;

	mutex_lock(&bd->update_lock);
	if (bd->ops && bd->ops->update_status)
		ret = bd->ops->update_status(bd);
	mutex_unlock(&bd->update_lock);

	return ret;
}


static inline int backlight_enable(struct backlight_device *bd)
{
	if (!bd)
		return 0;

	bd->props.power = FB_BLANK_UNBLANK;
	bd->props.fb_blank = FB_BLANK_UNBLANK;
	bd->props.state &= ~BL_CORE_FBBLANK;

	return backlight_update_status(bd);
}


static inline int backlight_disable(struct backlight_device *bd)
{
	if (!bd)
		return 0;

	bd->props.power = FB_BLANK_POWERDOWN;
	bd->props.fb_blank = FB_BLANK_POWERDOWN;
	bd->props.state |= BL_CORE_FBBLANK;

	return backlight_update_status(bd);
}


static inline bool backlight_is_blank(const struct backlight_device *bd)
{
	return bd->props.power != FB_BLANK_UNBLANK ||
	       bd->props.fb_blank != FB_BLANK_UNBLANK ||
	       bd->props.state & (BL_CORE_SUSPENDED | BL_CORE_FBBLANK);
}


static inline int backlight_get_brightness(const struct backlight_device *bd)
{
	if (backlight_is_blank(bd))
		return 0;
	else
		return bd->props.brightness;
}

struct backlight_device *
backlight_device_register(const char *name, struct device *dev, void *devdata,
			  const struct backlight_ops *ops,
			  const struct backlight_properties *props);
struct backlight_device *
devm_backlight_device_register(struct device *dev, const char *name,
			       struct device *parent, void *devdata,
			       const struct backlight_ops *ops,
			       const struct backlight_properties *props);
void backlight_device_unregister(struct backlight_device *bd);
void devm_backlight_device_unregister(struct device *dev,
				      struct backlight_device *bd);
void backlight_force_update(struct backlight_device *bd,
			    enum backlight_update_reason reason);
int backlight_register_notifier(struct notifier_block *nb);
int backlight_unregister_notifier(struct notifier_block *nb);
struct backlight_device *backlight_device_get_by_name(const char *name);
struct backlight_device *backlight_device_get_by_type(enum backlight_type type);
int backlight_device_set_brightness(struct backlight_device *bd,
				    unsigned long brightness);

#define to_backlight_device(obj) container_of(obj, struct backlight_device, dev)


static inline void * bl_get_data(struct backlight_device *bl_dev)
{
	return dev_get_drvdata(&bl_dev->dev);
}

#ifdef CONFIG_OF
struct backlight_device *of_find_backlight_by_node(struct device_node *node);
#else
static inline struct backlight_device *
of_find_backlight_by_node(struct device_node *node)
{
	return NULL;
}
#endif

#if IS_ENABLED(CONFIG_BACKLIGHT_CLASS_DEVICE)
struct backlight_device *devm_of_find_backlight(struct device *dev);
#else
static inline struct backlight_device *
devm_of_find_backlight(struct device *dev)
{
	return NULL;
}
#endif

#endif
