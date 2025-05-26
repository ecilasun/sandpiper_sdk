/* SPDX-License-Identifier: (GPL-2.0 OR BSD-3-Clause) */


#ifndef __GREYBUS_PROTOCOLS_H
#define __GREYBUS_PROTOCOLS_H

#include <linux/types.h>




#define GB_SVC_DEVICE_ID_SVC			0
#define GB_SVC_DEVICE_ID_AP			1
#define GB_SVC_DEVICE_ID_MIN			2
#define GB_SVC_DEVICE_ID_MAX			31

#define GB_SVC_CPORT_ID				0
#define GB_CONTROL_BUNDLE_ID			0
#define GB_CONTROL_CPORT_ID			0



struct gb_operation_msg_hdr {
	__le16	size;		
	__le16	operation_id;	
	__u8	type;		
	__u8	result;		
	__u8	pad[2];		
} __packed;



#define GB_REQUEST_TYPE_CPORT_SHUTDOWN		0x00
#define GB_REQUEST_TYPE_INVALID			0x7f

struct gb_cport_shutdown_request {
	__u8 phase;
} __packed;





#define GB_CONTROL_TYPE_VERSION			0x01
#define GB_CONTROL_TYPE_PROBE_AP		0x02
#define GB_CONTROL_TYPE_GET_MANIFEST_SIZE	0x03
#define GB_CONTROL_TYPE_GET_MANIFEST		0x04
#define GB_CONTROL_TYPE_CONNECTED		0x05
#define GB_CONTROL_TYPE_DISCONNECTED		0x06
#define GB_CONTROL_TYPE_TIMESYNC_ENABLE		0x07
#define GB_CONTROL_TYPE_TIMESYNC_DISABLE	0x08
#define GB_CONTROL_TYPE_TIMESYNC_AUTHORITATIVE	0x09

#define GB_CONTROL_TYPE_BUNDLE_VERSION		0x0b
#define GB_CONTROL_TYPE_DISCONNECTING		0x0c
#define GB_CONTROL_TYPE_TIMESYNC_GET_LAST_EVENT	0x0d
#define GB_CONTROL_TYPE_MODE_SWITCH		0x0e
#define GB_CONTROL_TYPE_BUNDLE_SUSPEND		0x0f
#define GB_CONTROL_TYPE_BUNDLE_RESUME		0x10
#define GB_CONTROL_TYPE_BUNDLE_DEACTIVATE	0x11
#define GB_CONTROL_TYPE_BUNDLE_ACTIVATE		0x12
#define GB_CONTROL_TYPE_INTF_SUSPEND_PREPARE		0x13
#define GB_CONTROL_TYPE_INTF_DEACTIVATE_PREPARE	0x14
#define GB_CONTROL_TYPE_INTF_HIBERNATE_ABORT	0x15

struct gb_control_version_request {
	__u8	major;
	__u8	minor;
} __packed;

struct gb_control_version_response {
	__u8	major;
	__u8	minor;
} __packed;

struct gb_control_bundle_version_request {
	__u8	bundle_id;
} __packed;

struct gb_control_bundle_version_response {
	__u8	major;
	__u8	minor;
} __packed;


struct gb_control_get_manifest_size_response {
	__le16			size;
} __packed;


struct gb_control_get_manifest_response {
	__u8			data[0];
} __packed;


struct gb_control_connected_request {
	__le16			cport_id;
} __packed;

struct gb_control_disconnecting_request {
	__le16			cport_id;
} __packed;


struct gb_control_disconnected_request {
	__le16			cport_id;
} __packed;




#define GB_CONTROL_BUNDLE_PM_OK		0x00
#define GB_CONTROL_BUNDLE_PM_INVAL	0x01
#define GB_CONTROL_BUNDLE_PM_BUSY	0x02
#define GB_CONTROL_BUNDLE_PM_FAIL	0x03
#define GB_CONTROL_BUNDLE_PM_NA		0x04

struct gb_control_bundle_pm_request {
	__u8	bundle_id;
} __packed;

struct gb_control_bundle_pm_response {
	__u8	status;
} __packed;



#define GB_CONTROL_INTF_PM_OK		0x00
#define GB_CONTROL_INTF_PM_BUSY		0x01
#define GB_CONTROL_INTF_PM_NA		0x02

struct gb_control_intf_pm_response {
	__u8	status;
} __packed;




#define GB_APB_REQUEST_LOG			0x02


#define GB_APB_REQUEST_EP_MAPPING		0x03


#define GB_APB_REQUEST_CPORT_COUNT		0x04


#define GB_APB_REQUEST_RESET_CPORT		0x05


#define GB_APB_REQUEST_LATENCY_TAG_EN		0x06
#define GB_APB_REQUEST_LATENCY_TAG_DIS		0x07


#define GB_APB_REQUEST_CSI_TX_CONTROL		0x08


#define GB_APB_REQUEST_AUDIO_CONTROL		0x09


#define GB_APB_REQUEST_TIMESYNC_ENABLE		0x0d
#define GB_APB_REQUEST_TIMESYNC_DISABLE		0x0e
#define GB_APB_REQUEST_TIMESYNC_AUTHORITATIVE	0x0f
#define GB_APB_REQUEST_TIMESYNC_GET_LAST_EVENT	0x10


#define GB_APB_REQUEST_CPORT_FLAGS		0x11


#define GB_APB_REQUEST_ARPC_RUN			0x12

struct gb_apb_request_cport_flags {
	__le32	flags;
#define GB_APB_CPORT_FLAG_CONTROL		0x01
#define GB_APB_CPORT_FLAG_HIGH_PRIO		0x02
} __packed;





#define GB_FW_DOWNLOAD_TYPE_FIND_FIRMWARE	0x01
#define GB_FW_DOWNLOAD_TYPE_FETCH_FIRMWARE	0x02
#define GB_FW_DOWNLOAD_TYPE_RELEASE_FIRMWARE	0x03

#define GB_FIRMWARE_TAG_MAX_SIZE		10


struct gb_fw_download_find_firmware_request {
	__u8			firmware_tag[GB_FIRMWARE_TAG_MAX_SIZE];
} __packed;

struct gb_fw_download_find_firmware_response {
	__u8			firmware_id;
	__le32			size;
} __packed;


struct gb_fw_download_fetch_firmware_request {
	__u8			firmware_id;
	__le32			offset;
	__le32			size;
} __packed;

struct gb_fw_download_fetch_firmware_response {
	__u8			data[0];
} __packed;


struct gb_fw_download_release_firmware_request {
	__u8			firmware_id;
} __packed;






#define GB_FW_MGMT_TYPE_INTERFACE_FW_VERSION	0x01
#define GB_FW_MGMT_TYPE_LOAD_AND_VALIDATE_FW	0x02
#define GB_FW_MGMT_TYPE_LOADED_FW		0x03
#define GB_FW_MGMT_TYPE_BACKEND_FW_VERSION	0x04
#define GB_FW_MGMT_TYPE_BACKEND_FW_UPDATE	0x05
#define GB_FW_MGMT_TYPE_BACKEND_FW_UPDATED	0x06

#define GB_FW_LOAD_METHOD_UNIPRO		0x01
#define GB_FW_LOAD_METHOD_INTERNAL		0x02

#define GB_FW_LOAD_STATUS_FAILED		0x00
#define GB_FW_LOAD_STATUS_UNVALIDATED		0x01
#define GB_FW_LOAD_STATUS_VALIDATED		0x02
#define GB_FW_LOAD_STATUS_VALIDATION_FAILED	0x03

#define GB_FW_BACKEND_FW_STATUS_SUCCESS		0x01
#define GB_FW_BACKEND_FW_STATUS_FAIL_FIND	0x02
#define GB_FW_BACKEND_FW_STATUS_FAIL_FETCH	0x03
#define GB_FW_BACKEND_FW_STATUS_FAIL_WRITE	0x04
#define GB_FW_BACKEND_FW_STATUS_INT		0x05
#define GB_FW_BACKEND_FW_STATUS_RETRY		0x06
#define GB_FW_BACKEND_FW_STATUS_NOT_SUPPORTED	0x07

#define GB_FW_BACKEND_VERSION_STATUS_SUCCESS		0x01
#define GB_FW_BACKEND_VERSION_STATUS_NOT_AVAILABLE	0x02
#define GB_FW_BACKEND_VERSION_STATUS_NOT_SUPPORTED	0x03
#define GB_FW_BACKEND_VERSION_STATUS_RETRY		0x04
#define GB_FW_BACKEND_VERSION_STATUS_FAIL_INT		0x05


struct gb_fw_mgmt_interface_fw_version_response {
	__u8			firmware_tag[GB_FIRMWARE_TAG_MAX_SIZE];
	__le16			major;
	__le16			minor;
} __packed;


struct gb_fw_mgmt_load_and_validate_fw_request {
	__u8			request_id;
	__u8			load_method;
	__u8			firmware_tag[GB_FIRMWARE_TAG_MAX_SIZE];
} __packed;



struct gb_fw_mgmt_loaded_fw_request {
	__u8			request_id;
	__u8			status;
	__le16			major;
	__le16			minor;
} __packed;



struct gb_fw_mgmt_backend_fw_version_request {
	__u8			firmware_tag[GB_FIRMWARE_TAG_MAX_SIZE];
} __packed;

struct gb_fw_mgmt_backend_fw_version_response {
	__le16			major;
	__le16			minor;
	__u8			status;
} __packed;


struct gb_fw_mgmt_backend_fw_update_request {
	__u8			request_id;
	__u8			firmware_tag[GB_FIRMWARE_TAG_MAX_SIZE];
} __packed;



