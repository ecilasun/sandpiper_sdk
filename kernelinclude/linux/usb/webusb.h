/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef	__LINUX_USB_WEBUSB_H
#define	__LINUX_USB_WEBUSB_H

#include "uapi/linux/usb/ch9.h"


#define WEBUSB_UUID \
	GUID_INIT(0x3408b638, 0x09a9, 0x47a0, 0x8b, 0xfd, 0xa0, 0x76, 0x88, 0x15, 0xb6, 0x65)


struct usb_webusb_cap_data {
	__le16 bcdVersion;
#define WEBUSB_VERSION_1_00	cpu_to_le16(0x0100) 
	u8  bVendorCode;
	u8  iLandingPage;
#define WEBUSB_LANDING_PAGE_NOT_PRESENT	0
#define WEBUSB_LANDING_PAGE_PRESENT	1 
} __packed;

#define USB_WEBUSB_CAP_DATA_SIZE	4


#define WEBUSB_GET_URL 2


struct webusb_url_descriptor {
	u8  bLength;
#define WEBUSB_URL_DESCRIPTOR_HEADER_LENGTH	3
	u8  bDescriptorType;
#define WEBUSB_URL_DESCRIPTOR_TYPE		3
	u8  bScheme;
#define WEBUSB_URL_SCHEME_HTTP			0
#define WEBUSB_URL_SCHEME_HTTPS			1
#define WEBUSB_URL_SCHEME_NONE			255
	u8  URL[U8_MAX - WEBUSB_URL_DESCRIPTOR_HEADER_LENGTH];
} __packed;


#define WEBUSB_URL_RAW_MAX_LENGTH (U8_MAX - WEBUSB_URL_DESCRIPTOR_HEADER_LENGTH + 8)

#endif 
