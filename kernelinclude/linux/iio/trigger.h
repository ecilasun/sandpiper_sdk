/* SPDX-License-Identifier: GPL-2.0-only */

#include <linux/irq.h>
#include <linux/module.h>
#include <linux/atomic.h>

#ifndef _IIO_TRIGGER_H_
#define _IIO_TRIGGER_H_

#ifdef CONFIG_IIO_TRIGGER
struct iio_subirq {
	bool enabled;
};

struct iio_dev;
struct iio_trigger;


struct iio_trigger_ops {
	int (*set_trigger_state)(struct iio_trigger *trig, bool state);
	void (*reenable)(struct iio_trigger *trig);
	int (*validate_device)(struct iio_trigger *trig,
			       struct iio_dev *indio_dev);
};



struct iio_trigger {
	const struct iio_trigger_ops	*ops;
	struct module			*owner;
	int				id;
	const char			*name;
	struct device			dev;

	struct list_head		list;
	struct list_head		alloc_list;
	atomic_t			use_count;

	struct irq_chip			subirq_chip;
	int				subirq_base;

	struct iio_subirq subirqs[CONFIG_IIO_CONSUMERS_PER_TRIGGER];
	unsigned long pool[BITS_TO_LONGS(CONFIG_IIO_CONSUMERS_PER_TRIGGER)];
	struct mutex			pool_lock;
	bool				attached_own_device;
	struct work_struct		reenable_work;
};


static inline struct iio_trigger *to_iio_trigger(struct device *d)
{
	return container_of(d, struct iio_trigger, dev);
}

static inline void iio_trigger_put(struct iio_trigger *trig)
{
	module_put(trig->owner);
	put_device(&trig->dev);
}

static inline struct iio_trigger *iio_trigger_get(struct iio_trigger *trig)
{
	get_device(&trig->dev);

	WARN_ONCE(list_empty(&trig->list),
		  "Getting non-registered iio trigger %s is prohibited\n",
		  trig->name);

	__module_get(trig->owner);

	return trig;
}


static inline void iio_trigger_set_drvdata(struct iio_trigger *trig, void *data)
{
	dev_set_drvdata(&trig->dev, data);
}


static inline void *iio_trigger_get_drvdata(struct iio_trigger *trig)
{
	return dev_get_drvdata(&trig->dev);
}


int iio_trigger_register(struct iio_trigger *trig_info);

int devm_iio_trigger_register(struct device *dev,
			      struct iio_trigger *trig_info);


void iio_trigger_unregister(struct iio_trigger *trig_info);


int iio_trigger_set_immutable(struct iio_dev *indio_dev, struct iio_trigger *trig);

void iio_trigger_poll(struct iio_trigger *trig);
void iio_trigger_poll_nested(struct iio_trigger *trig);

irqreturn_t iio_trigger_generic_data_rdy_poll(int irq, void *private);

#define iio_trigger_alloc(parent, fmt, ...) \
	__iio_trigger_alloc((parent), THIS_MODULE, (fmt), ##__VA_ARGS__)

__printf(3, 4)
struct iio_trigger *__iio_trigger_alloc(struct device *parent,
					struct module *this_mod,
					const char *fmt, ...);
void iio_trigger_free(struct iio_trigger *trig);


bool iio_trigger_using_own(struct iio_dev *indio_dev);

int iio_validate_own_trigger(struct iio_dev *idev, struct iio_trigger *trig);
int iio_trigger_validate_own_device(struct iio_trigger *trig,
				     struct iio_dev *indio_dev);

#else
struct iio_trigger;
struct iio_trigger_ops;
#endif
#endif 
