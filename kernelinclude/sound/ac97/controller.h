/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (C) 2016 Robert Jarzmik <robert.jarzmik@free.fr>
 */

#ifndef AC97_CONTROLLER_H
#define AC97_CONTROLLER_H

#include <linux/device.h>
#include <linux/list.h>

#define AC97_BUS_MAX_CODECS 4
#define AC97_SLOTS_AVAILABLE_ALL 0xf

struct ac97_controller_ops;


struct ac97_controller {
	const struct ac97_controller_ops *ops;
	struct list_head controllers;
	struct device adap;
	int nr;
	unsigned short slots_available;
	struct device *parent;
	struct ac97_codec_device *codecs[AC97_BUS_MAX_CODECS];
	void *codecs_pdata[AC97_BUS_MAX_CODECS];
};


struct ac97_controller_ops {
	void (*reset)(struct ac97_controller *adrv);
	void (*warm_reset)(struct ac97_controller *adrv);
	int (*write)(struct ac97_controller *adrv, int slot,
		     unsigned short reg, unsigned short val);
	int (*read)(struct ac97_controller *adrv, int slot, unsigned short reg);
};

#if IS_ENABLED(CONFIG_AC97_BUS_NEW)
struct ac97_controller *snd_ac97_controller_register(
	const struct ac97_controller_ops *ops, struct device *dev,
	unsigned short slots_available, void **codecs_pdata);
void snd_ac97_controller_unregister(struct ac97_controller *ac97_ctrl);
#else
static inline struct ac97_controller *
snd_ac97_controller_register(const struct ac97_controller_ops *ops,
			     struct device *dev,
			     unsigned short slots_available,
			     void **codecs_pdata)
{
	return ERR_PTR(-ENODEV);
}

static inline void
snd_ac97_controller_unregister(struct ac97_controller *ac97_ctrl)
{
}
#endif

#endif