struct gb_fw_mgmt_backend_fw_updated_request {
	__u8			request_id;
	__u8			status;
} __packed;






#define GB_CAP_TYPE_GET_ENDPOINT_UID	0x01
#define GB_CAP_TYPE_GET_IMS_CERTIFICATE	0x02
#define GB_CAP_TYPE_AUTHENTICATE	0x03


struct gb_cap_get_endpoint_uid_response {
	__u8			uid[8];
} __packed;


struct gb_cap_get_ims_certificate_request {
	__le32			certificate_class;
	__le32			certificate_id;
} __packed;

struct gb_cap_get_ims_certificate_response {
	__u8			result_code;
	__u8			certificate[];
} __packed;


struct gb_cap_authenticate_request {
	__le32			auth_type;
	__u8			uid[8];
	__u8			challenge[32];
} __packed;

struct gb_cap_authenticate_response {
	__u8			result_code;
	__u8			response[64];
	__u8			signature[];
} __packed;





#define GB_BOOTROM_VERSION_MAJOR		0x00
#define GB_BOOTROM_VERSION_MINOR		0x01


#define GB_BOOTROM_TYPE_VERSION			0x01
#define GB_BOOTROM_TYPE_FIRMWARE_SIZE		0x02
#define GB_BOOTROM_TYPE_GET_FIRMWARE		0x03
#define GB_BOOTROM_TYPE_READY_TO_BOOT		0x04
#define GB_BOOTROM_TYPE_AP_READY		0x05	
#define GB_BOOTROM_TYPE_GET_VID_PID		0x06	


#define GB_BOOTROM_BOOT_STAGE_ONE		0x01 
#define GB_BOOTROM_BOOT_STAGE_TWO		0x02 
#define GB_BOOTROM_BOOT_STAGE_THREE		0x03 


#define GB_BOOTROM_BOOT_STATUS_INVALID		0x00 
#define GB_BOOTROM_BOOT_STATUS_INSECURE		0x01 
#define GB_BOOTROM_BOOT_STATUS_SECURE		0x02 


#define GB_BOOTROM_FETCH_MAX			2000

struct gb_bootrom_version_request {
	__u8	major;
	__u8	minor;
} __packed;

struct gb_bootrom_version_response {
	__u8	major;
	__u8	minor;
} __packed;


struct gb_bootrom_firmware_size_request {
	__u8			stage;
} __packed;

struct gb_bootrom_firmware_size_response {
	__le32			size;
} __packed;


struct gb_bootrom_get_firmware_request {
	__le32			offset;
	__le32			size;
} __packed;

struct gb_bootrom_get_firmware_response {
	__u8			data[0];
} __packed;


struct gb_bootrom_ready_to_boot_request {
	__u8			status;
} __packed;



struct gb_bootrom_get_vid_pid_response {
	__le32			vendor_id;
	__le32			product_id;
} __packed;





#define GB_POWER_SUPPLY_TYPE_GET_SUPPLIES		0x02
#define GB_POWER_SUPPLY_TYPE_GET_DESCRIPTION		0x03
#define GB_POWER_SUPPLY_TYPE_GET_PROP_DESCRIPTORS	0x04
#define GB_POWER_SUPPLY_TYPE_GET_PROPERTY		0x05
#define GB_POWER_SUPPLY_TYPE_SET_PROPERTY		0x06
#define GB_POWER_SUPPLY_TYPE_EVENT			0x07


#define GB_POWER_SUPPLY_TECH_UNKNOWN			0x0000
#define GB_POWER_SUPPLY_TECH_NiMH			0x0001
#define GB_POWER_SUPPLY_TECH_LION			0x0002
#define GB_POWER_SUPPLY_TECH_LIPO			0x0003
#define GB_POWER_SUPPLY_TECH_LiFe			0x0004
#define GB_POWER_SUPPLY_TECH_NiCd			0x0005
#define GB_POWER_SUPPLY_TECH_LiMn			0x0006


#define GB_POWER_SUPPLY_UNKNOWN_TYPE			0x0000
#define GB_POWER_SUPPLY_BATTERY_TYPE			0x0001
#define GB_POWER_SUPPLY_UPS_TYPE			0x0002
#define GB_POWER_SUPPLY_MAINS_TYPE			0x0003
#define GB_POWER_SUPPLY_USB_TYPE			0x0004
#define GB_POWER_SUPPLY_USB_DCP_TYPE			0x0005
#define GB_POWER_SUPPLY_USB_CDP_TYPE			0x0006
#define GB_POWER_SUPPLY_USB_ACA_TYPE			0x0007


#define GB_POWER_SUPPLY_HEALTH_UNKNOWN			0x0000
#define GB_POWER_SUPPLY_HEALTH_GOOD			0x0001
#define GB_POWER_SUPPLY_HEALTH_OVERHEAT			0x0002
#define GB_POWER_SUPPLY_HEALTH_DEAD			0x0003
#define GB_POWER_SUPPLY_HEALTH_OVERVOLTAGE		0x0004
#define GB_POWER_SUPPLY_HEALTH_UNSPEC_FAILURE		0x0005
#define GB_POWER_SUPPLY_HEALTH_COLD			0x0006
#define GB_POWER_SUPPLY_HEALTH_WATCHDOG_TIMER_EXPIRE	0x0007
#define GB_POWER_SUPPLY_HEALTH_SAFETY_TIMER_EXPIRE	0x0008


#define GB_POWER_SUPPLY_STATUS_UNKNOWN			0x0000
#define GB_POWER_SUPPLY_STATUS_CHARGING			0x0001
#define GB_POWER_SUPPLY_STATUS_DISCHARGING		0x0002
#define GB_POWER_SUPPLY_STATUS_NOT_CHARGING		0x0003
#define GB_POWER_SUPPLY_STATUS_FULL			0x0004


#define GB_POWER_SUPPLY_CAPACITY_LEVEL_UNKNOWN		0x0000
#define GB_POWER_SUPPLY_CAPACITY_LEVEL_CRITICAL		0x0001
#define GB_POWER_SUPPLY_CAPACITY_LEVEL_LOW		0x0002
#define GB_POWER_SUPPLY_CAPACITY_LEVEL_NORMAL		0x0003
#define GB_POWER_SUPPLY_CAPACITY_LEVEL_HIGH		0x0004
#define GB_POWER_SUPPLY_CAPACITY_LEVEL_FULL		0x0005


#define GB_POWER_SUPPLY_SCOPE_UNKNOWN			0x0000
#define GB_POWER_SUPPLY_SCOPE_SYSTEM			0x0001
#define GB_POWER_SUPPLY_SCOPE_DEVICE			0x0002

struct gb_power_supply_get_supplies_response {
	__u8	supplies_count;
} __packed;

struct gb_power_supply_get_description_request {
	__u8	psy_id;
} __packed;

struct gb_power_supply_get_description_response {
	__u8	manufacturer[32];
	__u8	model[32];
	__u8	serial_number[32];
	__le16	type;
	__u8	properties_count;
} __packed;

