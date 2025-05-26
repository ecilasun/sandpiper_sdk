/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __LINUX_DEVM_HELPERS_H
#define __LINUX_DEVM_HELPERS_H



#include <linux/device.h>
#include <linux/workqueue.h>

static inline void devm_delayed_work_drop(void *res)
{
	cancel_delayed_work_sync(res);
}


static inline int devm_delayed_work_autocancel(struct device *dev,
					       struct delayed_work *w,
					       work_func_t worker)
{
	INIT_DELAYED_WORK(w, worker);
	return devm_add_action(dev, devm_delayed_work_drop, w);
}

static inline void devm_work_drop(void *res)
{
	cancel_work_sync(res);
}


static inline int devm_work_autocancel(struct device *dev,
				       struct work_struct *w,
				       work_func_t worker)
{
	INIT_WORK(w, worker);
	return devm_add_action(dev, devm_work_drop, w);
}

#endif
