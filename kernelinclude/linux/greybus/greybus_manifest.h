/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __GREYBUS_MANIFEST_H
#define __GREYBUS_MANIFEST_H

#include <linux/bits.h>
#include <linux/types.h>

enum greybus_descriptor_type {
	GREYBUS_TYPE_INVALID		= 0x00,
	GREYBUS_TYPE_INTERFACE		= 0x01,
	GREYBUS_TYPE_STRING		= 0x02,
	GREYBUS_TYPE_BUNDLE		= 0x03,
	GREYBUS_TYPE_CPORT		= 0x04,
};

enum greybus_protocol {
	GREYBUS_PROTOCOL_CONTROL	= 0x00,
	
	GREYBUS_PROTOCOL_GPIO		= 0x02,
	GREYBUS_PROTOCOL_I2C		= 0x03,
	GREYBUS_PROTOCOL_UART		= 0x04,
	GREYBUS_PROTOCOL_HID		= 0x05,
	GREYBUS_PROTOCOL_USB		= 0x06,
	GREYBUS_PROTOCOL_SDIO		= 0x07,
	GREYBUS_PROTOCOL_POWER_SUPPLY	= 0x08,
	GREYBUS_PROTOCOL_PWM		= 0x09,
	
	GREYBUS_PROTOCOL_SPI		= 0x0b,
	GREYBUS_PROTOCOL_DISPLAY	= 0x0c,
	GREYBUS_PROTOCOL_CAMERA_MGMT	= 0x0d,
	GREYBUS_PROTOCOL_SENSOR		= 0x0e,
	GREYBUS_PROTOCOL_LIGHTS		= 0x0f,
	GREYBUS_PROTOCOL_VIBRATOR	= 0x10,
	GREYBUS_PROTOCOL_LOOPBACK	= 0x11,
	GREYBUS_PROTOCOL_AUDIO_MGMT	= 0x12,
	GREYBUS_PROTOCOL_AUDIO_DATA	= 0x13,
	GREYBUS_PROTOCOL_SVC            = 0x14,
	GREYBUS_PROTOCOL_BOOTROM	= 0x15,
	GREYBUS_PROTOCOL_CAMERA_DATA	= 0x16,
	GREYBUS_PROTOCOL_FW_DOWNLOAD	= 0x17,
	GREYBUS_PROTOCOL_FW_MANAGEMENT	= 0x18,
	GREYBUS_PROTOCOL_AUTHENTICATION	= 0x19,
	GREYBUS_PROTOCOL_LOG		= 0x1a,
		
	GREYBUS_PROTOCOL_RAW		= 0xfe,
	GREYBUS_PROTOCOL_VENDOR		= 0xff,
};

enum greybus_class_type {
	GREYBUS_CLASS_CONTROL		= 0x00,
	
	
	
	
	GREYBUS_CLASS_HID		= 0x05,
	
	
	GREYBUS_CLASS_POWER_SUPPLY	= 0x08,
	
	GREYBUS_CLASS_BRIDGED_PHY	= 0x0a,
	
	GREYBUS_CLASS_DISPLAY		= 0x0c,
	GREYBUS_CLASS_CAMERA		= 0x0d,
	GREYBUS_CLASS_SENSOR		= 0x0e,
	GREYBUS_CLASS_LIGHTS		= 0x0f,
	GREYBUS_CLASS_VIBRATOR		= 0x10,
	GREYBUS_CLASS_LOOPBACK		= 0x11,
	GREYBUS_CLASS_AUDIO		= 0x12,
	
	
	GREYBUS_CLASS_BOOTROM		= 0x15,
	GREYBUS_CLASS_FW_MANAGEMENT	= 0x16,
	GREYBUS_CLASS_LOG		= 0x17,
		
	GREYBUS_CLASS_RAW		= 0xfe,
	GREYBUS_CLASS_VENDOR		= 0xff,
};

enum {
	GREYBUS_INTERFACE_FEATURE_TIMESYNC = BIT(0),
};


struct greybus_descriptor_string {
	__u8	length;
	__u8	id;
	__u8	string[];
} __packed;


struct greybus_descriptor_interface {
	__u8	vendor_stringid;
	__u8	product_stringid;
	__u8	features;
	__u8	pad;
} __packed;


struct greybus_descriptor_bundle {
	__u8	id;	
	__u8	class;
	__u8	pad[2];
} __packed;


struct greybus_descriptor_cport {
	__le16	id;
	__u8	bundle;
	__u8	protocol_id;	
} __packed;

struct greybus_descriptor_header {
	__le16	size;
	__u8	type;		
	__u8	pad;
} __packed;

struct greybus_descriptor {
	struct greybus_descriptor_header		header;
	union {
		struct greybus_descriptor_string	string;
		struct greybus_descriptor_interface	interface;
		struct greybus_descriptor_bundle	bundle;
		struct greybus_descriptor_cport		cport;
	};
} __packed;

struct greybus_manifest_header {
	__le16	size;
	__u8	version_major;
	__u8	version_minor;
} __packed;

struct greybus_manifest {
	struct greybus_manifest_header		header;
	struct greybus_descriptor		descriptors[];
} __packed;

#endif 