struct gb_power_supply_props_desc {
	__u8	property;
#define GB_POWER_SUPPLY_PROP_STATUS				0x00
#define GB_POWER_SUPPLY_PROP_CHARGE_TYPE			0x01
#define GB_POWER_SUPPLY_PROP_HEALTH				0x02
#define GB_POWER_SUPPLY_PROP_PRESENT				0x03
#define GB_POWER_SUPPLY_PROP_ONLINE				0x04
#define GB_POWER_SUPPLY_PROP_AUTHENTIC				0x05
#define GB_POWER_SUPPLY_PROP_TECHNOLOGY				0x06
#define GB_POWER_SUPPLY_PROP_CYCLE_COUNT			0x07
#define GB_POWER_SUPPLY_PROP_VOLTAGE_MAX			0x08
#define GB_POWER_SUPPLY_PROP_VOLTAGE_MIN			0x09
#define GB_POWER_SUPPLY_PROP_VOLTAGE_MAX_DESIGN			0x0A
#define GB_POWER_SUPPLY_PROP_VOLTAGE_MIN_DESIGN			0x0B
#define GB_POWER_SUPPLY_PROP_VOLTAGE_NOW			0x0C
#define GB_POWER_SUPPLY_PROP_VOLTAGE_AVG			0x0D
#define GB_POWER_SUPPLY_PROP_VOLTAGE_OCV			0x0E
#define GB_POWER_SUPPLY_PROP_VOLTAGE_BOOT			0x0F
#define GB_POWER_SUPPLY_PROP_CURRENT_MAX			0x10
#define GB_POWER_SUPPLY_PROP_CURRENT_NOW			0x11
#define GB_POWER_SUPPLY_PROP_CURRENT_AVG			0x12
#define GB_POWER_SUPPLY_PROP_CURRENT_BOOT			0x13
#define GB_POWER_SUPPLY_PROP_POWER_NOW				0x14
#define GB_POWER_SUPPLY_PROP_POWER_AVG				0x15
#define GB_POWER_SUPPLY_PROP_CHARGE_FULL_DESIGN			0x16
#define GB_POWER_SUPPLY_PROP_CHARGE_EMPTY_DESIGN		0x17
#define GB_POWER_SUPPLY_PROP_CHARGE_FULL			0x18
#define GB_POWER_SUPPLY_PROP_CHARGE_EMPTY			0x19
#define GB_POWER_SUPPLY_PROP_CHARGE_NOW				0x1A
#define GB_POWER_SUPPLY_PROP_CHARGE_AVG				0x1B
#define GB_POWER_SUPPLY_PROP_CHARGE_COUNTER			0x1C
#define GB_POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT		0x1D
#define GB_POWER_SUPPLY_PROP_CONSTANT_CHARGE_CURRENT_MAX	0x1E
#define GB_POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE		0x1F
#define GB_POWER_SUPPLY_PROP_CONSTANT_CHARGE_VOLTAGE_MAX	0x20
#define GB_POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT		0x21
#define GB_POWER_SUPPLY_PROP_CHARGE_CONTROL_LIMIT_MAX		0x22
#define GB_POWER_SUPPLY_PROP_INPUT_CURRENT_LIMIT		0x23
#define GB_POWER_SUPPLY_PROP_ENERGY_FULL_DESIGN			0x24
#define GB_POWER_SUPPLY_PROP_ENERGY_EMPTY_DESIGN		0x25
#define GB_POWER_SUPPLY_PROP_ENERGY_FULL			0x26
#define GB_POWER_SUPPLY_PROP_ENERGY_EMPTY			0x27
#define GB_POWER_SUPPLY_PROP_ENERGY_NOW				0x28
#define GB_POWER_SUPPLY_PROP_ENERGY_AVG				0x29
#define GB_POWER_SUPPLY_PROP_CAPACITY				0x2A
#define GB_POWER_SUPPLY_PROP_CAPACITY_ALERT_MIN			0x2B
#define GB_POWER_SUPPLY_PROP_CAPACITY_ALERT_MAX			0x2C
#define GB_POWER_SUPPLY_PROP_CAPACITY_LEVEL			0x2D
#define GB_POWER_SUPPLY_PROP_TEMP				0x2E
#define GB_POWER_SUPPLY_PROP_TEMP_MAX				0x2F
#define GB_POWER_SUPPLY_PROP_TEMP_MIN				0x30
#define GB_POWER_SUPPLY_PROP_TEMP_ALERT_MIN			0x31
#define GB_POWER_SUPPLY_PROP_TEMP_ALERT_MAX			0x32
#define GB_POWER_SUPPLY_PROP_TEMP_AMBIENT			0x33
#define GB_POWER_SUPPLY_PROP_TEMP_AMBIENT_ALERT_MIN		0x34
#define GB_POWER_SUPPLY_PROP_TEMP_AMBIENT_ALERT_MAX		0x35
#define GB_POWER_SUPPLY_PROP_TIME_TO_EMPTY_NOW			0x36
#define GB_POWER_SUPPLY_PROP_TIME_TO_EMPTY_AVG			0x37
#define GB_POWER_SUPPLY_PROP_TIME_TO_FULL_NOW			0x38
#define GB_POWER_SUPPLY_PROP_TIME_TO_FULL_AVG			0x39
#define GB_POWER_SUPPLY_PROP_TYPE				0x3A
#define GB_POWER_SUPPLY_PROP_SCOPE				0x3B
#define GB_POWER_SUPPLY_PROP_CHARGE_TERM_CURRENT		0x3C
#define GB_POWER_SUPPLY_PROP_CALIBRATE				0x3D
	__u8	is_writeable;
} __packed;

struct gb_power_supply_get_property_descriptors_request {
	__u8	psy_id;
} __packed;

struct gb_power_supply_get_property_descriptors_response {
	__u8	properties_count;
	struct gb_power_supply_props_desc props[];
} __packed;

struct gb_power_supply_get_property_request {
	__u8	psy_id;
	__u8	property;
} __packed;

struct gb_power_supply_get_property_response {
	__le32	prop_val;
};

struct gb_power_supply_set_property_request {
	__u8	psy_id;
	__u8	property;
	__le32	prop_val;
} __packed;

struct gb_power_supply_event_request {
	__u8	psy_id;
	__u8	event;
#define GB_POWER_SUPPLY_UPDATE		0x01
} __packed;





#define GB_HID_TYPE_GET_DESC		0x02
#define GB_HID_TYPE_GET_REPORT_DESC	0x03
#define GB_HID_TYPE_PWR_ON		0x04
#define GB_HID_TYPE_PWR_OFF		0x05
#define GB_HID_TYPE_GET_REPORT		0x06
#define GB_HID_TYPE_SET_REPORT		0x07
#define GB_HID_TYPE_IRQ_EVENT		0x08


#define GB_HID_INPUT_REPORT		0
#define GB_HID_OUTPUT_REPORT		1
#define GB_HID_FEATURE_REPORT		2



struct gb_hid_desc_response {
	__u8				bLength;
	__le16				wReportDescLength;
	__le16				bcdHID;
	__le16				wProductID;
	__le16				wVendorID;
	__u8				bCountryCode;
} __packed;


struct gb_hid_get_report_request {
	__u8				report_type;
	__u8				report_id;
} __packed;


struct gb_hid_set_report_request {
	__u8				report_type;
	__u8				report_id;
	__u8				report[];
} __packed;


struct gb_hid_input_report_request {
	__u8				report[0];
} __packed;





#define GB_I2C_TYPE_FUNCTIONALITY	0x02
#define GB_I2C_TYPE_TRANSFER		0x05


struct gb_i2c_functionality_response {
	__le32	functionality;
} __packed;


struct gb_i2c_transfer_op {
	__le16	addr;
	__le16	flags;
	__le16	size;
} __packed;

struct gb_i2c_transfer_request {
	__le16				op_count;
	struct gb_i2c_transfer_op	ops[];		
} __packed;
struct gb_i2c_transfer_response {
	__u8				data[0];	
} __packed;





#define GB_GPIO_TYPE_LINE_COUNT		0x02
#define GB_GPIO_TYPE_ACTIVATE		0x03
#define GB_GPIO_TYPE_DEACTIVATE		0x04
#define GB_GPIO_TYPE_GET_DIRECTION	0x05
#define GB_GPIO_TYPE_DIRECTION_IN	0x06
#define GB_GPIO_TYPE_DIRECTION_OUT	0x07
#define GB_GPIO_TYPE_GET_VALUE		0x08
#define GB_GPIO_TYPE_SET_VALUE		0x09
#define GB_GPIO_TYPE_SET_DEBOUNCE	0x0a
#define GB_GPIO_TYPE_IRQ_TYPE		0x0b
#define GB_GPIO_TYPE_IRQ_MASK		0x0c
#define GB_GPIO_TYPE_IRQ_UNMASK		0x0d
#define GB_GPIO_TYPE_IRQ_EVENT		0x0e

#define GB_GPIO_IRQ_TYPE_NONE		0x00
#define GB_GPIO_IRQ_TYPE_EDGE_RISING	0x01
#define GB_GPIO_IRQ_TYPE_EDGE_FALLING	0x02
#define GB_GPIO_IRQ_TYPE_EDGE_BOTH	0x03
#define GB_GPIO_IRQ_TYPE_LEVEL_HIGH	0x04
#define GB_GPIO_IRQ_TYPE_LEVEL_LOW	0x08


struct gb_gpio_line_count_response {
	__u8	count;
} __packed;

struct gb_gpio_activate_request {
	__u8	which;
} __packed;


struct gb_gpio_deactivate_request {
	__u8	which;
} __packed;


struct gb_gpio_get_direction_request {
	__u8	which;
} __packed;
struct gb_gpio_get_direction_response {
	__u8	direction;
} __packed;

struct gb_gpio_direction_in_request {
	__u8	which;
} __packed;


struct gb_gpio_direction_out_request {
	__u8	which;
	__u8	value;
} __packed;


struct gb_gpio_get_value_request {
	__u8	which;
} __packed;
struct gb_gpio_get_value_response {
	__u8	value;
} __packed;

struct gb_gpio_set_value_request {
	__u8	which;
	__u8	value;
} __packed;


struct gb_gpio_set_debounce_request {
	__u8	which;
	__le16	usec;
} __packed;


struct gb_gpio_irq_type_request {
	__u8	which;
	__u8	type;
} __packed;


struct gb_gpio_irq_mask_request {
	__u8	which;
} __packed;


struct gb_gpio_irq_unmask_request {
	__u8	which;
} __packed;



struct gb_gpio_irq_event_request {
	__u8	which;
} __packed;






#define GB_PWM_TYPE_PWM_COUNT		0x02
#define GB_PWM_TYPE_ACTIVATE		0x03
#define GB_PWM_TYPE_DEACTIVATE		0x04
#define GB_PWM_TYPE_CONFIG		0x05
#define GB_PWM_TYPE_POLARITY		0x06
#define GB_PWM_TYPE_ENABLE		0x07
#define GB_PWM_TYPE_DISABLE		0x08


struct gb_pwm_count_response {
	__u8	count;
} __packed;

struct gb_pwm_activate_request {
	__u8	which;
} __packed;

struct gb_pwm_deactivate_request {
	__u8	which;
} __packed;

struct gb_pwm_config_request {
	__u8	which;
	__le32	duty;
	__le32	period;
} __packed;

struct gb_pwm_polarity_request {
	__u8	which;
	__u8	polarity;
} __packed;

struct gb_pwm_enable_request {
	__u8	which;
} __packed;

struct gb_pwm_disable_request {
	__u8	which;
} __packed;




#define GB_SPI_MODE_CPHA		0x01		
#define GB_SPI_MODE_CPOL		0x02		
#define GB_SPI_MODE_MODE_0		(0 | 0)		
#define GB_SPI_MODE_MODE_1		(0 | GB_SPI_MODE_CPHA)
#define GB_SPI_MODE_MODE_2		(GB_SPI_MODE_CPOL | 0)
#define GB_SPI_MODE_MODE_3		(GB_SPI_MODE_CPOL | GB_SPI_MODE_CPHA)
#define GB_SPI_MODE_CS_HIGH		0x04		
#define GB_SPI_MODE_LSB_FIRST		0x08		
#define GB_SPI_MODE_3WIRE		0x10		
#define GB_SPI_MODE_LOOP		0x20		
#define GB_SPI_MODE_NO_CS		0x40		
#define GB_SPI_MODE_READY		0x80		


