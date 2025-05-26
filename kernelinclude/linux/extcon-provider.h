/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __LINUX_EXTCON_PROVIDER_H__
#define __LINUX_EXTCON_PROVIDER_H__

#include <linux/extcon.h>

struct extcon_dev;

#if IS_ENABLED(CONFIG_EXTCON)


int extcon_dev_register(struct extcon_dev *edev);
void extcon_dev_unregister(struct extcon_dev *edev);
int devm_extcon_dev_register(struct device *dev,
				struct extcon_dev *edev);
void devm_extcon_dev_unregister(struct device *dev,
				struct extcon_dev *edev);


struct extcon_dev *extcon_dev_allocate(const unsigned int *cable);
void extcon_dev_free(struct extcon_dev *edev);
struct extcon_dev *devm_extcon_dev_allocate(struct device *dev,
				const unsigned int *cable);
void devm_extcon_dev_free(struct device *dev, struct extcon_dev *edev);


int extcon_sync(struct extcon_dev *edev, unsigned int id);


int extcon_set_state(struct extcon_dev *edev, unsigned int id,
				bool state);
int extcon_set_state_sync(struct extcon_dev *edev, unsigned int id,
				bool state);


int extcon_set_property(struct extcon_dev *edev, unsigned int id,
				unsigned int prop,
				union extcon_property_value prop_val);
int extcon_set_property_sync(struct extcon_dev *edev, unsigned int id,
				unsigned int prop,
				union extcon_property_value prop_val);
int extcon_set_property_capability(struct extcon_dev *edev,
				unsigned int id, unsigned int prop);

#else 
static inline int extcon_dev_register(struct extcon_dev *edev)
{
	return 0;
}

static inline void extcon_dev_unregister(struct extcon_dev *edev) { }

static inline int devm_extcon_dev_register(struct device *dev,
				struct extcon_dev *edev)
{
	return -EINVAL;
}

static inline void devm_extcon_dev_unregister(struct device *dev,
				struct extcon_dev *edev) { }

static inline struct extcon_dev *extcon_dev_allocate(const unsigned int *cable)
{
	return ERR_PTR(-ENOSYS);
}

static inline void extcon_dev_free(struct extcon_dev *edev) { }

static inline struct extcon_dev *devm_extcon_dev_allocate(struct device *dev,
				const unsigned int *cable)
{
	return ERR_PTR(-ENOSYS);
}

static inline void devm_extcon_dev_free(struct extcon_dev *edev) { }


static inline int extcon_set_state(struct extcon_dev *edev, unsigned int id,
				bool state)
{
	return 0;
}

static inline int extcon_set_state_sync(struct extcon_dev *edev, unsigned int id,
				bool state)
{
	return 0;
}

static inline int extcon_sync(struct extcon_dev *edev, unsigned int id)
{
	return 0;
}

static inline int extcon_set_property(struct extcon_dev *edev, unsigned int id,
				unsigned int prop,
				union extcon_property_value prop_val)
{
	return 0;
}

static inline int extcon_set_property_sync(struct extcon_dev *edev,
				unsigned int id, unsigned int prop,
				union extcon_property_value prop_val)
{
	return 0;
}

static inline int extcon_set_property_capability(struct extcon_dev *edev,
				unsigned int id, unsigned int prop)
{
	return 0;
}
#endif 
#endif 
