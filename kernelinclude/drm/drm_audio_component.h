// SPDX-License-Identifier: MIT
// Copyright © 2014 Intel Corporation

#ifndef _DRM_AUDIO_COMPONENT_H_
#define _DRM_AUDIO_COMPONENT_H_

#include <linux/completion.h>
#include <linux/types.h>

struct drm_audio_component;
struct device;


struct drm_audio_component_ops {
	
	struct module *owner;
	
	unsigned long (*get_power)(struct device *);
	
	void (*put_power)(struct device *, unsigned long);
	
	void (*codec_wake_override)(struct device *, bool enable);
	
	int (*get_cdclk_freq)(struct device *);
	
	int (*sync_audio_rate)(struct device *, int port, int pipe, int rate);
	
	int (*get_eld)(struct device *, int port, int pipe, bool *enabled,
		       unsigned char *buf, int max_bytes);
};


struct drm_audio_component_audio_ops {
	
	void *audio_ptr;
	
	void (*pin_eld_notify)(void *audio_ptr, int port, int pipe);
	
	int (*pin2port)(void *audio_ptr, int pin);
	
	int (*master_bind)(struct device *dev, struct drm_audio_component *);
	
	void (*master_unbind)(struct device *dev, struct drm_audio_component *);
};


struct drm_audio_component {
	
	struct device *dev;
	
	const struct drm_audio_component_ops *ops;
	
	const struct drm_audio_component_audio_ops *audio_ops;
	
	struct completion master_bind_complete;
};

#endif 