#define GB_SPI_FLAG_HALF_DUPLEX		BIT(0)		
#define GB_SPI_FLAG_NO_RX		BIT(1)		
#define GB_SPI_FLAG_NO_TX		BIT(2)		


#define GB_SPI_TYPE_MASTER_CONFIG	0x02
#define GB_SPI_TYPE_DEVICE_CONFIG	0x03
#define GB_SPI_TYPE_TRANSFER		0x04


struct gb_spi_master_config_response {
	__le32	bits_per_word_mask;
	__le32	min_speed_hz;
	__le32	max_speed_hz;
	__le16	mode;
	__le16	flags;
	__u8	num_chipselect;
} __packed;

struct gb_spi_device_config_request {
	__u8	chip_select;
} __packed;

struct gb_spi_device_config_response {
	__le16	mode;
	__u8	bits_per_word;
	__le32	max_speed_hz;
	__u8	device_type;
#define GB_SPI_SPI_DEV		0x00
#define GB_SPI_SPI_NOR		0x01
#define GB_SPI_SPI_MODALIAS	0x02
	__u8	name[32];
} __packed;


struct gb_spi_transfer {
	__le32		speed_hz;
	__le32		len;
	__le16		delay_usecs;
	__u8		cs_change;
	__u8		bits_per_word;
	__u8		xfer_flags;
#define GB_SPI_XFER_READ	0x01
#define GB_SPI_XFER_WRITE	0x02
#define GB_SPI_XFER_INPROGRESS	0x04
} __packed;

struct gb_spi_transfer_request {
	__u8			chip_select;	
	__u8			mode;		
	__le16			count;
	struct gb_spi_transfer	transfers[];	
} __packed;

struct gb_spi_transfer_response {
	__u8			data[0];	
} __packed;


#define GB_SVC_VERSION_MAJOR		0x00
#define GB_SVC_VERSION_MINOR		0x01


#define GB_SVC_TYPE_PROTOCOL_VERSION		0x01
#define GB_SVC_TYPE_SVC_HELLO			0x02
#define GB_SVC_TYPE_INTF_DEVICE_ID		0x03
#define GB_SVC_TYPE_INTF_RESET			0x06
#define GB_SVC_TYPE_CONN_CREATE			0x07
#define GB_SVC_TYPE_CONN_DESTROY		0x08
#define GB_SVC_TYPE_DME_PEER_GET		0x09
#define GB_SVC_TYPE_DME_PEER_SET		0x0a
#define GB_SVC_TYPE_ROUTE_CREATE		0x0b
#define GB_SVC_TYPE_ROUTE_DESTROY		0x0c
#define GB_SVC_TYPE_TIMESYNC_ENABLE		0x0d
#define GB_SVC_TYPE_TIMESYNC_DISABLE		0x0e
#define GB_SVC_TYPE_TIMESYNC_AUTHORITATIVE	0x0f
#define GB_SVC_TYPE_INTF_SET_PWRM		0x10
#define GB_SVC_TYPE_INTF_EJECT			0x11
#define GB_SVC_TYPE_PING			0x13
#define GB_SVC_TYPE_PWRMON_RAIL_COUNT_GET	0x14
#define GB_SVC_TYPE_PWRMON_RAIL_NAMES_GET	0x15
#define GB_SVC_TYPE_PWRMON_SAMPLE_GET		0x16
#define GB_SVC_TYPE_PWRMON_INTF_SAMPLE_GET	0x17
#define GB_SVC_TYPE_TIMESYNC_WAKE_PINS_ACQUIRE	0x18
#define GB_SVC_TYPE_TIMESYNC_WAKE_PINS_RELEASE	0x19
#define GB_SVC_TYPE_TIMESYNC_PING		0x1a
#define GB_SVC_TYPE_MODULE_INSERTED		0x1f
#define GB_SVC_TYPE_MODULE_REMOVED		0x20
#define GB_SVC_TYPE_INTF_VSYS_ENABLE		0x21
#define GB_SVC_TYPE_INTF_VSYS_DISABLE		0x22
#define GB_SVC_TYPE_INTF_REFCLK_ENABLE		0x23
#define GB_SVC_TYPE_INTF_REFCLK_DISABLE		0x24
#define GB_SVC_TYPE_INTF_UNIPRO_ENABLE		0x25
#define GB_SVC_TYPE_INTF_UNIPRO_DISABLE		0x26
#define GB_SVC_TYPE_INTF_ACTIVATE		0x27
#define GB_SVC_TYPE_INTF_RESUME			0x28
#define GB_SVC_TYPE_INTF_MAILBOX_EVENT		0x29
#define GB_SVC_TYPE_INTF_OOPS			0x2a


#define GB_SVC_OP_SUCCESS			0x00
#define GB_SVC_OP_UNKNOWN_ERROR			0x01
#define GB_SVC_INTF_NOT_DETECTED		0x02
#define GB_SVC_INTF_NO_UPRO_LINK		0x03
#define GB_SVC_INTF_UPRO_NOT_DOWN		0x04
#define GB_SVC_INTF_UPRO_NOT_HIBERNATED		0x05
#define GB_SVC_INTF_NO_V_SYS			0x06
#define GB_SVC_INTF_V_CHG			0x07
#define GB_SVC_INTF_WAKE_BUSY			0x08
#define GB_SVC_INTF_NO_REFCLK			0x09
#define GB_SVC_INTF_RELEASING			0x0a
#define GB_SVC_INTF_NO_ORDER			0x0b
#define GB_SVC_INTF_MBOX_SET			0x0c
#define GB_SVC_INTF_BAD_MBOX			0x0d
#define GB_SVC_INTF_OP_TIMEOUT			0x0e
#define GB_SVC_PWRMON_OP_NOT_PRESENT		0x0f

struct gb_svc_version_request {
	__u8	major;
	__u8	minor;
} __packed;

struct gb_svc_version_response {
	__u8	major;
	__u8	minor;
} __packed;


struct gb_svc_hello_request {
	__le16			endo_id;
	__u8			interface_id;
} __packed;


struct gb_svc_intf_device_id_request {
	__u8	intf_id;
	__u8	device_id;
} __packed;


struct gb_svc_intf_reset_request {
	__u8	intf_id;
} __packed;


struct gb_svc_intf_eject_request {
	__u8	intf_id;
} __packed;


struct gb_svc_conn_create_request {
	__u8	intf1_id;
	__le16	cport1_id;
	__u8	intf2_id;
	__le16	cport2_id;
	__u8	tc;
	__u8	flags;
} __packed;


struct gb_svc_conn_destroy_request {
	__u8	intf1_id;
	__le16	cport1_id;
	__u8	intf2_id;
	__le16	cport2_id;
} __packed;


struct gb_svc_dme_peer_get_request {
	__u8	intf_id;
	__le16	attr;
	__le16	selector;
} __packed;

struct gb_svc_dme_peer_get_response {
	__le16	result_code;
	__le32	attr_value;
} __packed;

struct gb_svc_dme_peer_set_request {
	__u8	intf_id;
	__le16	attr;
	__le16	selector;
	__le32	value;
} __packed;

struct gb_svc_dme_peer_set_response {
	__le16	result_code;
} __packed;


#define GB_INIT_SPI_BOOT_STARTED			0x02
#define GB_INIT_TRUSTED_SPI_BOOT_FINISHED		0x03
#define GB_INIT_UNTRUSTED_SPI_BOOT_FINISHED		0x04
#define GB_INIT_BOOTROM_UNIPRO_BOOT_STARTED		0x06
#define GB_INIT_BOOTROM_FALLBACK_UNIPRO_BOOT_STARTED	0x09
#define GB_INIT_S2_LOADER_BOOT_STARTED			0x0D

struct gb_svc_route_create_request {
	__u8	intf1_id;
	__u8	dev1_id;
	__u8	intf2_id;
	__u8	dev2_id;
} __packed;


struct gb_svc_route_destroy_request {
	__u8	intf1_id;
	__u8	intf2_id;
} __packed;



struct gb_svc_intf_vsys_request {
	__u8	intf_id;
} __packed;

struct gb_svc_intf_vsys_response {
	__u8	result_code;
#define GB_SVC_INTF_VSYS_OK				0x00
	
#define GB_SVC_INTF_VSYS_FAIL				0x02
} __packed;


struct gb_svc_intf_refclk_request {
	__u8	intf_id;
} __packed;

struct gb_svc_intf_refclk_response {
	__u8	result_code;
#define GB_SVC_INTF_REFCLK_OK				0x00
	
#define GB_SVC_INTF_REFCLK_FAIL				0x02
} __packed;


struct gb_svc_intf_unipro_request {
	__u8	intf_id;
} __packed;

struct gb_svc_intf_unipro_response {
	__u8	result_code;
#define GB_SVC_INTF_UNIPRO_OK				0x00
	
#define GB_SVC_INTF_UNIPRO_FAIL				0x02
#define GB_SVC_INTF_UNIPRO_NOT_OFF			0x03
} __packed;

#define GB_SVC_UNIPRO_FAST_MODE			0x01
#define GB_SVC_UNIPRO_SLOW_MODE			0x02
#define GB_SVC_UNIPRO_FAST_AUTO_MODE		0x04
#define GB_SVC_UNIPRO_SLOW_AUTO_MODE		0x05
#define GB_SVC_UNIPRO_MODE_UNCHANGED		0x07
#define GB_SVC_UNIPRO_HIBERNATE_MODE		0x11
#define GB_SVC_UNIPRO_OFF_MODE			0x12

#define GB_SVC_SMALL_AMPLITUDE          0x01
#define GB_SVC_LARGE_AMPLITUDE          0x02

