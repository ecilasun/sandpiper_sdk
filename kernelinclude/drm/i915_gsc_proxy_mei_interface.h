/* SPDX-License-Identifier: MIT */


#ifndef _I915_GSC_PROXY_MEI_INTERFACE_H_
#define _I915_GSC_PROXY_MEI_INTERFACE_H_

#include <linux/types.h>

struct device;
struct module;


struct i915_gsc_proxy_component_ops {
	struct module *owner;

	
	int (*send)(struct device *dev, const void *buf, size_t size);

	
	int (*recv)(struct device *dev, void *buf, size_t size);
};


struct i915_gsc_proxy_component {
	struct device *mei_dev;
	const struct i915_gsc_proxy_component_ops *ops;
};

#endif 
