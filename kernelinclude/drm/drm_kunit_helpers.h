// SPDX-License-Identifier: GPL-2.0

#ifndef DRM_KUNIT_HELPERS_H_
#define DRM_KUNIT_HELPERS_H_

#include <drm/drm_drv.h>

#include <linux/device.h>

#include <kunit/test.h>

struct drm_device;
struct kunit;

struct device *drm_kunit_helper_alloc_device(struct kunit *test);
void drm_kunit_helper_free_device(struct kunit *test, struct device *dev);

struct drm_device *
__drm_kunit_helper_alloc_drm_device_with_driver(struct kunit *test,
						struct device *dev,
						size_t size, size_t offset,
						const struct drm_driver *driver);


#define drm_kunit_helper_alloc_drm_device_with_driver(_test, _dev, _type, _member, _drv)	\
	((_type *)__drm_kunit_helper_alloc_drm_device_with_driver(_test, _dev,			\
						       sizeof(_type),				\
						       offsetof(_type, _member),		\
						       _drv))

static inline struct drm_device *
__drm_kunit_helper_alloc_drm_device(struct kunit *test,
				    struct device *dev,
				    size_t size, size_t offset,
				    u32 features)
{
	struct drm_driver *driver;

	driver = devm_kzalloc(dev, sizeof(*driver), GFP_KERNEL);
	KUNIT_ASSERT_NOT_NULL(test, driver);

	driver->driver_features = features;

	return __drm_kunit_helper_alloc_drm_device_with_driver(test, dev,
							       size, offset,
							       driver);
}


#define drm_kunit_helper_alloc_drm_device(_test, _dev, _type, _member, _feat)	\
	((_type *)__drm_kunit_helper_alloc_drm_device(_test, _dev,		\
						      sizeof(_type),		\
						      offsetof(_type, _member),	\
						      _feat))
struct drm_modeset_acquire_ctx *
drm_kunit_helper_acquire_ctx_alloc(struct kunit *test);

struct drm_atomic_state *
drm_kunit_helper_atomic_state_alloc(struct kunit *test,
				    struct drm_device *drm,
				    struct drm_modeset_acquire_ctx *ctx);

#endif // DRM_KUNIT_HELPERS_H_