#define GB_SVC_NO_DE_EMPHASIS           0x00
#define GB_SVC_SMALL_DE_EMPHASIS        0x01
#define GB_SVC_LARGE_DE_EMPHASIS        0x02

#define GB_SVC_PWRM_RXTERMINATION		0x01
#define GB_SVC_PWRM_TXTERMINATION		0x02
#define GB_SVC_PWRM_LINE_RESET			0x04
#define GB_SVC_PWRM_SCRAMBLING			0x20

#define GB_SVC_PWRM_QUIRK_HSSER			0x00000001

#define GB_SVC_UNIPRO_HS_SERIES_A		0x01
#define GB_SVC_UNIPRO_HS_SERIES_B		0x02

#define GB_SVC_SETPWRM_PWR_OK           0x00
#define GB_SVC_SETPWRM_PWR_LOCAL        0x01
#define GB_SVC_SETPWRM_PWR_REMOTE       0x02
#define GB_SVC_SETPWRM_PWR_BUSY         0x03
#define GB_SVC_SETPWRM_PWR_ERROR_CAP    0x04
#define GB_SVC_SETPWRM_PWR_FATAL_ERROR  0x05

struct gb_svc_l2_timer_cfg {
	__le16 tsb_fc0_protection_timeout;
	__le16 tsb_tc0_replay_timeout;
	__le16 tsb_afc0_req_timeout;
	__le16 tsb_fc1_protection_timeout;
	__le16 tsb_tc1_replay_timeout;
	__le16 tsb_afc1_req_timeout;
	__le16 reserved_for_tc2[3];
	__le16 reserved_for_tc3[3];
} __packed;

struct gb_svc_intf_set_pwrm_request {
	__u8	intf_id;
	__u8	hs_series;
	__u8	tx_mode;
	__u8	tx_gear;
	__u8	tx_nlanes;
	__u8	tx_amplitude;
	__u8	tx_hs_equalizer;
	__u8	rx_mode;
	__u8	rx_gear;
	__u8	rx_nlanes;
	__u8	flags;
	__le32	quirks;
	struct gb_svc_l2_timer_cfg local_l2timerdata, remote_l2timerdata;
} __packed;

struct gb_svc_intf_set_pwrm_response {
	__u8	result_code;
} __packed;

struct gb_svc_key_event_request {
	__le16  key_code;
#define GB_KEYCODE_ARA         0x00

	__u8    key_event;
#define GB_SVC_KEY_RELEASED    0x00
#define GB_SVC_KEY_PRESSED     0x01
} __packed;

#define GB_SVC_PWRMON_MAX_RAIL_COUNT		254

struct gb_svc_pwrmon_rail_count_get_response {
	__u8	rail_count;
} __packed;

#define GB_SVC_PWRMON_RAIL_NAME_BUFSIZE		32

struct gb_svc_pwrmon_rail_names_get_response {
	__u8	status;
	__u8	name[][GB_SVC_PWRMON_RAIL_NAME_BUFSIZE];
} __packed;

#define GB_SVC_PWRMON_TYPE_CURR			0x01
#define GB_SVC_PWRMON_TYPE_VOL			0x02
#define GB_SVC_PWRMON_TYPE_PWR			0x03

#define GB_SVC_PWRMON_GET_SAMPLE_OK		0x00
#define GB_SVC_PWRMON_GET_SAMPLE_INVAL		0x01
#define GB_SVC_PWRMON_GET_SAMPLE_NOSUPP		0x02
#define GB_SVC_PWRMON_GET_SAMPLE_HWERR		0x03

struct gb_svc_pwrmon_sample_get_request {
	__u8	rail_id;
	__u8	measurement_type;
} __packed;

struct gb_svc_pwrmon_sample_get_response {
	__u8	result;
	__le32	measurement;
} __packed;

struct gb_svc_pwrmon_intf_sample_get_request {
	__u8	intf_id;
	__u8	measurement_type;
} __packed;

struct gb_svc_pwrmon_intf_sample_get_response {
	__u8	result;
	__le32	measurement;
} __packed;

#define GB_SVC_MODULE_INSERTED_FLAG_NO_PRIMARY	0x0001

struct gb_svc_module_inserted_request {
	__u8	primary_intf_id;
	__u8	intf_count;
	__le16	flags;
} __packed;


struct gb_svc_module_removed_request {
	__u8	primary_intf_id;
} __packed;


struct gb_svc_intf_activate_request {
	__u8	intf_id;
} __packed;

#define GB_SVC_INTF_TYPE_UNKNOWN		0x00
#define GB_SVC_INTF_TYPE_DUMMY			0x01
#define GB_SVC_INTF_TYPE_UNIPRO			0x02
#define GB_SVC_INTF_TYPE_GREYBUS		0x03

struct gb_svc_intf_activate_response {
	__u8	status;
	__u8	intf_type;
} __packed;

struct gb_svc_intf_resume_request {
	__u8	intf_id;
} __packed;

struct gb_svc_intf_resume_response {
	__u8	status;
} __packed;

#define GB_SVC_INTF_MAILBOX_NONE		0x00
#define GB_SVC_INTF_MAILBOX_AP			0x01
#define GB_SVC_INTF_MAILBOX_GREYBUS		0x02

struct gb_svc_intf_mailbox_event_request {
	__u8	intf_id;
	__le16	result_code;
	__le32	mailbox;
} __packed;


struct gb_svc_intf_oops_request {
	__u8	intf_id;
	__u8	reason;
} __packed;






#define	GB_RAW_TYPE_SEND			0x02

struct gb_raw_send_request {
	__le32	len;
	__u8	data[];
} __packed;





#define GB_UART_TYPE_SEND_DATA			0x02
#define GB_UART_TYPE_RECEIVE_DATA		0x03	
#define GB_UART_TYPE_SET_LINE_CODING		0x04
#define GB_UART_TYPE_SET_CONTROL_LINE_STATE	0x05
#define GB_UART_TYPE_SEND_BREAK			0x06
#define GB_UART_TYPE_SERIAL_STATE		0x07	
#define GB_UART_TYPE_RECEIVE_CREDITS		0x08
#define GB_UART_TYPE_FLUSH_FIFOS		0x09


struct gb_uart_send_data_request {
	__le16	size;
	__u8	data[];
} __packed;


#define GB_UART_RECV_FLAG_FRAMING		0x01	
#define GB_UART_RECV_FLAG_PARITY		0x02	
#define GB_UART_RECV_FLAG_OVERRUN		0x04	
#define GB_UART_RECV_FLAG_BREAK			0x08	


struct gb_uart_recv_data_request {
	__le16	size;
	__u8	flags;
	__u8	data[];
} __packed;

struct gb_uart_receive_credits_request {
	__le16  count;
} __packed;

struct gb_uart_set_line_coding_request {
	__le32	rate;
	__u8	format;
#define GB_SERIAL_1_STOP_BITS			0
#define GB_SERIAL_1_5_STOP_BITS			1
#define GB_SERIAL_2_STOP_BITS			2

	__u8	parity;
#define GB_SERIAL_NO_PARITY			0
#define GB_SERIAL_ODD_PARITY			1
#define GB_SERIAL_EVEN_PARITY			2
#define GB_SERIAL_MARK_PARITY			3
#define GB_SERIAL_SPACE_PARITY			4

	__u8	data_bits;

	__u8	flow_control;
#define GB_SERIAL_AUTO_RTSCTS_EN		0x1
} __packed;


#define GB_UART_CTRL_DTR			0x01
#define GB_UART_CTRL_RTS			0x02

struct gb_uart_set_control_line_state_request {
	__u8	control;
} __packed;

struct gb_uart_set_break_request {
	__u8	state;
} __packed;


#define GB_UART_CTRL_DCD			0x01
#define GB_UART_CTRL_DSR			0x02
#define GB_UART_CTRL_RI				0x04

struct gb_uart_serial_state_request {
	__u8	control;
} __packed;

struct gb_uart_serial_flush_request {
	__u8    flags;
#define GB_SERIAL_FLAG_FLUSH_TRANSMITTER	0x01
#define GB_SERIAL_FLAG_FLUSH_RECEIVER		0x02
} __packed;




#define GB_LOOPBACK_TYPE_PING			0x02
#define GB_LOOPBACK_TYPE_TRANSFER		0x03
#define GB_LOOPBACK_TYPE_SINK			0x04


struct gb_loopback_transfer_request {
	__le32	len;
	__le32  reserved0;
	__le32  reserved1;
	__u8	data[];
} __packed;

struct gb_loopback_transfer_response {
	__le32	len;
	__le32	reserved0;
	__le32	reserved1;
	__u8	data[];
} __packed;



#define GB_SDIO_TYPE_GET_CAPABILITIES		0x02
#define GB_SDIO_TYPE_SET_IOS			0x03
#define GB_SDIO_TYPE_COMMAND			0x04
#define GB_SDIO_TYPE_TRANSFER			0x05
#define GB_SDIO_TYPE_EVENT			0x06


