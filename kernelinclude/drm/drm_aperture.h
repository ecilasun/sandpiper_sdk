/* SPDX-License-Identifier: MIT */

#ifndef _DRM_APERTURE_H_
#define _DRM_APERTURE_H_

#include <linux/types.h>

struct drm_device;
struct drm_driver;
struct pci_dev;

int devm_aperture_acquire_from_firmware(struct drm_device *dev, resource_size_t base,
					resource_size_t size);

int drm_aperture_remove_conflicting_framebuffers(resource_size_t base, resource_size_t size,
						 const struct drm_driver *req_driver);

int drm_aperture_remove_conflicting_pci_framebuffers(struct pci_dev *pdev,
						     const struct drm_driver *req_driver);


static inline int
drm_aperture_remove_framebuffers(const struct drm_driver *req_driver)
{
	return drm_aperture_remove_conflicting_framebuffers(0, (resource_size_t)-1,
							    req_driver);
}

#endif
