/* SPDX-License-Identifier: GPL-2.0 */


#ifndef WILCO_EC_H
#define WILCO_EC_H

#include <linux/mutex.h>
#include <linux/types.h>


#define WILCO_EC_FLAG_NO_RESPONSE	BIT(0) 


#define EC_MAILBOX_DATA_SIZE		32

struct device;
struct resource;
struct platform_device;


struct wilco_ec_device {
	struct device *dev;
	struct mutex mailbox_lock;
	struct resource *io_command;
	struct resource *io_data;
	struct resource *io_packet;
	void *data_buffer;
	size_t data_size;
	struct platform_device *debugfs_pdev;
	struct platform_device *rtc_pdev;
	struct platform_device *charger_pdev;
	struct platform_device *telem_pdev;
};


struct wilco_ec_request {
	u8 struct_version;
	u8 checksum;
	u16 mailbox_id;
	u8 mailbox_version;
	u8 reserved;
	u16 data_size;
} __packed;


struct wilco_ec_response {
	u8 struct_version;
	u8 checksum;
	u16 result;
	u16 data_size;
	u8 reserved[2];
	u8 data[];
} __packed;


enum wilco_ec_msg_type {
	WILCO_EC_MSG_LEGACY = 0x00f0,
	WILCO_EC_MSG_PROPERTY = 0x00f2,
	WILCO_EC_MSG_TELEMETRY = 0x00f5,
};


struct wilco_ec_message {
	enum wilco_ec_msg_type type;
	u8 flags;
	size_t request_size;
	void *request_data;
	size_t response_size;
	void *response_data;
};


int wilco_ec_mailbox(struct wilco_ec_device *ec, struct wilco_ec_message *msg);


int wilco_keyboard_leds_init(struct wilco_ec_device *ec);



#define WILCO_EC_PROPERTY_MAX_SIZE	4


struct wilco_ec_property_msg {
	u32 property_id;
	int length;
	u8 data[WILCO_EC_PROPERTY_MAX_SIZE];
};


int wilco_ec_get_property(struct wilco_ec_device *ec,
			  struct wilco_ec_property_msg *prop_msg);


int wilco_ec_set_property(struct wilco_ec_device *ec,
			  struct wilco_ec_property_msg *prop_msg);


int wilco_ec_get_byte_property(struct wilco_ec_device *ec, u32 property_id,
			       u8 *val);


int wilco_ec_set_byte_property(struct wilco_ec_device *ec, u32 property_id,
			       u8 val);


int wilco_ec_add_sysfs(struct wilco_ec_device *ec);
void wilco_ec_remove_sysfs(struct wilco_ec_device *ec);

#endif 