struct gb_sdio_get_caps_response {
	__le32	caps;
#define GB_SDIO_CAP_NONREMOVABLE	0x00000001
#define GB_SDIO_CAP_4_BIT_DATA		0x00000002
#define GB_SDIO_CAP_8_BIT_DATA		0x00000004
#define GB_SDIO_CAP_MMC_HS		0x00000008
#define GB_SDIO_CAP_SD_HS		0x00000010
#define GB_SDIO_CAP_ERASE		0x00000020
#define GB_SDIO_CAP_1_2V_DDR		0x00000040
#define GB_SDIO_CAP_1_8V_DDR		0x00000080
#define GB_SDIO_CAP_POWER_OFF_CARD	0x00000100
#define GB_SDIO_CAP_UHS_SDR12		0x00000200
#define GB_SDIO_CAP_UHS_SDR25		0x00000400
#define GB_SDIO_CAP_UHS_SDR50		0x00000800
#define GB_SDIO_CAP_UHS_SDR104		0x00001000
#define GB_SDIO_CAP_UHS_DDR50		0x00002000
#define GB_SDIO_CAP_DRIVER_TYPE_A	0x00004000
#define GB_SDIO_CAP_DRIVER_TYPE_C	0x00008000
#define GB_SDIO_CAP_DRIVER_TYPE_D	0x00010000
#define GB_SDIO_CAP_HS200_1_2V		0x00020000
#define GB_SDIO_CAP_HS200_1_8V		0x00040000
#define GB_SDIO_CAP_HS400_1_2V		0x00080000
#define GB_SDIO_CAP_HS400_1_8V		0x00100000

	
	__le32 ocr;
	__le32 f_min;
	__le32 f_max;
	__le16 max_blk_count;
	__le16 max_blk_size;
} __packed;


struct gb_sdio_set_ios_request {
	__le32	clock;
	__le32	vdd;
#define GB_SDIO_VDD_165_195	0x00000001
#define GB_SDIO_VDD_20_21	0x00000002
#define GB_SDIO_VDD_21_22	0x00000004
#define GB_SDIO_VDD_22_23	0x00000008
#define GB_SDIO_VDD_23_24	0x00000010
#define GB_SDIO_VDD_24_25	0x00000020
#define GB_SDIO_VDD_25_26	0x00000040
#define GB_SDIO_VDD_26_27	0x00000080
#define GB_SDIO_VDD_27_28	0x00000100
#define GB_SDIO_VDD_28_29	0x00000200
#define GB_SDIO_VDD_29_30	0x00000400
#define GB_SDIO_VDD_30_31	0x00000800
#define GB_SDIO_VDD_31_32	0x00001000
#define GB_SDIO_VDD_32_33	0x00002000
#define GB_SDIO_VDD_33_34	0x00004000
#define GB_SDIO_VDD_34_35	0x00008000
#define GB_SDIO_VDD_35_36	0x00010000

	__u8	bus_mode;
#define GB_SDIO_BUSMODE_OPENDRAIN	0x00
#define GB_SDIO_BUSMODE_PUSHPULL	0x01

	__u8	power_mode;
#define GB_SDIO_POWER_OFF	0x00
#define GB_SDIO_POWER_UP	0x01
#define GB_SDIO_POWER_ON	0x02
#define GB_SDIO_POWER_UNDEFINED	0x03

	__u8	bus_width;
#define GB_SDIO_BUS_WIDTH_1	0x00
#define GB_SDIO_BUS_WIDTH_4	0x02
#define GB_SDIO_BUS_WIDTH_8	0x03

	__u8	timing;
#define GB_SDIO_TIMING_LEGACY		0x00
#define GB_SDIO_TIMING_MMC_HS		0x01
#define GB_SDIO_TIMING_SD_HS		0x02
#define GB_SDIO_TIMING_UHS_SDR12	0x03
#define GB_SDIO_TIMING_UHS_SDR25	0x04
#define GB_SDIO_TIMING_UHS_SDR50	0x05
#define GB_SDIO_TIMING_UHS_SDR104	0x06
#define GB_SDIO_TIMING_UHS_DDR50	0x07
#define GB_SDIO_TIMING_MMC_DDR52	0x08
#define GB_SDIO_TIMING_MMC_HS200	0x09
#define GB_SDIO_TIMING_MMC_HS400	0x0A

	__u8	signal_voltage;
#define GB_SDIO_SIGNAL_VOLTAGE_330	0x00
#define GB_SDIO_SIGNAL_VOLTAGE_180	0x01
#define GB_SDIO_SIGNAL_VOLTAGE_120	0x02

	__u8	drv_type;
#define GB_SDIO_SET_DRIVER_TYPE_B	0x00
#define GB_SDIO_SET_DRIVER_TYPE_A	0x01
#define GB_SDIO_SET_DRIVER_TYPE_C	0x02
#define GB_SDIO_SET_DRIVER_TYPE_D	0x03
} __packed;


struct gb_sdio_command_request {
	__u8	cmd;
	__u8	cmd_flags;
#define GB_SDIO_RSP_NONE		0x00
#define GB_SDIO_RSP_PRESENT		0x01
#define GB_SDIO_RSP_136			0x02
#define GB_SDIO_RSP_CRC			0x04
#define GB_SDIO_RSP_BUSY		0x08
#define GB_SDIO_RSP_OPCODE		0x10

	__u8	cmd_type;
#define GB_SDIO_CMD_AC		0x00
#define GB_SDIO_CMD_ADTC	0x01
#define GB_SDIO_CMD_BC		0x02
#define GB_SDIO_CMD_BCR		0x03

	__le32	cmd_arg;
	__le16	data_blocks;
	__le16	data_blksz;
} __packed;

struct gb_sdio_command_response {
	__le32	resp[4];
} __packed;


struct gb_sdio_transfer_request {
	__u8	data_flags;
#define GB_SDIO_DATA_WRITE	0x01
#define GB_SDIO_DATA_READ	0x02
#define GB_SDIO_DATA_STREAM	0x04

	__le16	data_blocks;
	__le16	data_blksz;
	__u8	data[];
} __packed;

struct gb_sdio_transfer_response {
	__le16	data_blocks;
	__le16	data_blksz;
	__u8	data[];
} __packed;


struct gb_sdio_event_request {
	__u8	event;
#define GB_SDIO_CARD_INSERTED	0x01
#define GB_SDIO_CARD_REMOVED	0x02
#define GB_SDIO_WP		0x04
} __packed;




#define GB_CAMERA_TYPE_CAPABILITIES		0x02
#define GB_CAMERA_TYPE_CONFIGURE_STREAMS	0x03
#define GB_CAMERA_TYPE_CAPTURE			0x04
#define GB_CAMERA_TYPE_FLUSH			0x05
#define GB_CAMERA_TYPE_METADATA			0x06

#define GB_CAMERA_MAX_STREAMS			4
#define GB_CAMERA_MAX_SETTINGS_SIZE		8192


struct gb_camera_stream_config_request {
	__le16 width;
	__le16 height;
	__le16 format;
	__le16 padding;
} __packed;

struct gb_camera_configure_streams_request {
	__u8 num_streams;
	__u8 flags;
#define GB_CAMERA_CONFIGURE_STREAMS_TEST_ONLY	0x01
	__le16 padding;
	struct gb_camera_stream_config_request config[];
} __packed;


struct gb_camera_stream_config_response {
	__le16 width;
	__le16 height;
	__le16 format;
	__u8 virtual_channel;
	__u8 data_type[2];
	__le16 max_pkt_size;
	__u8 padding;
	__le32 max_size;
} __packed;

struct gb_camera_configure_streams_response {
	__u8 num_streams;
#define GB_CAMERA_CONFIGURE_STREAMS_ADJUSTED	0x01
	__u8 flags;
	__u8 padding[2];
	__le32 data_rate;
	struct gb_camera_stream_config_response config[];
};


struct gb_camera_capture_request {
	__le32 request_id;
	__u8 streams;
	__u8 padding;
	__le16 num_frames;
	__u8 settings[];
} __packed;


struct gb_camera_flush_response {
	__le32 request_id;
} __packed;


struct gb_camera_metadata_request {
	__le32 request_id;
	__le16 frame_number;
	__u8 stream;
	__u8 padding;
	__u8 metadata[];
} __packed;




#define GB_LIGHTS_TYPE_GET_LIGHTS		0x02
#define GB_LIGHTS_TYPE_GET_LIGHT_CONFIG		0x03
#define GB_LIGHTS_TYPE_GET_CHANNEL_CONFIG	0x04
#define GB_LIGHTS_TYPE_GET_CHANNEL_FLASH_CONFIG	0x05
#define GB_LIGHTS_TYPE_SET_BRIGHTNESS		0x06
#define GB_LIGHTS_TYPE_SET_BLINK		0x07
#define GB_LIGHTS_TYPE_SET_COLOR		0x08
#define GB_LIGHTS_TYPE_SET_FADE			0x09
#define GB_LIGHTS_TYPE_EVENT			0x0A
#define GB_LIGHTS_TYPE_SET_FLASH_INTENSITY	0x0B
#define GB_LIGHTS_TYPE_SET_FLASH_STROBE		0x0C
#define GB_LIGHTS_TYPE_SET_FLASH_TIMEOUT	0x0D
#define GB_LIGHTS_TYPE_GET_FLASH_FAULT		0x0E




#define GB_CHANNEL_MODE_NONE		0x00000000
#define GB_CHANNEL_MODE_BATTERY		0x00000001
#define GB_CHANNEL_MODE_POWER		0x00000002
#define GB_CHANNEL_MODE_WIRELESS	0x00000004
#define GB_CHANNEL_MODE_BLUETOOTH	0x00000008
#define GB_CHANNEL_MODE_KEYBOARD	0x00000010
#define GB_CHANNEL_MODE_BUTTONS		0x00000020
#define GB_CHANNEL_MODE_NOTIFICATION	0x00000040
#define GB_CHANNEL_MODE_ATTENTION	0x00000080
#define GB_CHANNEL_MODE_FLASH		0x00000100
#define GB_CHANNEL_MODE_TORCH		0x00000200
#define GB_CHANNEL_MODE_INDICATOR	0x00000400


#define GB_CHANNEL_MODE_DEFINED_RANGE	0x000004FF
#define GB_CHANNEL_MODE_VENDOR_RANGE	0x00F00000


