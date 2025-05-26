/* SPDX-License-Identifier: GPL-2.0 */

#ifndef __LINUX_USB_CH9_H
#define __LINUX_USB_CH9_H

#include <uapi/linux/usb/ch9.h>



enum usb_ssp_rate {
	USB_SSP_GEN_UNKNOWN = 0,
	USB_SSP_GEN_2x1,
	USB_SSP_GEN_1x2,
	USB_SSP_GEN_2x2,
};

struct device;

extern const char *usb_ep_type_string(int ep_type);
extern const char *usb_speed_string(enum usb_device_speed speed);
extern enum usb_device_speed usb_get_maximum_speed(struct device *dev);
extern enum usb_ssp_rate usb_get_maximum_ssp_rate(struct device *dev);
extern const char *usb_state_string(enum usb_device_state state);
unsigned int usb_decode_interval(const struct usb_endpoint_descriptor *epd,
				 enum usb_device_speed speed);

#ifdef CONFIG_TRACING
extern const char *usb_decode_ctrl(char *str, size_t size, __u8 bRequestType,
				   __u8 bRequest, __u16 wValue, __u16 wIndex,
				   __u16 wLength);
#endif

#endif 
