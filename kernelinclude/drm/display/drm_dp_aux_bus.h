/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _DP_AUX_BUS_H_
#define _DP_AUX_BUS_H_

#include <linux/device.h>
#include <linux/mod_devicetable.h>


struct dp_aux_ep_device {
	
	struct device dev;
	
	struct drm_dp_aux *aux;
};

struct dp_aux_ep_driver {
	int (*probe)(struct dp_aux_ep_device *aux_ep);
	void (*remove)(struct dp_aux_ep_device *aux_ep);
	void (*shutdown)(struct dp_aux_ep_device *aux_ep);
	struct device_driver driver;
};

static inline struct dp_aux_ep_device *to_dp_aux_ep_dev(struct device *dev)
{
	return container_of(dev, struct dp_aux_ep_device, dev);
}

static inline struct dp_aux_ep_driver *to_dp_aux_ep_drv(struct device_driver *drv)
{
	return container_of(drv, struct dp_aux_ep_driver, driver);
}

int of_dp_aux_populate_bus(struct drm_dp_aux *aux,
			   int (*done_probing)(struct drm_dp_aux *aux));
void of_dp_aux_depopulate_bus(struct drm_dp_aux *aux);
int devm_of_dp_aux_populate_bus(struct drm_dp_aux *aux,
				int (*done_probing)(struct drm_dp_aux *aux));


static inline int of_dp_aux_populate_ep_devices(struct drm_dp_aux *aux)
{
	int ret;

	ret = of_dp_aux_populate_bus(aux, NULL);

	
	return (ret != -ENODEV) ? ret : 0;
}

static inline int devm_of_dp_aux_populate_ep_devices(struct drm_dp_aux *aux)
{
	int ret;

	ret = devm_of_dp_aux_populate_bus(aux, NULL);

	
	return (ret != -ENODEV) ? ret : 0;
}

static inline void of_dp_aux_depopulate_ep_devices(struct drm_dp_aux *aux)
{
	of_dp_aux_depopulate_bus(aux);
}

#define dp_aux_dp_driver_register(aux_ep_drv) \
	__dp_aux_dp_driver_register(aux_ep_drv, THIS_MODULE)
int __dp_aux_dp_driver_register(struct dp_aux_ep_driver *aux_ep_drv,
				struct module *owner);
void dp_aux_dp_driver_unregister(struct dp_aux_ep_driver *aux_ep_drv);

#endif 