#define GB_LIGHT_CHANNEL_MULTICOLOR	0x00000001
#define GB_LIGHT_CHANNEL_FADER		0x00000002
#define GB_LIGHT_CHANNEL_BLINK		0x00000004


struct gb_lights_get_lights_response {
	__u8	lights_count;
} __packed;


struct gb_lights_get_light_config_request {
	__u8	id;
} __packed;


struct gb_lights_get_light_config_response {
	__u8	channel_count;
	__u8	name[32];
} __packed;


struct gb_lights_get_channel_config_request {
	__u8	light_id;
	__u8	channel_id;
} __packed;


struct gb_lights_get_channel_flash_config_request {
	__u8	light_id;
	__u8	channel_id;
} __packed;


struct gb_lights_get_channel_config_response {
	__u8	max_brightness;
	__le32	flags;
	__le32	color;
	__u8	color_name[32];
	__le32	mode;
	__u8	mode_name[32];
} __packed;


struct gb_lights_get_channel_flash_config_response {
	__le32	intensity_min_uA;
	__le32	intensity_max_uA;
	__le32	intensity_step_uA;
	__le32	timeout_min_us;
	__le32	timeout_max_us;
	__le32	timeout_step_us;
} __packed;


struct gb_lights_blink_request {
	__u8	light_id;
	__u8	channel_id;
	__le16	time_on_ms;
	__le16	time_off_ms;
} __packed;


struct gb_lights_set_brightness_request {
	__u8	light_id;
	__u8	channel_id;
	__u8	brightness;
} __packed;


struct gb_lights_set_color_request {
	__u8	light_id;
	__u8	channel_id;
	__le32	color;
} __packed;


struct gb_lights_set_fade_request {
	__u8	light_id;
	__u8	channel_id;
	__u8	fade_in;
	__u8	fade_out;
} __packed;


struct gb_lights_event_request {
	__u8	light_id;
	__u8	event;
#define GB_LIGHTS_LIGHT_CONFIG		0x01
} __packed;


struct gb_lights_set_flash_intensity_request {
	__u8	light_id;
	__u8	channel_id;
	__le32	intensity_uA;
} __packed;


struct gb_lights_set_flash_strobe_request {
	__u8	light_id;
	__u8	channel_id;
	__u8	state;
} __packed;


struct gb_lights_set_flash_timeout_request {
	__u8	light_id;
	__u8	channel_id;
	__le32	timeout_us;
} __packed;


struct gb_lights_get_flash_fault_request {
	__u8	light_id;
	__u8	channel_id;
} __packed;


struct gb_lights_get_flash_fault_response {
	__le32	fault;
#define GB_LIGHTS_FLASH_FAULT_OVER_VOLTAGE		0x00000000
#define GB_LIGHTS_FLASH_FAULT_TIMEOUT			0x00000001
#define GB_LIGHTS_FLASH_FAULT_OVER_TEMPERATURE		0x00000002
#define GB_LIGHTS_FLASH_FAULT_SHORT_CIRCUIT		0x00000004
#define GB_LIGHTS_FLASH_FAULT_OVER_CURRENT		0x00000008
#define GB_LIGHTS_FLASH_FAULT_INDICATOR			0x00000010
#define GB_LIGHTS_FLASH_FAULT_UNDER_VOLTAGE		0x00000020
#define GB_LIGHTS_FLASH_FAULT_INPUT_VOLTAGE		0x00000040
#define GB_LIGHTS_FLASH_FAULT_LED_OVER_TEMPERATURE	0x00000080
} __packed;



#define GB_AUDIO_TYPE_GET_TOPOLOGY_SIZE		0x02
#define GB_AUDIO_TYPE_GET_TOPOLOGY		0x03
#define GB_AUDIO_TYPE_GET_CONTROL		0x04
#define GB_AUDIO_TYPE_SET_CONTROL		0x05
#define GB_AUDIO_TYPE_ENABLE_WIDGET		0x06
#define GB_AUDIO_TYPE_DISABLE_WIDGET		0x07
#define GB_AUDIO_TYPE_GET_PCM			0x08
#define GB_AUDIO_TYPE_SET_PCM			0x09
#define GB_AUDIO_TYPE_SET_TX_DATA_SIZE		0x0a
						
#define GB_AUDIO_TYPE_ACTIVATE_TX		0x0c
#define GB_AUDIO_TYPE_DEACTIVATE_TX		0x0d
#define GB_AUDIO_TYPE_SET_RX_DATA_SIZE		0x0e
						
#define GB_AUDIO_TYPE_ACTIVATE_RX		0x10
#define GB_AUDIO_TYPE_DEACTIVATE_RX		0x11
#define GB_AUDIO_TYPE_JACK_EVENT		0x12
#define GB_AUDIO_TYPE_BUTTON_EVENT		0x13
#define GB_AUDIO_TYPE_STREAMING_EVENT		0x14
#define GB_AUDIO_TYPE_SEND_DATA			0x15


#define GB_AUDIO_SAMPLE_BUFFER_MIN_US		10000

#define GB_AUDIO_PCM_NAME_MAX			32
#define AUDIO_DAI_NAME_MAX			32
#define AUDIO_CONTROL_NAME_MAX			32
#define AUDIO_CTL_ELEM_NAME_MAX			44
#define AUDIO_ENUM_NAME_MAX			64
#define AUDIO_WIDGET_NAME_MAX			32


#define GB_AUDIO_PCM_FMT_S8			BIT(0)
#define GB_AUDIO_PCM_FMT_U8			BIT(1)
#define GB_AUDIO_PCM_FMT_S16_LE			BIT(2)
#define GB_AUDIO_PCM_FMT_S16_BE			BIT(3)
#define GB_AUDIO_PCM_FMT_U16_LE			BIT(4)
#define GB_AUDIO_PCM_FMT_U16_BE			BIT(5)
#define GB_AUDIO_PCM_FMT_S24_LE			BIT(6)
#define GB_AUDIO_PCM_FMT_S24_BE			BIT(7)
#define GB_AUDIO_PCM_FMT_U24_LE			BIT(8)
#define GB_AUDIO_PCM_FMT_U24_BE			BIT(9)
#define GB_AUDIO_PCM_FMT_S32_LE			BIT(10)
#define GB_AUDIO_PCM_FMT_S32_BE			BIT(11)
#define GB_AUDIO_PCM_FMT_U32_LE			BIT(12)
#define GB_AUDIO_PCM_FMT_U32_BE			BIT(13)


#define GB_AUDIO_PCM_RATE_5512			BIT(0)
#define GB_AUDIO_PCM_RATE_8000			BIT(1)
#define GB_AUDIO_PCM_RATE_11025			BIT(2)
#define GB_AUDIO_PCM_RATE_16000			BIT(3)
#define GB_AUDIO_PCM_RATE_22050			BIT(4)
#define GB_AUDIO_PCM_RATE_32000			BIT(5)
#define GB_AUDIO_PCM_RATE_44100			BIT(6)
#define GB_AUDIO_PCM_RATE_48000			BIT(7)
#define GB_AUDIO_PCM_RATE_64000			BIT(8)
#define GB_AUDIO_PCM_RATE_88200			BIT(9)
#define GB_AUDIO_PCM_RATE_96000			BIT(10)
#define GB_AUDIO_PCM_RATE_176400		BIT(11)
#define GB_AUDIO_PCM_RATE_192000		BIT(12)

#define GB_AUDIO_STREAM_TYPE_CAPTURE		0x1
#define GB_AUDIO_STREAM_TYPE_PLAYBACK		0x2

#define GB_AUDIO_CTL_ELEM_ACCESS_READ		BIT(0)
#define GB_AUDIO_CTL_ELEM_ACCESS_WRITE		BIT(1)


#define GB_AUDIO_CTL_ELEM_TYPE_BOOLEAN		0x01
#define GB_AUDIO_CTL_ELEM_TYPE_INTEGER		0x02
#define GB_AUDIO_CTL_ELEM_TYPE_ENUMERATED	0x03
#define GB_AUDIO_CTL_ELEM_TYPE_INTEGER64	0x06


#define GB_AUDIO_CTL_ELEM_IFACE_CARD		0x00
#define GB_AUDIO_CTL_ELEM_IFACE_HWDEP		0x01
#define GB_AUDIO_CTL_ELEM_IFACE_MIXER		0x02
#define GB_AUDIO_CTL_ELEM_IFACE_PCM		0x03
#define GB_AUDIO_CTL_ELEM_IFACE_RAWMIDI		0x04
#define GB_AUDIO_CTL_ELEM_IFACE_TIMER		0x05
#define GB_AUDIO_CTL_ELEM_IFACE_SEQUENCER	0x06


#define GB_AUDIO_ACCESS_READ			BIT(0)
#define GB_AUDIO_ACCESS_WRITE			BIT(1)
#define GB_AUDIO_ACCESS_VOLATILE		BIT(2)
#define GB_AUDIO_ACCESS_TIMESTAMP		BIT(3)
#define GB_AUDIO_ACCESS_TLV_READ		BIT(4)
#define GB_AUDIO_ACCESS_TLV_WRITE		BIT(5)
#define GB_AUDIO_ACCESS_TLV_COMMAND		BIT(6)
#define GB_AUDIO_ACCESS_INACTIVE		BIT(7)
#define GB_AUDIO_ACCESS_LOCK			BIT(8)
#define GB_AUDIO_ACCESS_OWNER			BIT(9)


