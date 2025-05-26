/* SPDX-License-Identifier: GPL-2.0+ */




#ifndef _MEDIA_DEV_ALLOCATOR_H
#define _MEDIA_DEV_ALLOCATOR_H

struct usb_device;

#if defined(CONFIG_MEDIA_CONTROLLER) && IS_ENABLED(CONFIG_USB)

struct media_device *media_device_usb_allocate(struct usb_device *udev,
					       const char *module_name,
					       struct module *owner);

void media_device_delete(struct media_device *mdev, const char *module_name,
			 struct module *owner);
#else
static inline struct media_device *media_device_usb_allocate(
			struct usb_device *udev, const char *module_name,
			struct module *owner)
			{ return NULL; }
static inline void media_device_delete(
			struct media_device *mdev, const char *module_name,
			struct module *owner) { }
#endif 
#endif 
