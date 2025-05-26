/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef _LINUX_PM_WAKEUP_H
#define _LINUX_PM_WAKEUP_H

#ifndef _DEVICE_H_
# error "please don't include this file directly"
#endif

#include <linux/types.h>

struct wake_irq;


struct wakeup_source {
	const char 		*name;
	int			id;
	struct list_head	entry;
	spinlock_t		lock;
	struct wake_irq		*wakeirq;
	struct timer_list	timer;
	unsigned long		timer_expires;
	ktime_t total_time;
	ktime_t max_time;
	ktime_t last_time;
	ktime_t start_prevent_time;
	ktime_t prevent_sleep_time;
	unsigned long		event_count;
	unsigned long		active_count;
	unsigned long		relax_count;
	unsigned long		expire_count;
	unsigned long		wakeup_count;
	struct device		*dev;
	bool			active:1;
	bool			autosleep_enabled:1;
};

#define for_each_wakeup_source(ws) \
	for ((ws) = wakeup_sources_walk_start();	\
	     (ws);					\
	     (ws) = wakeup_sources_walk_next((ws)))

#ifdef CONFIG_PM_SLEEP



static inline bool device_can_wakeup(struct device *dev)
{
	return dev->power.can_wakeup;
}

static inline bool device_may_wakeup(struct device *dev)
{
	return dev->power.can_wakeup && !!dev->power.wakeup;
}

static inline bool device_wakeup_path(struct device *dev)
{
	return dev->power.wakeup_path;
}

static inline void device_set_wakeup_path(struct device *dev)
{
	dev->power.wakeup_path = true;
}


extern struct wakeup_source *wakeup_source_create(const char *name);
extern void wakeup_source_destroy(struct wakeup_source *ws);
extern void wakeup_source_add(struct wakeup_source *ws);
extern void wakeup_source_remove(struct wakeup_source *ws);
extern struct wakeup_source *wakeup_source_register(struct device *dev,
						    const char *name);
extern void wakeup_source_unregister(struct wakeup_source *ws);
extern int wakeup_sources_read_lock(void);
extern void wakeup_sources_read_unlock(int idx);
extern struct wakeup_source *wakeup_sources_walk_start(void);
extern struct wakeup_source *wakeup_sources_walk_next(struct wakeup_source *ws);
extern int device_wakeup_enable(struct device *dev);
extern int device_wakeup_disable(struct device *dev);
extern void device_set_wakeup_capable(struct device *dev, bool capable);
extern int device_set_wakeup_enable(struct device *dev, bool enable);
extern void __pm_stay_awake(struct wakeup_source *ws);
extern void pm_stay_awake(struct device *dev);
extern void __pm_relax(struct wakeup_source *ws);
extern void pm_relax(struct device *dev);
extern void pm_wakeup_ws_event(struct wakeup_source *ws, unsigned int msec, bool hard);
extern void pm_wakeup_dev_event(struct device *dev, unsigned int msec, bool hard);

#else 

static inline void device_set_wakeup_capable(struct device *dev, bool capable)
{
	dev->power.can_wakeup = capable;
}

static inline bool device_can_wakeup(struct device *dev)
{
	return dev->power.can_wakeup;
}

static inline struct wakeup_source *wakeup_source_create(const char *name)
{
	return NULL;
}

static inline void wakeup_source_destroy(struct wakeup_source *ws) {}

static inline void wakeup_source_add(struct wakeup_source *ws) {}

static inline void wakeup_source_remove(struct wakeup_source *ws) {}

static inline struct wakeup_source *wakeup_source_register(struct device *dev,
							   const char *name)
{
	return NULL;
}

static inline void wakeup_source_unregister(struct wakeup_source *ws) {}

static inline int device_wakeup_enable(struct device *dev)
{
	dev->power.should_wakeup = true;
	return 0;
}

static inline int device_wakeup_disable(struct device *dev)
{
	dev->power.should_wakeup = false;
	return 0;
}

static inline int device_set_wakeup_enable(struct device *dev, bool enable)
{
	dev->power.should_wakeup = enable;
	return 0;
}

static inline bool device_may_wakeup(struct device *dev)
{
	return dev->power.can_wakeup && dev->power.should_wakeup;
}

static inline bool device_wakeup_path(struct device *dev)
{
	return false;
}

static inline void device_set_wakeup_path(struct device *dev) {}

static inline void __pm_stay_awake(struct wakeup_source *ws) {}

static inline void pm_stay_awake(struct device *dev) {}

static inline void __pm_relax(struct wakeup_source *ws) {}

static inline void pm_relax(struct device *dev) {}

static inline void pm_wakeup_ws_event(struct wakeup_source *ws,
				      unsigned int msec, bool hard) {}

static inline void pm_wakeup_dev_event(struct device *dev, unsigned int msec,
				       bool hard) {}

#endif 

static inline bool device_awake_path(struct device *dev)
{
	return device_wakeup_path(dev);
}

static inline void device_set_awake_path(struct device *dev)
{
	device_set_wakeup_path(dev);
}

static inline void __pm_wakeup_event(struct wakeup_source *ws, unsigned int msec)
{
	return pm_wakeup_ws_event(ws, msec, false);
}

static inline void pm_wakeup_event(struct device *dev, unsigned int msec)
{
	return pm_wakeup_dev_event(dev, msec, false);
}

static inline void pm_wakeup_hard_event(struct device *dev)
{
	return pm_wakeup_dev_event(dev, 0, true);
}


static inline int device_init_wakeup(struct device *dev, bool enable)
{
	if (enable) {
		device_set_wakeup_capable(dev, true);
		return device_wakeup_enable(dev);
	} else {
		device_wakeup_disable(dev);
		device_set_wakeup_capable(dev, false);
		return 0;
	}
}

#endif 