#define GB_AUDIO_WIDGET_TYPE_INPUT		0x0
#define GB_AUDIO_WIDGET_TYPE_OUTPUT		0x1
#define GB_AUDIO_WIDGET_TYPE_MUX		0x2
#define GB_AUDIO_WIDGET_TYPE_VIRT_MUX		0x3
#define GB_AUDIO_WIDGET_TYPE_VALUE_MUX		0x4
#define GB_AUDIO_WIDGET_TYPE_MIXER		0x5
#define GB_AUDIO_WIDGET_TYPE_MIXER_NAMED_CTL	0x6
#define GB_AUDIO_WIDGET_TYPE_PGA		0x7
#define GB_AUDIO_WIDGET_TYPE_OUT_DRV		0x8
#define GB_AUDIO_WIDGET_TYPE_ADC		0x9
#define GB_AUDIO_WIDGET_TYPE_DAC		0xa
#define GB_AUDIO_WIDGET_TYPE_MICBIAS		0xb
#define GB_AUDIO_WIDGET_TYPE_MIC		0xc
#define GB_AUDIO_WIDGET_TYPE_HP			0xd
#define GB_AUDIO_WIDGET_TYPE_SPK		0xe
#define GB_AUDIO_WIDGET_TYPE_LINE		0xf
#define GB_AUDIO_WIDGET_TYPE_SWITCH		0x10
#define GB_AUDIO_WIDGET_TYPE_VMID		0x11
#define GB_AUDIO_WIDGET_TYPE_PRE		0x12
#define GB_AUDIO_WIDGET_TYPE_POST		0x13
#define GB_AUDIO_WIDGET_TYPE_SUPPLY		0x14
#define GB_AUDIO_WIDGET_TYPE_REGULATOR_SUPPLY	0x15
#define GB_AUDIO_WIDGET_TYPE_CLOCK_SUPPLY	0x16
#define GB_AUDIO_WIDGET_TYPE_AIF_IN		0x17
#define GB_AUDIO_WIDGET_TYPE_AIF_OUT		0x18
#define GB_AUDIO_WIDGET_TYPE_SIGGEN		0x19
#define GB_AUDIO_WIDGET_TYPE_DAI_IN		0x1a
#define GB_AUDIO_WIDGET_TYPE_DAI_OUT		0x1b
#define GB_AUDIO_WIDGET_TYPE_DAI_LINK		0x1c

#define GB_AUDIO_WIDGET_STATE_DISABLED		0x01
#define GB_AUDIO_WIDGET_STATE_ENAABLED		0x02

#define GB_AUDIO_JACK_EVENT_INSERTION		0x1
#define GB_AUDIO_JACK_EVENT_REMOVAL		0x2

#define GB_AUDIO_BUTTON_EVENT_PRESS		0x1
#define GB_AUDIO_BUTTON_EVENT_RELEASE		0x2

#define GB_AUDIO_STREAMING_EVENT_UNSPECIFIED	0x1
#define GB_AUDIO_STREAMING_EVENT_HALT		0x2
#define GB_AUDIO_STREAMING_EVENT_INTERNAL_ERROR	0x3
#define GB_AUDIO_STREAMING_EVENT_PROTOCOL_ERROR	0x4
#define GB_AUDIO_STREAMING_EVENT_FAILURE	0x5
#define GB_AUDIO_STREAMING_EVENT_UNDERRUN	0x6
#define GB_AUDIO_STREAMING_EVENT_OVERRUN	0x7
#define GB_AUDIO_STREAMING_EVENT_CLOCKING	0x8
#define GB_AUDIO_STREAMING_EVENT_DATA_LEN	0x9

#define GB_AUDIO_INVALID_INDEX			0xff


#define GB_AUDIO_JACK_HEADPHONE			0x0000001
#define GB_AUDIO_JACK_MICROPHONE		0x0000002
#define GB_AUDIO_JACK_HEADSET			(GB_AUDIO_JACK_HEADPHONE | \
						 GB_AUDIO_JACK_MICROPHONE)
#define GB_AUDIO_JACK_LINEOUT			0x0000004
#define GB_AUDIO_JACK_MECHANICAL		0x0000008
#define GB_AUDIO_JACK_VIDEOOUT			0x0000010
#define GB_AUDIO_JACK_AVOUT			(GB_AUDIO_JACK_LINEOUT | \
						 GB_AUDIO_JACK_VIDEOOUT)
#define GB_AUDIO_JACK_LINEIN			0x0000020
#define GB_AUDIO_JACK_OC_HPHL			0x0000040
#define GB_AUDIO_JACK_OC_HPHR			0x0000080
#define GB_AUDIO_JACK_MICROPHONE2		0x0000200
#define GB_AUDIO_JACK_ANC_HEADPHONE		(GB_AUDIO_JACK_HEADPHONE | \
						 GB_AUDIO_JACK_MICROPHONE | \
						 GB_AUDIO_JACK_MICROPHONE2)

#define GB_AUDIO_JACK_BTN_0			0x4000000
#define GB_AUDIO_JACK_BTN_1			0x2000000
#define GB_AUDIO_JACK_BTN_2			0x1000000
#define GB_AUDIO_JACK_BTN_3			0x0800000

struct gb_audio_pcm {
	__u8	stream_name[GB_AUDIO_PCM_NAME_MAX];
	__le32	formats;	
	__le32	rates;		
	__u8	chan_min;
	__u8	chan_max;
	__u8	sig_bits;	
} __packed;

struct gb_audio_dai {
	__u8			name[AUDIO_DAI_NAME_MAX];
	__le16			data_cport;
	struct gb_audio_pcm	capture;
	struct gb_audio_pcm	playback;
} __packed;

struct gb_audio_integer {
	__le32	min;
	__le32	max;
	__le32	step;
} __packed;

struct gb_audio_integer64 {
	__le64	min;
	__le64	max;
	__le64	step;
} __packed;

struct gb_audio_enumerated {
	__le32	items;
	__le16	names_length;
	__u8	names[];
} __packed;

struct gb_audio_ctl_elem_info { 
	__u8		type;		
	__le16		dimen[4];
	union {
		struct gb_audio_integer		integer;
		struct gb_audio_integer64	integer64;
		struct gb_audio_enumerated	enumerated;
	} value;
} __packed;

struct gb_audio_ctl_elem_value { 
	__le64				timestamp; 
	union {
		__le32	integer_value[2];	
		__le64	integer64_value[2];
		__le32	enumerated_item[2];
	} value;
} __packed;

struct gb_audio_control {
	__u8	name[AUDIO_CONTROL_NAME_MAX];
	__u8	id;		
	__u8	iface;		
	__le16	data_cport;
	__le32	access;		
	__u8    count;		
	__u8	count_values;	
	struct gb_audio_ctl_elem_info	info;
} __packed;

struct gb_audio_widget {
	__u8	name[AUDIO_WIDGET_NAME_MAX];
	__u8	sname[AUDIO_WIDGET_NAME_MAX];
	__u8	id;
	__u8	type;		
	__u8	state;		
	__u8	ncontrols;
	struct gb_audio_control	ctl[];	
} __packed;

struct gb_audio_route {
	__u8	source_id;	
	__u8	destination_id;	
	__u8	control_id;	
	__u8	index;		
} __packed;

struct gb_audio_topology {
	__u8	num_dais;
	__u8	num_controls;
	__u8	num_widgets;
	__u8	num_routes;
	__le32	size_dais;
	__le32	size_controls;
	__le32	size_widgets;
	__le32	size_routes;
	__le32	jack_type;
	
	__u8	data[];
} __packed;

struct gb_audio_get_topology_size_response {
	__le16	size;
} __packed;

struct gb_audio_get_topology_response {
	struct gb_audio_topology	topology;
} __packed;

struct gb_audio_get_control_request {
	__u8	control_id;
	__u8	index;
} __packed;

struct gb_audio_get_control_response {
	struct gb_audio_ctl_elem_value	value;
} __packed;

struct gb_audio_set_control_request {
	__u8	control_id;
	__u8	index;
	struct gb_audio_ctl_elem_value	value;
} __packed;

struct gb_audio_enable_widget_request {
	__u8	widget_id;
} __packed;

struct gb_audio_disable_widget_request {
	__u8	widget_id;
} __packed;

struct gb_audio_get_pcm_request {
	__le16	data_cport;
} __packed;

struct gb_audio_get_pcm_response {
	__le32	format;
	__le32	rate;
	__u8	channels;
	__u8	sig_bits;
} __packed;

struct gb_audio_set_pcm_request {
	__le16	data_cport;
	__le32	format;
	__le32	rate;
	__u8	channels;
	__u8	sig_bits;
} __packed;

struct gb_audio_set_tx_data_size_request {
	__le16	data_cport;
	__le16	size;
} __packed;

struct gb_audio_activate_tx_request {
	__le16	data_cport;
} __packed;

struct gb_audio_deactivate_tx_request {
	__le16	data_cport;
} __packed;

struct gb_audio_set_rx_data_size_request {
	__le16	data_cport;
	__le16	size;
} __packed;

struct gb_audio_activate_rx_request {
	__le16	data_cport;
} __packed;

struct gb_audio_deactivate_rx_request {
	__le16	data_cport;
} __packed;

struct gb_audio_jack_event_request {
	__u8	widget_id;
	__u8	jack_attribute;
	__u8	event;
} __packed;

struct gb_audio_button_event_request {
	__u8	widget_id;
	__u8	button_id;
	__u8	event;
} __packed;

struct gb_audio_streaming_event_request {
	__le16	data_cport;
	__u8	event;
} __packed;

struct gb_audio_send_data_request {
	__le64	timestamp;
	__u8	data[];
} __packed;





#define GB_LOG_TYPE_SEND_LOG	0x02


#define GB_LOG_MAX_LEN		1024

struct gb_log_send_log_request {
	__le16	len;
	__u8    msg[];
} __packed;

#endif 

