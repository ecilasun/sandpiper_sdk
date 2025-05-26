/* SPDX-License-Identifier: GPL-2.0
 *
 *  Copyright (C) 2016 Robert Jarzmik <robert.jarzmik@free.fr>
 */

#ifndef __SOUND_AC97_CODEC2_H
#define __SOUND_AC97_CODEC2_H

#include <linux/device.h>

#define AC97_ID(vendor_id1, vendor_id2) \
	((((vendor_id1) & 0xffff) << 16) | ((vendor_id2) & 0xffff))
#define AC97_DRIVER_ID(vendor_id1, vendor_id2, mask_id1, mask_id2, _data) \
	{ .id = (((vendor_id1) & 0xffff) << 16) | ((vendor_id2) & 0xffff), \
	  .mask = (((mask_id1) & 0xffff) << 16) | ((mask_id2) & 0xffff), \
	  .data = (_data) }

struct ac97_controller;
struct clk;


struct ac97_id {
	unsigned int		id;
	unsigned int		mask;
	void			*data;
};


struct ac97_codec_device {
	struct device		dev;
	unsigned int		vendor_id;
	unsigned int		num;
	struct clk		*clk;
	struct ac97_controller	*ac97_ctrl;
};


struct ac97_codec_driver {
	struct device_driver	driver;
	int			(*probe)(struct ac97_codec_device *);
	void			(*remove)(struct ac97_codec_device *dev);
	void			(*shutdown)(struct ac97_codec_device *);
	const struct ac97_id	*id_table;
};

static inline struct ac97_codec_device *to_ac97_device(struct device *d)
{
	return container_of(d, struct ac97_codec_device, dev);
}

static inline struct ac97_codec_driver *to_ac97_driver(struct device_driver *d)
{
	return container_of(d, struct ac97_codec_driver, driver);
}

#if IS_ENABLED(CONFIG_AC97_BUS_NEW)
int snd_ac97_codec_driver_register(struct ac97_codec_driver *drv);
void snd_ac97_codec_driver_unregister(struct ac97_codec_driver *drv);
#else
static inline int
snd_ac97_codec_driver_register(struct ac97_codec_driver *drv)
{
	return 0;
}
static inline void
snd_ac97_codec_driver_unregister(struct ac97_codec_driver *drv)
{
}
#endif


static inline struct device *
ac97_codec_dev2dev(struct ac97_codec_device *adev)
{
	return &adev->dev;
}

static inline void *ac97_get_drvdata(struct ac97_codec_device *adev)
{
	return dev_get_drvdata(ac97_codec_dev2dev(adev));
}

static inline void ac97_set_drvdata(struct ac97_codec_device *adev,
				    void *data)
{
	dev_set_drvdata(ac97_codec_dev2dev(adev), data);
}

void *snd_ac97_codec_get_platdata(const struct ac97_codec_device *adev);

#endif
