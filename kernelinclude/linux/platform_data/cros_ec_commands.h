/* SPDX-License-Identifier: GPL-2.0-only */




#ifndef __CROS_EC_COMMANDS_H
#define __CROS_EC_COMMANDS_H

#include <linux/bits.h>
#include <linux/types.h>

#define BUILD_ASSERT(_cond)


#define EC_PROTO_VERSION          0x00000002


#define EC_VER_MASK(version) BIT(version)


#define EC_LPC_ADDR_ACPI_DATA  0x62
#define EC_LPC_ADDR_ACPI_CMD   0x66


#define EC_LPC_ADDR_HOST_DATA  0x200
#define EC_LPC_ADDR_HOST_CMD   0x204



#define EC_LPC_ADDR_HOST_ARGS    0x800  
#define EC_LPC_ADDR_HOST_PARAM   0x804  

#define EC_LPC_ADDR_HOST_PACKET  0x800  
#define EC_LPC_HOST_PACKET_SIZE  0x100  


#define EC_HOST_CMD_REGION0       0x800
#define EC_HOST_CMD_REGION1       0x880
#define EC_HOST_CMD_REGION_SIZE    0x80
#define EC_HOST_CMD_MEC_REGION_SIZE 0x8


#define EC_LPC_CMDR_DATA	BIT(0)  
#define EC_LPC_CMDR_PENDING	BIT(1)  
#define EC_LPC_CMDR_BUSY	BIT(2)  
#define EC_LPC_CMDR_CMD		BIT(3)  
#define EC_LPC_CMDR_ACPI_BRST	BIT(4)  
#define EC_LPC_CMDR_SCI		BIT(5)  
#define EC_LPC_CMDR_SMI		BIT(6)  

#define EC_LPC_ADDR_MEMMAP       0x900
#define EC_MEMMAP_SIZE         255 
#define EC_MEMMAP_TEXT_MAX     8   


#define EC_MEMMAP_TEMP_SENSOR      0x00 
#define EC_MEMMAP_FAN              0x10 
#define EC_MEMMAP_TEMP_SENSOR_B    0x18 
#define EC_MEMMAP_ID               0x20 
#define EC_MEMMAP_ID_VERSION       0x22 
#define EC_MEMMAP_THERMAL_VERSION  0x23 
#define EC_MEMMAP_BATTERY_VERSION  0x24 
#define EC_MEMMAP_SWITCHES_VERSION 0x25 
#define EC_MEMMAP_EVENTS_VERSION   0x26 
#define EC_MEMMAP_HOST_CMD_FLAGS   0x27 

#define EC_MEMMAP_SWITCHES         0x30	

#define EC_MEMMAP_HOST_EVENTS      0x34 

#define EC_MEMMAP_BATT_VOLT        0x40 
#define EC_MEMMAP_BATT_RATE        0x44 
#define EC_MEMMAP_BATT_CAP         0x48 
#define EC_MEMMAP_BATT_FLAG        0x4c 
#define EC_MEMMAP_BATT_COUNT       0x4d 
#define EC_MEMMAP_BATT_INDEX       0x4e 

#define EC_MEMMAP_BATT_DCAP        0x50 
#define EC_MEMMAP_BATT_DVLT        0x54 
#define EC_MEMMAP_BATT_LFCC        0x58 
#define EC_MEMMAP_BATT_CCNT        0x5c 

#define EC_MEMMAP_BATT_MFGR        0x60 
#define EC_MEMMAP_BATT_MODEL       0x68 
#define EC_MEMMAP_BATT_SERIAL      0x70 
#define EC_MEMMAP_BATT_TYPE        0x78 
#define EC_MEMMAP_ALS              0x80 

#define EC_MEMMAP_ACC_STATUS       0x90 

#define EC_MEMMAP_ACC_DATA         0x92 



#define EC_MEMMAP_GYRO_DATA        0xa0 



#define EC_MEMMAP_NO_ACPI 0xe0


#define EC_MEMMAP_ACC_STATUS_SAMPLE_ID_MASK  0x0f
#define EC_MEMMAP_ACC_STATUS_BUSY_BIT        BIT(4)
#define EC_MEMMAP_ACC_STATUS_PRESENCE_BIT    BIT(7)


#define EC_TEMP_SENSOR_ENTRIES     16

#define EC_TEMP_SENSOR_B_ENTRIES      8


#define EC_TEMP_SENSOR_NOT_PRESENT    0xff
#define EC_TEMP_SENSOR_ERROR          0xfe
#define EC_TEMP_SENSOR_NOT_POWERED    0xfd
#define EC_TEMP_SENSOR_NOT_CALIBRATED 0xfc

#define EC_TEMP_SENSOR_OFFSET      200


#define EC_ALS_ENTRIES             2


#define EC_TEMP_SENSOR_DEFAULT     (296 - EC_TEMP_SENSOR_OFFSET)

#define EC_FAN_SPEED_ENTRIES       4       
#define EC_FAN_SPEED_NOT_PRESENT   0xffff  
#define EC_FAN_SPEED_STALLED       0xfffe  


#define EC_BATT_FLAG_AC_PRESENT   0x01
#define EC_BATT_FLAG_BATT_PRESENT 0x02
#define EC_BATT_FLAG_DISCHARGING  0x04
#define EC_BATT_FLAG_CHARGING     0x08
#define EC_BATT_FLAG_LEVEL_CRITICAL 0x10

#define EC_BATT_FLAG_INVALID_DATA 0x20


#define EC_SWITCH_LID_OPEN               0x01
#define EC_SWITCH_POWER_BUTTON_PRESSED   0x02
#define EC_SWITCH_WRITE_PROTECT_DISABLED 0x04

#define EC_SWITCH_IGNORE1		 0x08

#define EC_SWITCH_DEDICATED_RECOVERY     0x10

#define EC_SWITCH_IGNORE0                0x20



#define EC_HOST_CMD_FLAG_LPC_ARGS_SUPPORTED  0x01

#define EC_HOST_CMD_FLAG_VERSION_3   0x02


#define EC_WIRELESS_SWITCH_ALL       ~0x00  
#define EC_WIRELESS_SWITCH_WLAN       0x01  
#define EC_WIRELESS_SWITCH_BLUETOOTH  0x02  
#define EC_WIRELESS_SWITCH_WWAN       0x04  
#define EC_WIRELESS_SWITCH_WLAN_POWER 0x08  





#define EC_CMD_ACPI_READ 0x0080


#define EC_CMD_ACPI_WRITE 0x0081


#define EC_CMD_ACPI_BURST_ENABLE 0x0082


#define EC_CMD_ACPI_BURST_DISABLE 0x0083


#define EC_CMD_ACPI_QUERY_EVENT 0x0084




#define EC_ACPI_MEM_VERSION            0x00

#define EC_ACPI_MEM_TEST               0x01

#define EC_ACPI_MEM_TEST_COMPLIMENT    0x02


#define EC_ACPI_MEM_KEYBOARD_BACKLIGHT 0x03

#define EC_ACPI_MEM_FAN_DUTY           0x04


#define EC_ACPI_MEM_TEMP_ID            0x05
#define EC_ACPI_MEM_TEMP_THRESHOLD     0x06
#define EC_ACPI_MEM_TEMP_COMMIT        0x07

#define EC_ACPI_MEM_TEMP_COMMIT_SELECT_MASK BIT(0)
#define EC_ACPI_MEM_TEMP_COMMIT_ENABLE_MASK BIT(1)



#define EC_ACPI_MEM_CHARGING_LIMIT     0x08


#define EC_ACPI_MEM_CHARGING_LIMIT_STEP_MA   64

#define EC_ACPI_MEM_CHARGING_LIMIT_DISABLED  0xff


#define EC_ACPI_MEM_DEVICE_ORIENTATION 0x09
#define EC_ACPI_MEM_TBMD_SHIFT         0
#define EC_ACPI_MEM_TBMD_MASK          0x1
#define EC_ACPI_MEM_DDPN_SHIFT         1
#define EC_ACPI_MEM_DDPN_MASK          0x7


#define EC_ACPI_MEM_DEVICE_FEATURES0 0x0a
#define EC_ACPI_MEM_DEVICE_FEATURES1 0x0b
#define EC_ACPI_MEM_DEVICE_FEATURES2 0x0c
#define EC_ACPI_MEM_DEVICE_FEATURES3 0x0d
#define EC_ACPI_MEM_DEVICE_FEATURES4 0x0e
#define EC_ACPI_MEM_DEVICE_FEATURES5 0x0f
#define EC_ACPI_MEM_DEVICE_FEATURES6 0x10
#define EC_ACPI_MEM_DEVICE_FEATURES7 0x11

#define EC_ACPI_MEM_BATTERY_INDEX    0x12


#define EC_ACPI_MEM_USB_PORT_POWER 0x13


#define EC_ACPI_MEM_MAPPED_BEGIN   0x20
#define EC_ACPI_MEM_MAPPED_SIZE    0xe0


#define EC_ACPI_MEM_VERSION_CURRENT 2








#define __ec_align1 __packed
#define __ec_align2 __packed
#define __ec_align4 __packed
#define __ec_align_size1 __packed
#define __ec_align_offset1 __packed
#define __ec_align_offset2 __packed
#define __ec_todo_packed __packed
#define __ec_todo_unpacked




#define EC_LPC_STATUS_TO_HOST     0x01

#define EC_LPC_STATUS_FROM_HOST   0x02

#define EC_LPC_STATUS_PROCESSING  0x04

#define EC_LPC_STATUS_LAST_CMD    0x08

#define EC_LPC_STATUS_BURST_MODE  0x10

#define EC_LPC_STATUS_SCI_PENDING 0x20

#define EC_LPC_STATUS_SMI_PENDING 0x40

#define EC_LPC_STATUS_RESERVED    0x80


#define EC_LPC_STATUS_BUSY_MASK \
	(EC_LPC_STATUS_FROM_HOST | EC_LPC_STATUS_PROCESSING)


enum ec_status {
	EC_RES_SUCCESS = 0,
	EC_RES_INVALID_COMMAND = 1,
	EC_RES_ERROR = 2,
	EC_RES_INVALID_PARAM = 3,
	EC_RES_ACCESS_DENIED = 4,
	EC_RES_INVALID_RESPONSE = 5,
	EC_RES_INVALID_VERSION = 6,
	EC_RES_INVALID_CHECKSUM = 7,
	EC_RES_IN_PROGRESS = 8,		
	EC_RES_UNAVAILABLE = 9,		
	EC_RES_TIMEOUT = 10,		
	EC_RES_OVERFLOW = 11,		
	EC_RES_INVALID_HEADER = 12,     
	EC_RES_REQUEST_TRUNCATED = 13,  
	EC_RES_RESPONSE_TOO_BIG = 14,   
	EC_RES_BUS_ERROR = 15,		
	EC_RES_BUSY = 16,		
	EC_RES_INVALID_HEADER_VERSION = 17,  
	EC_RES_INVALID_HEADER_CRC = 18,      
	EC_RES_INVALID_DATA_CRC = 19,        
	EC_RES_DUP_UNAVAILABLE = 20,         
};


enum host_event_code {
	EC_HOST_EVENT_LID_CLOSED = 1,
	EC_HOST_EVENT_LID_OPEN = 2,
	EC_HOST_EVENT_POWER_BUTTON = 3,
	EC_HOST_EVENT_AC_CONNECTED = 4,
	EC_HOST_EVENT_AC_DISCONNECTED = 5,
	EC_HOST_EVENT_BATTERY_LOW = 6,
	EC_HOST_EVENT_BATTERY_CRITICAL = 7,
	EC_HOST_EVENT_BATTERY = 8,
	EC_HOST_EVENT_THERMAL_THRESHOLD = 9,
	
	EC_HOST_EVENT_DEVICE = 10,
	EC_HOST_EVENT_THERMAL = 11,
	EC_HOST_EVENT_USB_CHARGER = 12,
	EC_HOST_EVENT_KEY_PRESSED = 13,
	
	EC_HOST_EVENT_INTERFACE_READY = 14,
	
	EC_HOST_EVENT_KEYBOARD_RECOVERY = 15,

	
	EC_HOST_EVENT_THERMAL_SHUTDOWN = 16,
	
	EC_HOST_EVENT_BATTERY_SHUTDOWN = 17,

	
	EC_HOST_EVENT_THROTTLE_START = 18,
	
	EC_HOST_EVENT_THROTTLE_STOP = 19,

	
	EC_HOST_EVENT_HANG_DETECT = 20,
	
	EC_HOST_EVENT_HANG_REBOOT = 21,

	
	EC_HOST_EVENT_PD_MCU = 22,

	
	EC_HOST_EVENT_BATTERY_STATUS = 23,

	
	EC_HOST_EVENT_PANIC = 24,

	
	EC_HOST_EVENT_KEYBOARD_FASTBOOT = 25,

	
	EC_HOST_EVENT_RTC = 26,

	
	EC_HOST_EVENT_MKBP = 27,

	
	EC_HOST_EVENT_USB_MUX = 28,

	
	EC_HOST_EVENT_MODE_CHANGE = 29,

	
	EC_HOST_EVENT_KEYBOARD_RECOVERY_HW_REINIT = 30,

	
	EC_HOST_EVENT_WOV = 31,

	
	EC_HOST_EVENT_INVALID = 32
};

#define EC_HOST_EVENT_MASK(event_code) BIT_ULL((event_code) - 1)


struct ec_lpc_host_args {
	uint8_t flags;
	uint8_t command_version;
	uint8_t data_size;
	uint8_t checksum;
} __ec_align4;



#define EC_HOST_ARGS_FLAG_FROM_HOST 0x01

#define EC_HOST_ARGS_FLAG_TO_HOST   0x02





#define EC_SPI_FRAME_START    0xec


#define EC_SPI_PAST_END       0xed


#define EC_SPI_RX_READY       0xf8


#define EC_SPI_RECEIVING      0xf9


#define EC_SPI_PROCESSING     0xfa


#define EC_SPI_RX_BAD_DATA    0xfb


#define EC_SPI_NOT_READY      0xfc


#define EC_SPI_OLD_READY      0xfd




#define EC_PROTO2_REQUEST_HEADER_BYTES 3
#define EC_PROTO2_REQUEST_TRAILER_BYTES 1
#define EC_PROTO2_REQUEST_OVERHEAD (EC_PROTO2_REQUEST_HEADER_BYTES +	\
				    EC_PROTO2_REQUEST_TRAILER_BYTES)

#define EC_PROTO2_RESPONSE_HEADER_BYTES 2
#define EC_PROTO2_RESPONSE_TRAILER_BYTES 1
#define EC_PROTO2_RESPONSE_OVERHEAD (EC_PROTO2_RESPONSE_HEADER_BYTES +	\
				     EC_PROTO2_RESPONSE_TRAILER_BYTES)


#define EC_PROTO2_MAX_PARAM_SIZE 0xfc


#define EC_PROTO2_MAX_REQUEST_SIZE (EC_PROTO2_REQUEST_OVERHEAD +	\
				    EC_PROTO2_MAX_PARAM_SIZE)
#define EC_PROTO2_MAX_RESPONSE_SIZE (EC_PROTO2_RESPONSE_OVERHEAD +	\
				     EC_PROTO2_MAX_PARAM_SIZE)




#define EC_COMMAND_PROTOCOL_3 0xda

#define EC_HOST_REQUEST_VERSION 3


struct ec_host_request {
	uint8_t struct_version;
	uint8_t checksum;
	uint16_t command;
	uint8_t command_version;
	uint8_t reserved;
	uint16_t data_len;
} __ec_align4;

#define EC_HOST_RESPONSE_VERSION 3


struct ec_host_response {
	uint8_t struct_version;
	uint8_t checksum;
	uint16_t result;
	uint16_t data_len;
	uint16_t reserved;
} __ec_align4;






struct ec_host_request4 {
	
	uint8_t fields0;

	
	uint8_t fields1;

	
	uint16_t command;

	
	uint16_t data_len;

	
	uint8_t reserved;

	
	uint8_t header_crc;
} __ec_align4;


struct ec_host_response4 {
	
	uint8_t fields0;

	
	uint8_t fields1;

	
	uint16_t result;

	
	uint16_t data_len;

	
	uint8_t reserved;

	
	uint8_t header_crc;
} __ec_align4;


#define EC_PACKET4_0_STRUCT_VERSION_MASK	0x0f
#define EC_PACKET4_0_IS_RESPONSE_MASK		0x10
#define EC_PACKET4_0_SEQ_NUM_SHIFT		5
#define EC_PACKET4_0_SEQ_NUM_MASK		0x60
#define EC_PACKET4_0_SEQ_DUP_MASK		0x80


#define EC_PACKET4_1_COMMAND_VERSION_MASK	0x1f  
#define EC_PACKET4_1_DATA_CRC_PRESENT_MASK	0x80








#define EC_CMD_PROTO_VERSION 0x0000


struct ec_response_proto_version {
	uint32_t version;
} __ec_align4;


#define EC_CMD_HELLO 0x0001


struct ec_params_hello {
	uint32_t in_data;
} __ec_align4;


struct ec_response_hello {
	uint32_t out_data;
} __ec_align4;


#define EC_CMD_GET_VERSION 0x0002

enum ec_current_image {
	EC_IMAGE_UNKNOWN = 0,
	EC_IMAGE_RO,
	EC_IMAGE_RW
};


struct ec_response_get_version {
	char version_string_ro[32];
	char version_string_rw[32];
	char reserved[32];
	uint32_t current_image;
} __ec_align4;


#define EC_CMD_READ_TEST 0x0003


struct ec_params_read_test {
	uint32_t offset;
	uint32_t size;
} __ec_align4;


struct ec_response_read_test {
	uint32_t data[32];
} __ec_align4;


#define EC_CMD_GET_BUILD_INFO 0x0004


#define EC_CMD_GET_CHIP_INFO 0x0005


struct ec_response_get_chip_info {
	char vendor[32];
	char name[32];
	char revision[32];
} __ec_align4;


#define EC_CMD_GET_BOARD_VERSION 0x0006


struct ec_response_board_version {
	uint16_t board_version;
} __ec_align2;


#define EC_CMD_READ_MEMMAP 0x0007


struct ec_params_read_memmap {
	uint8_t offset;
	uint8_t size;
} __ec_align1;


#define EC_CMD_GET_CMD_VERSIONS 0x0008


struct ec_params_get_cmd_versions {
	uint8_t cmd;
} __ec_align1;


struct ec_params_get_cmd_versions_v1 {
	uint16_t cmd;
} __ec_align2;


struct ec_response_get_cmd_versions {
	uint32_t version_mask;
} __ec_align4;


#define EC_CMD_GET_COMMS_STATUS		0x0009


enum ec_comms_status {
	EC_COMMS_STATUS_PROCESSING	= BIT(0),	
};


struct ec_response_get_comms_status {
	uint32_t flags;		
} __ec_align4;


#define EC_CMD_TEST_PROTOCOL		0x000A


struct ec_params_test_protocol {
	uint32_t ec_result;
	uint32_t ret_len;
	uint8_t buf[32];
} __ec_align4;


struct ec_response_test_protocol {
	uint8_t buf[32];
} __ec_align4;


#define EC_CMD_GET_PROTOCOL_INFO	0x000B



#define EC_PROTOCOL_INFO_IN_PROGRESS_SUPPORTED BIT(0)


struct ec_response_get_protocol_info {
	
	uint32_t protocol_versions;
	uint16_t max_request_packet_size;
	uint16_t max_response_packet_size;
	uint32_t flags;
} __ec_align4;






#define EC_GSV_SET        0x80000000


#define EC_GSV_PARAM_MASK 0x00ffffff

struct ec_params_get_set_value {
	uint32_t flags;
	uint32_t value;
} __ec_align4;

struct ec_response_get_set_value {
	uint32_t flags;
	uint32_t value;
} __ec_align4;


#define EC_CMD_GSV_PAUSE_IN_S5	0x000C



#define EC_CMD_GET_FEATURES  0x000D


enum ec_feature_code {
	
	EC_FEATURE_LIMITED = 0,
	
	EC_FEATURE_FLASH = 1,
	
	EC_FEATURE_PWM_FAN = 2,
	
	EC_FEATURE_PWM_KEYB = 3,
	
	EC_FEATURE_LIGHTBAR = 4,
	
	EC_FEATURE_LED = 5,
	
	EC_FEATURE_MOTION_SENSE = 6,
	
	EC_FEATURE_KEYB = 7,
	
	EC_FEATURE_PSTORE = 8,
	
	EC_FEATURE_PORT80 = 9,
	
	EC_FEATURE_THERMAL = 10,
	
	EC_FEATURE_BKLIGHT_SWITCH = 11,
	
	EC_FEATURE_WIFI_SWITCH = 12,
	
	EC_FEATURE_HOST_EVENTS = 13,
	
	EC_FEATURE_GPIO = 14,
	
	EC_FEATURE_I2C = 15,
	
	EC_FEATURE_CHARGER = 16,
	
	EC_FEATURE_BATTERY = 17,
	
	EC_FEATURE_SMART_BATTERY = 18,
	
	EC_FEATURE_HANG_DETECT = 19,
	
	EC_FEATURE_PMU = 20,
	
	EC_FEATURE_SUB_MCU = 21,
	
	EC_FEATURE_USB_PD = 22,
	
	EC_FEATURE_USB_MUX = 23,
	
	EC_FEATURE_MOTION_SENSE_FIFO = 24,
	
	EC_FEATURE_VSTORE = 25,
	
	EC_FEATURE_USBC_SS_MUX_VIRTUAL = 26,
	
	EC_FEATURE_RTC = 27,
	
	EC_FEATURE_FINGERPRINT = 28,
	
	EC_FEATURE_TOUCHPAD = 29,
	
	EC_FEATURE_RWSIG = 30,
	
	EC_FEATURE_DEVICE_EVENT = 31,
	
	EC_FEATURE_UNIFIED_WAKE_MASKS = 32,
	
	EC_FEATURE_HOST_EVENT64 = 33,
	
	EC_FEATURE_EXEC_IN_RAM = 34,
	
	EC_FEATURE_CEC = 35,
	
	EC_FEATURE_MOTION_SENSE_TIGHT_TIMESTAMPS = 36,
	
	EC_FEATURE_REFINED_TABLET_MODE_HYSTERESIS = 37,
	
	EC_FEATURE_SCP = 39,
	
	EC_FEATURE_ISH = 40,
	
	EC_FEATURE_TYPEC_CMD = 41,
	
	EC_FEATURE_TYPEC_REQUIRE_AP_MODE_ENTRY = 42,
	
	EC_FEATURE_TYPEC_MUX_REQUIRE_AP_ACK = 43,
	
	EC_FEATURE_S4_RESIDENCY = 44,
	
	EC_FEATURE_TYPEC_AP_MUX_SET = 45,
	
	EC_FEATURE_TYPEC_AP_VDM_SEND = 46,
};

#define EC_FEATURE_MASK_0(event_code) BIT(event_code % 32)
#define EC_FEATURE_MASK_1(event_code) BIT(event_code - 32)

struct ec_response_get_features {
	uint32_t flags[2];
} __ec_align4;



#define EC_CMD_GET_SKU_ID 0x000E


#define EC_CMD_SET_SKU_ID 0x000F

struct ec_sku_id_info {
	uint32_t sku_id;
} __ec_align4;





#define EC_CMD_FLASH_INFO 0x0010
#define EC_VER_FLASH_INFO 2


struct ec_response_flash_info {
	uint32_t flash_size;
	uint32_t write_block_size;
	uint32_t erase_block_size;
	uint32_t protect_block_size;
} __ec_align4;


#define EC_FLASH_INFO_ERASE_TO_0 BIT(0)


#define EC_FLASH_INFO_SELECT_REQUIRED BIT(1)


struct ec_response_flash_info_1 {
	
	uint32_t flash_size;
	uint32_t write_block_size;
	uint32_t erase_block_size;
	uint32_t protect_block_size;

	
	uint32_t write_ideal_size;
	uint32_t flags;
} __ec_align4;

struct ec_params_flash_info_2 {
	
	uint16_t num_banks_desc;
	
	uint8_t reserved[2];
} __ec_align4;

struct ec_flash_bank {
	
	uint16_t count;
	
	uint8_t size_exp;
	
	uint8_t write_size_exp;
	
	uint8_t erase_size_exp;
	
	uint8_t protect_size_exp;
	
	uint8_t reserved[2];
};

struct ec_response_flash_info_2 {
	
	uint32_t flash_size;
	
	uint32_t flags;
	
	uint32_t write_ideal_size;
	
	uint16_t num_banks_total;
	
	uint16_t num_banks_desc;
	struct ec_flash_bank banks[];
} __ec_align4;


#define EC_CMD_FLASH_READ 0x0011


struct ec_params_flash_read {
	uint32_t offset;
	uint32_t size;
} __ec_align4;


#define EC_CMD_FLASH_WRITE 0x0012
#define EC_VER_FLASH_WRITE 1


#define EC_FLASH_WRITE_VER0_SIZE 64


struct ec_params_flash_write {
	uint32_t offset;
	uint32_t size;
	
} __ec_align4;


#define EC_CMD_FLASH_ERASE 0x0013


struct ec_params_flash_erase {
	uint32_t offset;
	uint32_t size;
} __ec_align4;


enum ec_flash_erase_cmd {
	FLASH_ERASE_SECTOR,     
	FLASH_ERASE_SECTOR_ASYNC,  
	FLASH_ERASE_GET_RESULT,  
};


struct ec_params_flash_erase_v1 {
	uint8_t  cmd;
	uint8_t  reserved;
	uint16_t flag;
	struct ec_params_flash_erase params;
} __ec_align4;


#define EC_CMD_FLASH_PROTECT 0x0015
#define EC_VER_FLASH_PROTECT 1  



#define EC_FLASH_PROTECT_RO_AT_BOOT         BIT(0)

#define EC_FLASH_PROTECT_RO_NOW             BIT(1)

#define EC_FLASH_PROTECT_ALL_NOW            BIT(2)

#define EC_FLASH_PROTECT_GPIO_ASSERTED      BIT(3)

#define EC_FLASH_PROTECT_ERROR_STUCK        BIT(4)

#define EC_FLASH_PROTECT_ERROR_INCONSISTENT BIT(5)

#define EC_FLASH_PROTECT_ALL_AT_BOOT        BIT(6)

#define EC_FLASH_PROTECT_RW_AT_BOOT         BIT(7)

#define EC_FLASH_PROTECT_RW_NOW             BIT(8)

#define EC_FLASH_PROTECT_ROLLBACK_AT_BOOT   BIT(9)

#define EC_FLASH_PROTECT_ROLLBACK_NOW       BIT(10)



struct ec_params_flash_protect {
	uint32_t mask;
	uint32_t flags;
} __ec_align4;


struct ec_response_flash_protect {
	uint32_t flags;
	uint32_t valid_flags;
	uint32_t writable_flags;
} __ec_align4;




#define EC_CMD_FLASH_REGION_INFO 0x0016
#define EC_VER_FLASH_REGION_INFO 1

enum ec_flash_region {
	
	EC_FLASH_REGION_RO = 0,
	
	EC_FLASH_REGION_ACTIVE,
	
	EC_FLASH_REGION_WP_RO,
	
	EC_FLASH_REGION_UPDATE,
	
	EC_FLASH_REGION_COUNT,
};

#define EC_FLASH_REGION_RW EC_FLASH_REGION_ACTIVE


struct ec_params_flash_region_info {
	uint32_t region;
} __ec_align4;

struct ec_response_flash_region_info {
	uint32_t offset;
	uint32_t size;
} __ec_align4;


#define EC_CMD_VBNV_CONTEXT 0x0017
#define EC_VER_VBNV_CONTEXT 1
#define EC_VBNV_BLOCK_SIZE 16

enum ec_vbnvcontext_op {
	EC_VBNV_CONTEXT_OP_READ,
	EC_VBNV_CONTEXT_OP_WRITE,
};

struct ec_params_vbnvcontext {
	uint32_t op;
	uint8_t block[EC_VBNV_BLOCK_SIZE];
} __ec_align4;

struct ec_response_vbnvcontext {
	uint8_t block[EC_VBNV_BLOCK_SIZE];
} __ec_align4;



#define EC_CMD_FLASH_SPI_INFO 0x0018

struct ec_response_flash_spi_info {
	
	uint8_t jedec[3];

	
	uint8_t reserved0;

	
	uint8_t mfr_dev_id[2];

	
	uint8_t sr1, sr2;
} __ec_align1;



#define EC_CMD_FLASH_SELECT 0x0019


struct ec_params_flash_select {
	uint8_t select;
} __ec_align4;






#define EC_CMD_PWM_GET_FAN_TARGET_RPM 0x0020

struct ec_response_pwm_get_fan_rpm {
	uint32_t rpm;
} __ec_align4;


#define EC_CMD_PWM_SET_FAN_TARGET_RPM 0x0021


struct ec_params_pwm_set_fan_target_rpm_v0 {
	uint32_t rpm;
} __ec_align4;


struct ec_params_pwm_set_fan_target_rpm_v1 {
	uint32_t rpm;
	uint8_t fan_idx;
} __ec_align_size1;



#define EC_CMD_PWM_GET_KEYBOARD_BACKLIGHT 0x0022

struct ec_response_pwm_get_keyboard_backlight {
	uint8_t percent;
	uint8_t enabled;
} __ec_align1;



#define EC_CMD_PWM_SET_KEYBOARD_BACKLIGHT 0x0023

struct ec_params_pwm_set_keyboard_backlight {
	uint8_t percent;
} __ec_align1;


#define EC_CMD_PWM_SET_FAN_DUTY 0x0024


struct ec_params_pwm_set_fan_duty_v0 {
	uint32_t percent;
} __ec_align4;


struct ec_params_pwm_set_fan_duty_v1 {
	uint32_t percent;
	uint8_t fan_idx;
} __ec_align_size1;

#define EC_CMD_PWM_SET_DUTY 0x0025

#define EC_PWM_MAX_DUTY 0xffff

enum ec_pwm_type {
	
	EC_PWM_TYPE_GENERIC = 0,
	
	EC_PWM_TYPE_KB_LIGHT,
	
	EC_PWM_TYPE_DISPLAY_LIGHT,
	EC_PWM_TYPE_COUNT,
};

struct ec_params_pwm_set_duty {
	uint16_t duty;     
	uint8_t pwm_type;  
	uint8_t index;     
} __ec_align4;

#define EC_CMD_PWM_GET_DUTY 0x0026

struct ec_params_pwm_get_duty {
	uint8_t pwm_type;  
	uint8_t index;     
} __ec_align1;

struct ec_response_pwm_get_duty {
	uint16_t duty;     
} __ec_align2;



#define EC_CMD_LIGHTBAR_CMD 0x0028

struct rgb_s {
	uint8_t r, g, b;
} __ec_todo_unpacked;

#define LB_BATTERY_LEVELS 4


struct lightbar_params_v0 {
	
	int32_t google_ramp_up;
	int32_t google_ramp_down;
	int32_t s3s0_ramp_up;
	int32_t s0_tick_delay[2];		
	int32_t s0a_tick_delay[2];		
	int32_t s0s3_ramp_down;
	int32_t s3_sleep_for;
	int32_t s3_ramp_up;
	int32_t s3_ramp_down;

	
	uint8_t new_s0;
	uint8_t osc_min[2];			
	uint8_t osc_max[2];			
	uint8_t w_ofs[2];			

	
	uint8_t bright_bl_off_fixed[2];		
	uint8_t bright_bl_on_min[2];		
	uint8_t bright_bl_on_max[2];		

	
	uint8_t battery_threshold[LB_BATTERY_LEVELS - 1];

	
	uint8_t s0_idx[2][LB_BATTERY_LEVELS];	
	uint8_t s3_idx[2][LB_BATTERY_LEVELS];	

	
	struct rgb_s color[8];			
} __ec_todo_packed;

struct lightbar_params_v1 {
	
	int32_t google_ramp_up;
	int32_t google_ramp_down;
	int32_t s3s0_ramp_up;
	int32_t s0_tick_delay[2];		
	int32_t s0a_tick_delay[2];		
	int32_t s0s3_ramp_down;
	int32_t s3_sleep_for;
	int32_t s3_ramp_up;
	int32_t s3_ramp_down;
	int32_t s5_ramp_up;
	int32_t s5_ramp_down;
	int32_t tap_tick_delay;
	int32_t tap_gate_delay;
	int32_t tap_display_time;

	
	uint8_t tap_pct_red;
	uint8_t tap_pct_green;
	uint8_t tap_seg_min_on;
	uint8_t tap_seg_max_on;
	uint8_t tap_seg_osc;
	uint8_t tap_idx[3];

	
	uint8_t osc_min[2];			
	uint8_t osc_max[2];			
	uint8_t w_ofs[2];			

	
	uint8_t bright_bl_off_fixed[2];		
	uint8_t bright_bl_on_min[2];		
	uint8_t bright_bl_on_max[2];		

	
	uint8_t battery_threshold[LB_BATTERY_LEVELS - 1];

	
	uint8_t s0_idx[2][LB_BATTERY_LEVELS];	
	uint8_t s3_idx[2][LB_BATTERY_LEVELS];	

	
	uint8_t s5_idx;

	
	struct rgb_s color[8];			
} __ec_todo_packed;



struct lightbar_params_v2_timing {
	
	int32_t google_ramp_up;
	int32_t google_ramp_down;
	int32_t s3s0_ramp_up;
	int32_t s0_tick_delay[2];		
	int32_t s0a_tick_delay[2];		
	int32_t s0s3_ramp_down;
	int32_t s3_sleep_for;
	int32_t s3_ramp_up;
	int32_t s3_ramp_down;
	int32_t s5_ramp_up;
	int32_t s5_ramp_down;
	int32_t tap_tick_delay;
	int32_t tap_gate_delay;
	int32_t tap_display_time;
} __ec_todo_packed;

struct lightbar_params_v2_tap {
	
	uint8_t tap_pct_red;
	uint8_t tap_pct_green;
	uint8_t tap_seg_min_on;
	uint8_t tap_seg_max_on;
	uint8_t tap_seg_osc;
	uint8_t tap_idx[3];
} __ec_todo_packed;

struct lightbar_params_v2_oscillation {
	
	uint8_t osc_min[2];			
	uint8_t osc_max[2];			
	uint8_t w_ofs[2];			
} __ec_todo_packed;

struct lightbar_params_v2_brightness {
	
	uint8_t bright_bl_off_fixed[2];		
	uint8_t bright_bl_on_min[2];		
	uint8_t bright_bl_on_max[2];		
} __ec_todo_packed;

struct lightbar_params_v2_thresholds {
	
	uint8_t battery_threshold[LB_BATTERY_LEVELS - 1];
} __ec_todo_packed;

struct lightbar_params_v2_colors {
	
	uint8_t s0_idx[2][LB_BATTERY_LEVELS];	
	uint8_t s3_idx[2][LB_BATTERY_LEVELS];	

	
	uint8_t s5_idx;

	
	struct rgb_s color[8];			
} __ec_todo_packed;


#define EC_LB_PROG_LEN 192
struct lightbar_program {
	uint8_t size;
	uint8_t data[EC_LB_PROG_LEN];
} __ec_todo_unpacked;

struct ec_params_lightbar {
	uint8_t cmd;		      
	union {
		

		struct __ec_todo_unpacked {
			uint8_t num;
		} set_brightness, seq, demo;

		struct __ec_todo_unpacked {
			uint8_t ctrl, reg, value;
		} reg;

		struct __ec_todo_unpacked {
			uint8_t led, red, green, blue;
		} set_rgb;

		struct __ec_todo_unpacked {
			uint8_t led;
		} get_rgb;

		struct __ec_todo_unpacked {
			uint8_t enable;
		} manual_suspend_ctrl;

		struct lightbar_params_v0 set_params_v0;
		struct lightbar_params_v1 set_params_v1;

		struct lightbar_params_v2_timing set_v2par_timing;
		struct lightbar_params_v2_tap set_v2par_tap;
		struct lightbar_params_v2_oscillation set_v2par_osc;
		struct lightbar_params_v2_brightness set_v2par_bright;
		struct lightbar_params_v2_thresholds set_v2par_thlds;
		struct lightbar_params_v2_colors set_v2par_colors;

		struct lightbar_program set_program;
	};
} __ec_todo_packed;

struct ec_response_lightbar {
	union {
		struct __ec_todo_unpacked {
			struct __ec_todo_unpacked {
				uint8_t reg;
				uint8_t ic0;
				uint8_t ic1;
			} vals[23];
		} dump;

		struct __ec_todo_unpacked {
			uint8_t num;
		} get_seq, get_brightness, get_demo;

		struct lightbar_params_v0 get_params_v0;
		struct lightbar_params_v1 get_params_v1;


		struct lightbar_params_v2_timing get_params_v2_timing;
		struct lightbar_params_v2_tap get_params_v2_tap;
		struct lightbar_params_v2_oscillation get_params_v2_osc;
		struct lightbar_params_v2_brightness get_params_v2_bright;
		struct lightbar_params_v2_thresholds get_params_v2_thlds;
		struct lightbar_params_v2_colors get_params_v2_colors;

		struct __ec_todo_unpacked {
			uint32_t num;
			uint32_t flags;
		} version;

		struct __ec_todo_unpacked {
			uint8_t red, green, blue;
		} get_rgb;

		
	};
} __ec_todo_packed;


enum lightbar_command {
	LIGHTBAR_CMD_DUMP = 0,
	LIGHTBAR_CMD_OFF = 1,
	LIGHTBAR_CMD_ON = 2,
	LIGHTBAR_CMD_INIT = 3,
	LIGHTBAR_CMD_SET_BRIGHTNESS = 4,
	LIGHTBAR_CMD_SEQ = 5,
	LIGHTBAR_CMD_REG = 6,
	LIGHTBAR_CMD_SET_RGB = 7,
	LIGHTBAR_CMD_GET_SEQ = 8,
	LIGHTBAR_CMD_DEMO = 9,
	LIGHTBAR_CMD_GET_PARAMS_V0 = 10,
	LIGHTBAR_CMD_SET_PARAMS_V0 = 11,
	LIGHTBAR_CMD_VERSION = 12,
	LIGHTBAR_CMD_GET_BRIGHTNESS = 13,
	LIGHTBAR_CMD_GET_RGB = 14,
	LIGHTBAR_CMD_GET_DEMO = 15,
	LIGHTBAR_CMD_GET_PARAMS_V1 = 16,
	LIGHTBAR_CMD_SET_PARAMS_V1 = 17,
	LIGHTBAR_CMD_SET_PROGRAM = 18,
	LIGHTBAR_CMD_MANUAL_SUSPEND_CTRL = 19,
	LIGHTBAR_CMD_SUSPEND = 20,
	LIGHTBAR_CMD_RESUME = 21,
	LIGHTBAR_CMD_GET_PARAMS_V2_TIMING = 22,
	LIGHTBAR_CMD_SET_PARAMS_V2_TIMING = 23,
	LIGHTBAR_CMD_GET_PARAMS_V2_TAP = 24,
	LIGHTBAR_CMD_SET_PARAMS_V2_TAP = 25,
	LIGHTBAR_CMD_GET_PARAMS_V2_OSCILLATION = 26,
	LIGHTBAR_CMD_SET_PARAMS_V2_OSCILLATION = 27,
	LIGHTBAR_CMD_GET_PARAMS_V2_BRIGHTNESS = 28,
	LIGHTBAR_CMD_SET_PARAMS_V2_BRIGHTNESS = 29,
	LIGHTBAR_CMD_GET_PARAMS_V2_THRESHOLDS = 30,
	LIGHTBAR_CMD_SET_PARAMS_V2_THRESHOLDS = 31,
	LIGHTBAR_CMD_GET_PARAMS_V2_COLORS = 32,
	LIGHTBAR_CMD_SET_PARAMS_V2_COLORS = 33,
	LIGHTBAR_NUM_CMDS
};




#define EC_CMD_LED_CONTROL 0x0029

enum ec_led_id {
	
	EC_LED_ID_BATTERY_LED = 0,
	
	EC_LED_ID_POWER_LED,
	
	EC_LED_ID_ADAPTER_LED,
	
	EC_LED_ID_LEFT_LED,
	
	EC_LED_ID_RIGHT_LED,
	
	EC_LED_ID_RECOVERY_HW_REINIT_LED,
	
	EC_LED_ID_SYSRQ_DEBUG_LED,

	EC_LED_ID_COUNT
};


#define EC_LED_FLAGS_QUERY BIT(0) 
#define EC_LED_FLAGS_AUTO  BIT(1) 

enum ec_led_colors {
	EC_LED_COLOR_RED = 0,
	EC_LED_COLOR_GREEN,
	EC_LED_COLOR_BLUE,
	EC_LED_COLOR_YELLOW,
	EC_LED_COLOR_WHITE,
	EC_LED_COLOR_AMBER,

	EC_LED_COLOR_COUNT
};

struct ec_params_led_control {
	uint8_t led_id;     
	uint8_t flags;      

	uint8_t brightness[EC_LED_COLOR_COUNT];
} __ec_align1;

struct ec_response_led_control {
	
	uint8_t brightness_range[EC_LED_COLOR_COUNT];
} __ec_align1;







#define EC_CMD_VBOOT_HASH 0x002A

struct ec_params_vboot_hash {
	uint8_t cmd;             
	uint8_t hash_type;       
	uint8_t nonce_size;      
	uint8_t reserved0;       
	uint32_t offset;         
	uint32_t size;           
	uint8_t nonce_data[64];  
} __ec_align4;

struct ec_response_vboot_hash {
	uint8_t status;          
	uint8_t hash_type;       
	uint8_t digest_size;     
	uint8_t reserved0;       
	uint32_t offset;         
	uint32_t size;           
	uint8_t hash_digest[64]; 
} __ec_align4;

enum ec_vboot_hash_cmd {
	EC_VBOOT_HASH_GET = 0,       
	EC_VBOOT_HASH_ABORT = 1,     
	EC_VBOOT_HASH_START = 2,     
	EC_VBOOT_HASH_RECALC = 3,    
};

enum ec_vboot_hash_type {
	EC_VBOOT_HASH_TYPE_SHA256 = 0, 
};

enum ec_vboot_hash_status {
	EC_VBOOT_HASH_STATUS_NONE = 0, 
	EC_VBOOT_HASH_STATUS_DONE = 1, 
	EC_VBOOT_HASH_STATUS_BUSY = 2, 
};


#define EC_VBOOT_HASH_OFFSET_RO		0xfffffffe
#define EC_VBOOT_HASH_OFFSET_ACTIVE	0xfffffffd
#define EC_VBOOT_HASH_OFFSET_UPDATE	0xfffffffc


#define EC_VBOOT_HASH_OFFSET_RW EC_VBOOT_HASH_OFFSET_ACTIVE



#define EC_CMD_MOTION_SENSE_CMD 0x002B


enum motionsense_command {
	
	MOTIONSENSE_CMD_DUMP = 0,

	
	MOTIONSENSE_CMD_INFO = 1,

	
	MOTIONSENSE_CMD_EC_RATE = 2,

	
	MOTIONSENSE_CMD_SENSOR_ODR = 3,

	
	MOTIONSENSE_CMD_SENSOR_RANGE = 4,

	
	MOTIONSENSE_CMD_KB_WAKE_ANGLE = 5,

	
	MOTIONSENSE_CMD_DATA = 6,

	
	MOTIONSENSE_CMD_FIFO_INFO = 7,

	
	MOTIONSENSE_CMD_FIFO_FLUSH = 8,

	
	MOTIONSENSE_CMD_FIFO_READ = 9,

	
	MOTIONSENSE_CMD_PERFORM_CALIB = 10,

	
	MOTIONSENSE_CMD_SENSOR_OFFSET = 11,

	
	MOTIONSENSE_CMD_LIST_ACTIVITIES = 12,

	
	MOTIONSENSE_CMD_SET_ACTIVITY = 13,

	
	MOTIONSENSE_CMD_LID_ANGLE = 14,

	
	MOTIONSENSE_CMD_FIFO_INT_ENABLE = 15,

	
	MOTIONSENSE_CMD_SPOOF = 16,

	
	MOTIONSENSE_CMD_TABLET_MODE_LID_ANGLE = 17,

	
	MOTIONSENSE_CMD_SENSOR_SCALE = 18,

	
	MOTIONSENSE_NUM_CMDS
};


enum motionsensor_type {
	MOTIONSENSE_TYPE_ACCEL = 0,
	MOTIONSENSE_TYPE_GYRO = 1,
	MOTIONSENSE_TYPE_MAG = 2,
	MOTIONSENSE_TYPE_PROX = 3,
	MOTIONSENSE_TYPE_LIGHT = 4,
	MOTIONSENSE_TYPE_ACTIVITY = 5,
	MOTIONSENSE_TYPE_BARO = 6,
	MOTIONSENSE_TYPE_SYNC = 7,
	MOTIONSENSE_TYPE_MAX,
};


enum motionsensor_location {
	MOTIONSENSE_LOC_BASE = 0,
	MOTIONSENSE_LOC_LID = 1,
	MOTIONSENSE_LOC_CAMERA = 2,
	MOTIONSENSE_LOC_MAX,
};


enum motionsensor_chip {
	MOTIONSENSE_CHIP_KXCJ9 = 0,
	MOTIONSENSE_CHIP_LSM6DS0 = 1,
	MOTIONSENSE_CHIP_BMI160 = 2,
	MOTIONSENSE_CHIP_SI1141 = 3,
	MOTIONSENSE_CHIP_SI1142 = 4,
	MOTIONSENSE_CHIP_SI1143 = 5,
	MOTIONSENSE_CHIP_KX022 = 6,
	MOTIONSENSE_CHIP_L3GD20H = 7,
	MOTIONSENSE_CHIP_BMA255 = 8,
	MOTIONSENSE_CHIP_BMP280 = 9,
	MOTIONSENSE_CHIP_OPT3001 = 10,
	MOTIONSENSE_CHIP_BH1730 = 11,
	MOTIONSENSE_CHIP_GPIO = 12,
	MOTIONSENSE_CHIP_LIS2DH = 13,
	MOTIONSENSE_CHIP_LSM6DSM = 14,
	MOTIONSENSE_CHIP_LIS2DE = 15,
	MOTIONSENSE_CHIP_LIS2MDL = 16,
	MOTIONSENSE_CHIP_LSM6DS3 = 17,
	MOTIONSENSE_CHIP_LSM6DSO = 18,
	MOTIONSENSE_CHIP_LNG2DM = 19,
	MOTIONSENSE_CHIP_MAX,
};


enum motionsensor_orientation {
	MOTIONSENSE_ORIENTATION_LANDSCAPE = 0,
	MOTIONSENSE_ORIENTATION_PORTRAIT = 1,
	MOTIONSENSE_ORIENTATION_UPSIDE_DOWN_PORTRAIT = 2,
	MOTIONSENSE_ORIENTATION_UPSIDE_DOWN_LANDSCAPE = 3,
	MOTIONSENSE_ORIENTATION_UNKNOWN = 4,
};

struct ec_response_motion_sensor_data {
	
	uint8_t flags;
	
	uint8_t sensor_num;
	
	union {
		int16_t             data[3];
		struct __ec_todo_packed {
			uint16_t    reserved;
			uint32_t    timestamp;
		};
		struct __ec_todo_unpacked {
			uint8_t     activity; 
			uint8_t     state;
			int16_t     add_info[2];
		};
	};
} __ec_todo_packed;


struct ec_response_motion_sense_fifo_info {
	
	uint16_t size;
	
	uint16_t count;
	
	uint32_t timestamp;
	
	uint16_t total_lost;
	
	uint16_t lost[];
} __ec_todo_packed;

struct ec_response_motion_sense_fifo_data {
	uint32_t number_data;
	struct ec_response_motion_sensor_data data[];
} __ec_todo_packed;


enum motionsensor_activity {
	MOTIONSENSE_ACTIVITY_RESERVED = 0,
	MOTIONSENSE_ACTIVITY_SIG_MOTION = 1,
	MOTIONSENSE_ACTIVITY_DOUBLE_TAP = 2,
	MOTIONSENSE_ACTIVITY_ORIENTATION = 3,
};

struct ec_motion_sense_activity {
	uint8_t sensor_num;
	uint8_t activity; 
	uint8_t enable;   
	uint8_t reserved;
	uint16_t parameters[3]; 
} __ec_todo_unpacked;


#define MOTIONSENSE_MODULE_FLAG_ACTIVE BIT(0)


#define MOTIONSENSE_SENSOR_FLAG_PRESENT BIT(0)


#define MOTIONSENSE_SENSOR_FLAG_FLUSH BIT(0)
#define MOTIONSENSE_SENSOR_FLAG_TIMESTAMP BIT(1)
#define MOTIONSENSE_SENSOR_FLAG_WAKEUP BIT(2)
#define MOTIONSENSE_SENSOR_FLAG_TABLET_MODE BIT(3)
#define MOTIONSENSE_SENSOR_FLAG_ODR BIT(4)


#define EC_MOTION_SENSE_NO_VALUE -1

#define EC_MOTION_SENSE_INVALID_CALIB_TEMP 0x8000



#define MOTION_SENSE_SET_OFFSET BIT(0)


#define MOTION_SENSE_DEFAULT_SCALE BIT(15)

#define LID_ANGLE_UNRELIABLE 500

enum motionsense_spoof_mode {
	
	MOTIONSENSE_SPOOF_MODE_DISABLE = 0,

	
	MOTIONSENSE_SPOOF_MODE_CUSTOM,

	
	MOTIONSENSE_SPOOF_MODE_LOCK_CURRENT,

	
	MOTIONSENSE_SPOOF_MODE_QUERY,
};

struct ec_params_motion_sense {
	uint8_t cmd;
	union {
		
		struct __ec_todo_unpacked {
			
			uint8_t max_sensor_count;
		} dump;

		
		struct __ec_todo_unpacked {
			
			int16_t data;
		} kb_wake_angle;

		
		struct __ec_todo_unpacked {
			uint8_t sensor_num;
		} info, info_3, data, fifo_flush, perform_calib,
				list_activities;

		
		struct __ec_todo_unpacked {
			uint8_t sensor_num;

			
			uint8_t roundup;

			uint16_t reserved;

			
			int32_t data;
		} ec_rate, sensor_odr, sensor_range;

		
		struct __ec_todo_packed {
			uint8_t sensor_num;

			
			uint16_t flags;

			
			int16_t temp;

			
			int16_t offset[3];
		} sensor_offset;

		
		struct __ec_todo_packed {
			uint8_t sensor_num;

			
			uint16_t flags;

			
			int16_t temp;

			
			uint16_t scale[3];
		} sensor_scale;


		
		

		
		struct __ec_todo_unpacked {
			
			uint32_t max_data_vector;
		} fifo_read;

		struct ec_motion_sense_activity set_activity;

		
		

		
		struct __ec_todo_unpacked {
			
			int8_t enable;
		} fifo_int_enable;

		
		struct __ec_todo_packed {
			uint8_t sensor_id;

			
			uint8_t spoof_enable;

			
			uint8_t reserved;

			
			int16_t components[3];
		} spoof;

		
		struct __ec_todo_unpacked {
			
			int16_t lid_angle;

			
			int16_t hys_degree;
		} tablet_mode_threshold;
	};
} __ec_todo_packed;

struct ec_response_motion_sense {
	union {
		
		struct __ec_todo_unpacked {
			
			uint8_t module_flags;

			
			uint8_t sensor_count;

			
			DECLARE_FLEX_ARRAY(struct ec_response_motion_sensor_data, sensor);
		} dump;

		
		struct __ec_todo_unpacked {
			
			uint8_t type;

			
			uint8_t location;

			
			uint8_t chip;
		} info;

		
		struct __ec_todo_unpacked {
			
			uint8_t type;

			
			uint8_t location;

			
			uint8_t chip;

			
			uint32_t min_frequency;

			
			uint32_t max_frequency;

			
			uint32_t fifo_max_event_count;
		} info_3;

		
		struct ec_response_motion_sensor_data data;

		
		struct __ec_todo_unpacked {
			
			int32_t ret;
		} ec_rate, sensor_odr, sensor_range, kb_wake_angle,
		  fifo_int_enable, spoof;

		
		struct __ec_todo_unpacked  {
			int16_t temp;
			int16_t offset[3];
		} sensor_offset, perform_calib;

		
		struct __ec_todo_unpacked  {
			int16_t temp;
			uint16_t scale[3];
		} sensor_scale;

		struct ec_response_motion_sense_fifo_info fifo_info, fifo_flush;

		struct ec_response_motion_sense_fifo_data fifo_read;

		struct __ec_todo_packed {
			uint16_t reserved;
			uint32_t enabled;
			uint32_t disabled;
		} list_activities;

		

		
		struct __ec_todo_unpacked {
			
			uint16_t value;
		} lid_angle;

		
		struct __ec_todo_unpacked {
			
			uint16_t lid_angle;

			
			uint16_t hys_degree;
		} tablet_mode_threshold;

	};
} __ec_todo_packed;





#define EC_CMD_FORCE_LID_OPEN 0x002C

struct ec_params_force_lid_open {
	uint8_t enabled;
} __ec_align1;



#define EC_CMD_CONFIG_POWER_BUTTON 0x002D

enum ec_config_power_button_flags {
	
	EC_POWER_BUTTON_ENABLE_PULSE = BIT(0),
};

struct ec_params_config_power_button {
	
	uint8_t flags;
} __ec_align1;





#define EC_CMD_USB_CHARGE_SET_MODE 0x0030

struct ec_params_usb_charge_set_mode {
	uint8_t usb_port_id;
	uint8_t mode:7;
	uint8_t inhibit_charge:1;
} __ec_align1;





#define EC_PSTORE_SIZE_MAX 64


#define EC_CMD_PSTORE_INFO 0x0040

struct ec_response_pstore_info {
	
	uint32_t pstore_size;
	
	uint32_t access_size;
} __ec_align4;


#define EC_CMD_PSTORE_READ 0x0041

struct ec_params_pstore_read {
	uint32_t offset;   
	uint32_t size;     
} __ec_align4;


#define EC_CMD_PSTORE_WRITE 0x0042

struct ec_params_pstore_write {
	uint32_t offset;   
	uint32_t size;     
	uint8_t data[EC_PSTORE_SIZE_MAX];
} __ec_align4;





struct ec_params_rtc {
	uint32_t time;
} __ec_align4;

struct ec_response_rtc {
	uint32_t time;
} __ec_align4;


#define EC_CMD_RTC_GET_VALUE 0x0044
#define EC_CMD_RTC_GET_ALARM 0x0045


#define EC_CMD_RTC_SET_VALUE 0x0046
#define EC_CMD_RTC_SET_ALARM 0x0047


#define EC_RTC_ALARM_CLEAR 0





#define EC_PORT80_SIZE_MAX 32


#define EC_CMD_PORT80_LAST_BOOT 0x0048
#define EC_CMD_PORT80_READ 0x0048

enum ec_port80_subcmd {
	EC_PORT80_GET_INFO = 0,
	EC_PORT80_READ_BUFFER,
};

struct ec_params_port80_read {
	uint16_t subcmd;
	union {
		struct __ec_todo_unpacked {
			uint32_t offset;
			uint32_t num_entries;
		} read_buffer;
	};
} __ec_todo_packed;

struct ec_response_port80_read {
	union {
		struct __ec_todo_unpacked {
			uint32_t writes;
			uint32_t history_size;
			uint32_t last_boot;
		} get_info;
		struct __ec_todo_unpacked {
			uint16_t codes[EC_PORT80_SIZE_MAX];
		} data;
	};
} __ec_todo_packed;

struct ec_response_port80_last_boot {
	uint16_t code;
} __ec_align2;





#define EC_VSTORE_SLOT_SIZE 64


#define EC_VSTORE_SLOT_MAX 32


#define EC_CMD_VSTORE_INFO 0x0049
struct ec_response_vstore_info {
	
	uint32_t slot_locked;
	
	uint8_t slot_count;
} __ec_align_size1;


#define EC_CMD_VSTORE_READ 0x004A

struct ec_params_vstore_read {
	uint8_t slot; 
} __ec_align1;

struct ec_response_vstore_read {
	uint8_t data[EC_VSTORE_SLOT_SIZE];
} __ec_align1;


#define EC_CMD_VSTORE_WRITE 0x004B

struct ec_params_vstore_write {
	uint8_t slot; 
	uint8_t data[EC_VSTORE_SLOT_SIZE];
} __ec_align1;




#define EC_CMD_THERMAL_SET_THRESHOLD 0x0050
#define EC_CMD_THERMAL_GET_THRESHOLD 0x0051




struct ec_params_thermal_set_threshold {
	uint8_t sensor_type;
	uint8_t threshold_id;
	uint16_t value;
} __ec_align2;


struct ec_params_thermal_get_threshold {
	uint8_t sensor_type;
	uint8_t threshold_id;
} __ec_align1;

struct ec_response_thermal_get_threshold {
	uint16_t value;
} __ec_align2;



enum ec_temp_thresholds {
	EC_TEMP_THRESH_WARN = 0,
	EC_TEMP_THRESH_HIGH,
	EC_TEMP_THRESH_HALT,

	EC_TEMP_THRESH_COUNT
};


struct ec_thermal_config {
	uint32_t temp_host[EC_TEMP_THRESH_COUNT]; 
	uint32_t temp_host_release[EC_TEMP_THRESH_COUNT]; 
	uint32_t temp_fan_off;		
	uint32_t temp_fan_max;		
} __ec_align4;


struct ec_params_thermal_get_threshold_v1 {
	uint32_t sensor_num;
} __ec_align4;



struct ec_params_thermal_set_threshold_v1 {
	uint32_t sensor_num;
	struct ec_thermal_config cfg;
} __ec_align4;





#define EC_CMD_THERMAL_AUTO_FAN_CTRL 0x0052


struct ec_params_auto_fan_ctrl_v1 {
	uint8_t fan_idx;
} __ec_align1;


#define EC_CMD_TMP006_GET_CALIBRATION 0x0053
#define EC_CMD_TMP006_SET_CALIBRATION 0x0054




struct ec_params_tmp006_get_calibration {
	uint8_t index;
} __ec_align1;


struct ec_response_tmp006_get_calibration_v0 {
	float s0;
	float b0;
	float b1;
	float b2;
} __ec_align4;

struct ec_params_tmp006_set_calibration_v0 {
	uint8_t index;
	uint8_t reserved[3];
	float s0;
	float b0;
	float b1;
	float b2;
} __ec_align4;


struct ec_response_tmp006_get_calibration_v1 {
	uint8_t algorithm;
	uint8_t num_params;
	uint8_t reserved[2];
	float val[];
} __ec_align4;

struct ec_params_tmp006_set_calibration_v1 {
	uint8_t index;
	uint8_t algorithm;
	uint8_t num_params;
	uint8_t reserved;
	float val[];
} __ec_align4;



#define EC_CMD_TMP006_GET_RAW 0x0055

struct ec_params_tmp006_get_raw {
	uint8_t index;
} __ec_align1;

struct ec_response_tmp006_get_raw {
	int32_t t;  
	int32_t v;  
} __ec_align4;





#define EC_CMD_MKBP_STATE 0x0060


#define EC_CMD_MKBP_INFO 0x0061

struct ec_response_mkbp_info {
	uint32_t rows;
	uint32_t cols;
	
	uint8_t reserved;
} __ec_align_size1;

struct ec_params_mkbp_info {
	uint8_t info_type;
	uint8_t event_type;
} __ec_align1;

enum ec_mkbp_info_type {
	
	EC_MKBP_INFO_KBD = 0,

	
	EC_MKBP_INFO_SUPPORTED = 1,

	
	EC_MKBP_INFO_CURRENT = 2,
};


#define EC_CMD_MKBP_SIMULATE_KEY 0x0062

struct ec_params_mkbp_simulate_key {
	uint8_t col;
	uint8_t row;
	uint8_t pressed;
} __ec_align1;

#define EC_CMD_GET_KEYBOARD_ID 0x0063

struct ec_response_keyboard_id {
	uint32_t keyboard_id;
} __ec_align4;

enum keyboard_id {
	KEYBOARD_ID_UNSUPPORTED = 0,
	KEYBOARD_ID_UNREADABLE = 0xffffffff,
};


#define EC_CMD_MKBP_SET_CONFIG 0x0064
#define EC_CMD_MKBP_GET_CONFIG 0x0065


enum mkbp_config_flags {
	EC_MKBP_FLAGS_ENABLE = 1,	
};

enum mkbp_config_valid {
	EC_MKBP_VALID_SCAN_PERIOD		= BIT(0),
	EC_MKBP_VALID_POLL_TIMEOUT		= BIT(1),
	EC_MKBP_VALID_MIN_POST_SCAN_DELAY	= BIT(3),
	EC_MKBP_VALID_OUTPUT_SETTLE		= BIT(4),
	EC_MKBP_VALID_DEBOUNCE_DOWN		= BIT(5),
	EC_MKBP_VALID_DEBOUNCE_UP		= BIT(6),
	EC_MKBP_VALID_FIFO_MAX_DEPTH		= BIT(7),
};


struct ec_mkbp_config {
	uint32_t valid_mask;		
	uint8_t flags;		
	uint8_t valid_flags;		
	uint16_t scan_period_us;	
	
	uint32_t poll_timeout_us;
	
	uint16_t min_post_scan_delay_us;
	
	uint16_t output_settle_us;
	uint16_t debounce_down_us;	
	uint16_t debounce_up_us;	
	
	uint8_t fifo_max_depth;
} __ec_align_size1;

struct ec_params_mkbp_set_config {
	struct ec_mkbp_config config;
} __ec_align_size1;

struct ec_response_mkbp_get_config {
	struct ec_mkbp_config config;
} __ec_align_size1;


#define EC_CMD_KEYSCAN_SEQ_CTRL 0x0066

enum ec_keyscan_seq_cmd {
	EC_KEYSCAN_SEQ_STATUS = 0,	
	EC_KEYSCAN_SEQ_CLEAR = 1,	
	EC_KEYSCAN_SEQ_ADD = 2,		
	EC_KEYSCAN_SEQ_START = 3,	
	EC_KEYSCAN_SEQ_COLLECT = 4,	
};

enum ec_collect_flags {
	
	EC_KEYSCAN_SEQ_FLAG_DONE	= BIT(0),
};

struct ec_collect_item {
	uint8_t flags;		
} __ec_align1;

struct ec_params_keyscan_seq_ctrl {
	uint8_t cmd;	
	union {
		struct __ec_align1 {
			uint8_t active;		
			uint8_t num_items;	
			
			uint8_t cur_item;
		} status;
		struct __ec_todo_unpacked {
			
			uint32_t time_us;
			uint8_t scan[0];	
		} add;
		struct __ec_align1 {
			uint8_t start_item;	
			uint8_t num_items;	
		} collect;
	};
} __ec_todo_packed;

struct ec_result_keyscan_seq_ctrl {
	union {
		struct __ec_todo_unpacked {
			uint8_t num_items;	
			
			struct ec_collect_item item[0];
		} collect;
	};
} __ec_todo_packed;


#define EC_CMD_GET_NEXT_EVENT 0x0067

#define EC_MKBP_HAS_MORE_EVENTS_SHIFT 7


#define EC_MKBP_HAS_MORE_EVENTS BIT(EC_MKBP_HAS_MORE_EVENTS_SHIFT)


#define EC_MKBP_EVENT_TYPE_MASK (BIT(EC_MKBP_HAS_MORE_EVENTS_SHIFT) - 1)

enum ec_mkbp_event {
	
	EC_MKBP_EVENT_KEY_MATRIX = 0,

	
	EC_MKBP_EVENT_HOST_EVENT = 1,

	
	EC_MKBP_EVENT_SENSOR_FIFO = 2,

	
	EC_MKBP_EVENT_BUTTON = 3,

	
	EC_MKBP_EVENT_SWITCH = 4,

	
	EC_MKBP_EVENT_FINGERPRINT = 5,

	
	EC_MKBP_EVENT_SYSRQ = 6,

	
	EC_MKBP_EVENT_HOST_EVENT64 = 7,

	
	EC_MKBP_EVENT_CEC_EVENT = 8,

	
	EC_MKBP_EVENT_CEC_MESSAGE = 9,

	
	EC_MKBP_EVENT_PCHG = 12,

	
	EC_MKBP_EVENT_COUNT,
};
BUILD_ASSERT(EC_MKBP_EVENT_COUNT <= EC_MKBP_EVENT_TYPE_MASK);

union __ec_align_offset1 ec_response_get_next_data {
	uint8_t key_matrix[13];

	
	uint32_t host_event;
	uint64_t host_event64;

	struct __ec_todo_unpacked {
		
		uint8_t reserved[3];
		struct ec_response_motion_sense_fifo_info info;
	} sensor_fifo;

	uint32_t buttons;

	uint32_t switches;

	uint32_t fp_events;

	uint32_t sysrq;

	
	uint32_t cec_events;
};

union __ec_align_offset1 ec_response_get_next_data_v1 {
	uint8_t key_matrix[16];

	
	uint32_t host_event;
	uint64_t host_event64;

	struct __ec_todo_unpacked {
		
		uint8_t reserved[3];
		struct ec_response_motion_sense_fifo_info info;
	} sensor_fifo;

	uint32_t buttons;

	uint32_t switches;

	uint32_t fp_events;

	uint32_t sysrq;

	
	uint32_t cec_events;

	uint8_t cec_message[16];
};
BUILD_ASSERT(sizeof(union ec_response_get_next_data_v1) == 16);

struct ec_response_get_next_event {
	uint8_t event_type;
	
	union ec_response_get_next_data data;
} __ec_align1;

struct ec_response_get_next_event_v1 {
	uint8_t event_type;
	
	union ec_response_get_next_data_v1 data;
} __ec_align1;



#define EC_MKBP_POWER_BUTTON	0
#define EC_MKBP_VOL_UP		1
#define EC_MKBP_VOL_DOWN	2
#define EC_MKBP_RECOVERY	3
#define EC_MKBP_BRI_UP		4
#define EC_MKBP_BRI_DOWN	5
#define EC_MKBP_SCREEN_LOCK	6


#define EC_MKBP_LID_OPEN	0
#define EC_MKBP_TABLET_MODE	1
#define EC_MKBP_BASE_ATTACHED	2
#define EC_MKBP_FRONT_PROXIMITY	3


#define EC_CMD_KEYBOARD_FACTORY_TEST 0x0068

struct ec_response_keyboard_factory_test {
	uint16_t shorted;	
} __ec_align2;


#define EC_MKBP_FP_RAW_EVENT(fp_events) ((fp_events) & 0x00FFFFFF)
#define EC_MKBP_FP_ERRCODE(fp_events)   ((fp_events) & 0x0000000F)
#define EC_MKBP_FP_ENROLL_PROGRESS_OFFSET 4
#define EC_MKBP_FP_ENROLL_PROGRESS(fpe) (((fpe) & 0x00000FF0) \
					 >> EC_MKBP_FP_ENROLL_PROGRESS_OFFSET)
#define EC_MKBP_FP_MATCH_IDX_OFFSET 12
#define EC_MKBP_FP_MATCH_IDX_MASK 0x0000F000
#define EC_MKBP_FP_MATCH_IDX(fpe) (((fpe) & EC_MKBP_FP_MATCH_IDX_MASK) \
					 >> EC_MKBP_FP_MATCH_IDX_OFFSET)
#define EC_MKBP_FP_ENROLL               BIT(27)
#define EC_MKBP_FP_MATCH                BIT(28)
#define EC_MKBP_FP_FINGER_DOWN          BIT(29)
#define EC_MKBP_FP_FINGER_UP            BIT(30)
#define EC_MKBP_FP_IMAGE_READY          BIT(31)

#define EC_MKBP_FP_ERR_ENROLL_OK               0
#define EC_MKBP_FP_ERR_ENROLL_LOW_QUALITY      1
#define EC_MKBP_FP_ERR_ENROLL_IMMOBILE         2
#define EC_MKBP_FP_ERR_ENROLL_LOW_COVERAGE     3
#define EC_MKBP_FP_ERR_ENROLL_INTERNAL         5

#define EC_MKBP_FP_ERR_ENROLL_PROBLEM_MASK     1

#define EC_MKBP_FP_ERR_MATCH_NO                0
#define EC_MKBP_FP_ERR_MATCH_NO_INTERNAL       6
#define EC_MKBP_FP_ERR_MATCH_NO_TEMPLATES      7
#define EC_MKBP_FP_ERR_MATCH_NO_LOW_QUALITY    2
#define EC_MKBP_FP_ERR_MATCH_NO_LOW_COVERAGE   4
#define EC_MKBP_FP_ERR_MATCH_YES               1
#define EC_MKBP_FP_ERR_MATCH_YES_UPDATED       3
#define EC_MKBP_FP_ERR_MATCH_YES_UPDATE_FAILED 5






#define EC_CMD_TEMP_SENSOR_GET_INFO 0x0070

struct ec_params_temp_sensor_get_info {
	uint8_t id;
} __ec_align1;

struct ec_response_temp_sensor_get_info {
	char sensor_name[32];
	uint8_t sensor_type;
} __ec_align1;











struct ec_params_host_event_mask {
	uint32_t mask;
} __ec_align4;

struct ec_response_host_event_mask {
	uint32_t mask;
} __ec_align4;


#define EC_CMD_HOST_EVENT_GET_B         0x0087
#define EC_CMD_HOST_EVENT_GET_SMI_MASK  0x0088
#define EC_CMD_HOST_EVENT_GET_SCI_MASK  0x0089
#define EC_CMD_HOST_EVENT_GET_WAKE_MASK 0x008D


#define EC_CMD_HOST_EVENT_SET_SMI_MASK  0x008A
#define EC_CMD_HOST_EVENT_SET_SCI_MASK  0x008B
#define EC_CMD_HOST_EVENT_CLEAR         0x008C
#define EC_CMD_HOST_EVENT_SET_WAKE_MASK 0x008E
#define EC_CMD_HOST_EVENT_CLEAR_B       0x008F



struct ec_params_host_event {

	
	uint8_t action;

	
	uint8_t mask_type;

	
	uint16_t reserved;

	
	uint64_t value;
} __ec_align4;



struct ec_response_host_event {

	
	uint64_t value;
} __ec_align4;

enum ec_host_event_action {
	
	EC_HOST_EVENT_GET,

	
	EC_HOST_EVENT_SET,

	
	EC_HOST_EVENT_CLEAR,
};

enum ec_host_event_mask_type {

	
	EC_HOST_EVENT_MAIN,

	
	EC_HOST_EVENT_B,

	
	EC_HOST_EVENT_SCI_MASK,

	
	EC_HOST_EVENT_SMI_MASK,

	
	EC_HOST_EVENT_ALWAYS_REPORT_MASK,

	
	EC_HOST_EVENT_ACTIVE_WAKE_MASK,

	
	EC_HOST_EVENT_LAZY_WAKE_MASK_S0IX,

	
	EC_HOST_EVENT_LAZY_WAKE_MASK_S3,

	
	EC_HOST_EVENT_LAZY_WAKE_MASK_S5,
};

#define EC_CMD_HOST_EVENT       0x00A4





#define EC_CMD_SWITCH_ENABLE_BKLIGHT 0x0090

struct ec_params_switch_enable_backlight {
	uint8_t enabled;
} __ec_align1;


#define EC_CMD_SWITCH_ENABLE_WIRELESS 0x0091
#define EC_VER_SWITCH_ENABLE_WIRELESS 1


struct ec_params_switch_enable_wireless_v0 {
	uint8_t enabled;
} __ec_align1;


struct ec_params_switch_enable_wireless_v1 {
	
	uint8_t now_flags;

	
	uint8_t now_mask;

	
	uint8_t suspend_flags;

	
	uint8_t suspend_mask;
} __ec_align1;


struct ec_response_switch_enable_wireless_v1 {
	
	uint8_t now_flags;

	
	uint8_t suspend_flags;
} __ec_align1;





#define EC_CMD_GPIO_SET 0x0092

struct ec_params_gpio_set {
	char name[32];
	uint8_t val;
} __ec_align1;


#define EC_CMD_GPIO_GET 0x0093


struct ec_params_gpio_get {
	char name[32];
} __ec_align1;

struct ec_response_gpio_get {
	uint8_t val;
} __ec_align1;


struct ec_params_gpio_get_v1 {
	uint8_t subcmd;
	union {
		struct __ec_align1 {
			char name[32];
		} get_value_by_name;
		struct __ec_align1 {
			uint8_t index;
		} get_info;
	};
} __ec_align1;

struct ec_response_gpio_get_v1 {
	union {
		struct __ec_align1 {
			uint8_t val;
		} get_value_by_name, get_count;
		struct __ec_todo_unpacked {
			uint8_t val;
			char name[32];
			uint32_t flags;
		} get_info;
	};
} __ec_todo_packed;

enum gpio_get_subcmd {
	EC_GPIO_GET_BY_NAME = 0,
	EC_GPIO_GET_COUNT = 1,
	EC_GPIO_GET_INFO = 2,
};







#define EC_CMD_I2C_READ 0x0094

struct ec_params_i2c_read {
	uint16_t addr; 
	uint8_t read_size; 
	uint8_t port;
	uint8_t offset;
} __ec_align_size1;

struct ec_response_i2c_read {
	uint16_t data;
} __ec_align2;


#define EC_CMD_I2C_WRITE 0x0095

struct ec_params_i2c_write {
	uint16_t data;
	uint16_t addr; 
	uint8_t write_size; 
	uint8_t port;
	uint8_t offset;
} __ec_align_size1;





#define EC_CMD_CHARGE_CONTROL 0x0096
#define EC_VER_CHARGE_CONTROL 1

enum ec_charge_control_mode {
	CHARGE_CONTROL_NORMAL = 0,
	CHARGE_CONTROL_IDLE,
	CHARGE_CONTROL_DISCHARGE,
};

struct ec_params_charge_control {
	uint32_t mode;  
} __ec_align4;




#define EC_CMD_CONSOLE_SNAPSHOT 0x0097


#define EC_CMD_CONSOLE_READ 0x0098

enum ec_console_read_subcmd {
	CONSOLE_READ_NEXT = 0,
	CONSOLE_READ_RECENT
};

struct ec_params_console_read_v1 {
	uint8_t subcmd; 
} __ec_align1;




#define EC_CMD_BATTERY_CUT_OFF 0x0099

#define EC_BATTERY_CUTOFF_FLAG_AT_SHUTDOWN	BIT(0)

struct ec_params_battery_cutoff {
	uint8_t flags;
} __ec_align1;





#define EC_CMD_USB_MUX 0x009A

struct ec_params_usb_mux {
	uint8_t mux;
} __ec_align1;




enum ec_ldo_state {
	EC_LDO_STATE_OFF = 0,	
	EC_LDO_STATE_ON = 1,	
};


#define EC_CMD_LDO_SET 0x009B

struct ec_params_ldo_set {
	uint8_t index;
	uint8_t state;
} __ec_align1;


#define EC_CMD_LDO_GET 0x009C

struct ec_params_ldo_get {
	uint8_t index;
} __ec_align1;

struct ec_response_ldo_get {
	uint8_t state;
} __ec_align1;





#define EC_CMD_POWER_INFO 0x009D

struct ec_response_power_info {
	uint32_t usb_dev_type;
	uint16_t voltage_ac;
	uint16_t voltage_system;
	uint16_t current_system;
	uint16_t usb_current_limit;
} __ec_align4;




#define EC_CMD_I2C_PASSTHRU 0x009E


#define EC_I2C_FLAG_READ	BIT(15)


#define EC_I2C_ADDR_MASK	0x3ff

#define EC_I2C_STATUS_NAK	BIT(0) 
#define EC_I2C_STATUS_TIMEOUT	BIT(1) 


#define EC_I2C_STATUS_ERROR	(EC_I2C_STATUS_NAK | EC_I2C_STATUS_TIMEOUT)

struct ec_params_i2c_passthru_msg {
	uint16_t addr_flags;	
	uint16_t len;		
} __ec_align2;

struct ec_params_i2c_passthru {
	uint8_t port;		
	uint8_t num_msgs;	
	struct ec_params_i2c_passthru_msg msg[];
	
} __ec_align2;

struct ec_response_i2c_passthru {
	uint8_t i2c_status;	
	uint8_t num_msgs;	
	uint8_t data[];		
} __ec_align1;




#define EC_CMD_HANG_DETECT 0x009F



#define EC_HANG_START_ON_POWER_PRESS  BIT(0)


#define EC_HANG_START_ON_LID_CLOSE    BIT(1)

 
#define EC_HANG_START_ON_LID_OPEN     BIT(2)


#define EC_HANG_START_ON_RESUME       BIT(3)




#define EC_HANG_STOP_ON_POWER_RELEASE BIT(8)


#define EC_HANG_STOP_ON_HOST_COMMAND  BIT(9)


#define EC_HANG_STOP_ON_SUSPEND       BIT(10)


#define EC_HANG_START_NOW             BIT(30)


#define EC_HANG_STOP_NOW              BIT(31)

struct ec_params_hang_detect {
	
	uint32_t flags;

	
	uint16_t host_event_timeout_msec;

	
	uint16_t warm_reboot_timeout_msec;
} __ec_align4;





#define EC_CMD_CHARGE_STATE 0x00A0


enum charge_state_command {
	CHARGE_STATE_CMD_GET_STATE,
	CHARGE_STATE_CMD_GET_PARAM,
	CHARGE_STATE_CMD_SET_PARAM,
	CHARGE_STATE_NUM_CMDS
};


enum charge_state_params {
	CS_PARAM_CHG_VOLTAGE,	      
	CS_PARAM_CHG_CURRENT,	      
	CS_PARAM_CHG_INPUT_CURRENT,   
	CS_PARAM_CHG_STATUS,	      
	CS_PARAM_CHG_OPTION,	      
	CS_PARAM_LIMIT_POWER,	      
	
	CS_NUM_BASE_PARAMS,

	
	CS_PARAM_CUSTOM_PROFILE_MIN = 0x10000,
	CS_PARAM_CUSTOM_PROFILE_MAX = 0x1ffff,

	
	CS_PARAM_DEBUG_MIN = 0x20000,
	CS_PARAM_DEBUG_CTL_MODE = 0x20000,
	CS_PARAM_DEBUG_MANUAL_MODE,
	CS_PARAM_DEBUG_SEEMS_DEAD,
	CS_PARAM_DEBUG_SEEMS_DISCONNECTED,
	CS_PARAM_DEBUG_BATT_REMOVED,
	CS_PARAM_DEBUG_MANUAL_CURRENT,
	CS_PARAM_DEBUG_MANUAL_VOLTAGE,
	CS_PARAM_DEBUG_MAX = 0x2ffff,

	
};

struct ec_params_charge_state {
	uint8_t cmd;				
	union {
		

		struct __ec_todo_unpacked {
			uint32_t param;		
		} get_param;

		struct __ec_todo_unpacked {
			uint32_t param;		
			uint32_t value;		
		} set_param;
	};
} __ec_todo_packed;

struct ec_response_charge_state {
	union {
		struct __ec_align4 {
			int ac;
			int chg_voltage;
			int chg_current;
			int chg_input_current;
			int batt_state_of_charge;
		} get_state;

		struct __ec_align4 {
			uint32_t value;
		} get_param;

		
	};
} __ec_align4;



#define EC_CMD_CHARGE_CURRENT_LIMIT 0x00A1

struct ec_params_current_limit {
	uint32_t limit; 
} __ec_align4;


#define EC_CMD_EXTERNAL_POWER_LIMIT 0x00A2


struct ec_params_external_power_limit_v1 {
	uint16_t current_lim; 
	uint16_t voltage_lim; 
} __ec_align2;

#define EC_POWER_LIMIT_NONE 0xffff


#define EC_CMD_OVERRIDE_DEDICATED_CHARGER_LIMIT 0x00A3

struct ec_params_dedicated_charger_limit {
	uint16_t current_lim; 
	uint16_t voltage_lim; 
} __ec_align2;





#define EC_CMD_HIBERNATION_DELAY 0x00A8

struct ec_params_hibernation_delay {
	
	uint32_t seconds;
} __ec_align4;

struct ec_response_hibernation_delay {
	
	uint32_t time_g3;

	
	uint32_t time_remaining;

	
	uint32_t hibernate_delay;
} __ec_align4;


#define EC_CMD_HOST_SLEEP_EVENT 0x00A9

enum host_sleep_event {
	HOST_SLEEP_EVENT_S3_SUSPEND   = 1,
	HOST_SLEEP_EVENT_S3_RESUME    = 2,
	HOST_SLEEP_EVENT_S0IX_SUSPEND = 3,
	HOST_SLEEP_EVENT_S0IX_RESUME  = 4,
	
	HOST_SLEEP_EVENT_S3_WAKEABLE_SUSPEND = 5,
};

struct ec_params_host_sleep_event {
	uint8_t sleep_event;
} __ec_align1;


#define EC_HOST_SLEEP_TIMEOUT_DEFAULT 0


#define EC_HOST_SLEEP_TIMEOUT_INFINITE 0xFFFF

struct ec_params_host_sleep_event_v1 {
	
	uint8_t sleep_event;

	
	uint8_t reserved;
	union {
		
		struct {
			
			uint16_t sleep_timeout_ms;
		} suspend_params;

		
	};
} __ec_align2;


#define EC_HOST_RESUME_SLEEP_TIMEOUT 0x80000000


#define EC_HOST_RESUME_SLEEP_TRANSITIONS_MASK 0x7FFFFFFF

struct ec_response_host_sleep_event_v1 {
	union {
		
		struct {
			
			uint32_t sleep_transitions;
		} resume_response;

		
	};
} __ec_align4;



#define EC_CMD_DEVICE_EVENT 0x00AA

enum ec_device_event {
	EC_DEVICE_EVENT_TRACKPAD,
	EC_DEVICE_EVENT_DSP,
	EC_DEVICE_EVENT_WIFI,
	EC_DEVICE_EVENT_WLC,
};

enum ec_device_event_param {
	
	EC_DEVICE_EVENT_PARAM_GET_CURRENT_EVENTS,
	
	EC_DEVICE_EVENT_PARAM_GET_ENABLED_EVENTS,
	
	EC_DEVICE_EVENT_PARAM_SET_ENABLED_EVENTS,
};

#define EC_DEVICE_EVENT_MASK(event_code) BIT(event_code % 32)

struct ec_params_device_event {
	uint32_t event_mask;
	uint8_t param;
} __ec_align_size1;

struct ec_response_device_event {
	uint32_t event_mask;
} __ec_align4;





#define EC_CMD_SB_READ_WORD   0x00B0
#define EC_CMD_SB_WRITE_WORD  0x00B1


#define EC_CMD_SB_READ_BLOCK  0x00B2
#define EC_CMD_SB_WRITE_BLOCK 0x00B3

struct ec_params_sb_rd {
	uint8_t reg;
} __ec_align1;

struct ec_response_sb_rd_word {
	uint16_t value;
} __ec_align2;

struct ec_params_sb_wr_word {
	uint8_t reg;
	uint16_t value;
} __ec_align1;

struct ec_response_sb_rd_block {
	uint8_t data[32];
} __ec_align1;

struct ec_params_sb_wr_block {
	uint8_t reg;
	uint16_t data[32];
} __ec_align1;




#define EC_CMD_BATTERY_VENDOR_PARAM 0x00B4

enum ec_battery_vendor_param_mode {
	BATTERY_VENDOR_PARAM_MODE_GET = 0,
	BATTERY_VENDOR_PARAM_MODE_SET,
};

struct ec_params_battery_vendor_param {
	uint32_t param;
	uint32_t value;
	uint8_t mode;
} __ec_align_size1;

struct ec_response_battery_vendor_param {
	uint32_t value;
} __ec_align4;



#define EC_CMD_SB_FW_UPDATE 0x00B5

enum ec_sb_fw_update_subcmd {
	EC_SB_FW_UPDATE_PREPARE  = 0x0,
	EC_SB_FW_UPDATE_INFO     = 0x1, 
	EC_SB_FW_UPDATE_BEGIN    = 0x2, 
	EC_SB_FW_UPDATE_WRITE    = 0x3, 
	EC_SB_FW_UPDATE_END      = 0x4,
	EC_SB_FW_UPDATE_STATUS   = 0x5,
	EC_SB_FW_UPDATE_PROTECT  = 0x6,
	EC_SB_FW_UPDATE_MAX      = 0x7,
};

#define SB_FW_UPDATE_CMD_WRITE_BLOCK_SIZE 32
#define SB_FW_UPDATE_CMD_STATUS_SIZE 2
#define SB_FW_UPDATE_CMD_INFO_SIZE 8

struct ec_sb_fw_update_header {
	uint16_t subcmd;  
	uint16_t fw_id;   
} __ec_align4;

struct ec_params_sb_fw_update {
	struct ec_sb_fw_update_header hdr;
	union {
		
		
		
		
		
		
		

		
		struct __ec_align4 {
			uint8_t  data[SB_FW_UPDATE_CMD_WRITE_BLOCK_SIZE];
		} write;
	};
} __ec_align4;

struct ec_response_sb_fw_update {
	union {
		
		struct __ec_align1 {
			uint8_t data[SB_FW_UPDATE_CMD_INFO_SIZE];
		} info;

		
		struct __ec_align1 {
			uint8_t data[SB_FW_UPDATE_CMD_STATUS_SIZE];
		} status;
	};
} __ec_align1;


#define EC_CMD_ENTERING_MODE 0x00B6

struct ec_params_entering_mode {
	int vboot_mode;
} __ec_align4;

#define VBOOT_MODE_NORMAL    0
#define VBOOT_MODE_DEVELOPER 1
#define VBOOT_MODE_RECOVERY  2



#define EC_CMD_I2C_PASSTHRU_PROTECT 0x00B7

enum ec_i2c_passthru_protect_subcmd {
	EC_CMD_I2C_PASSTHRU_PROTECT_STATUS = 0x0,
	EC_CMD_I2C_PASSTHRU_PROTECT_ENABLE = 0x1,
};

struct ec_params_i2c_passthru_protect {
	uint8_t subcmd;
	uint8_t port;		
} __ec_align1;

struct ec_response_i2c_passthru_protect {
	uint8_t status;		
} __ec_align1;





#define MAX_CEC_MSG_LEN 16


#define EC_CMD_CEC_WRITE_MSG 0x00B8


struct ec_params_cec_write {
	uint8_t msg[MAX_CEC_MSG_LEN];
} __ec_align1;


#define EC_CMD_CEC_SET 0x00BA


struct ec_params_cec_set {
	uint8_t cmd; 
	uint8_t val;
} __ec_align1;


#define EC_CMD_CEC_GET 0x00BB


struct ec_params_cec_get {
	uint8_t cmd; 
} __ec_align1;


struct ec_response_cec_get {
	uint8_t val;
} __ec_align1;


enum cec_command {
	
	CEC_CMD_ENABLE,
	
	CEC_CMD_LOGICAL_ADDRESS,
};


enum mkbp_cec_event {
	
	EC_MKBP_CEC_SEND_OK			= BIT(0),
	
	EC_MKBP_CEC_SEND_FAILED			= BIT(1),
};




#define EC_CMD_EC_CODEC 0x00BC

enum ec_codec_subcmd {
	EC_CODEC_GET_CAPABILITIES = 0x0,
	EC_CODEC_GET_SHM_ADDR = 0x1,
	EC_CODEC_SET_SHM_ADDR = 0x2,
	EC_CODEC_SUBCMD_COUNT,
};

enum ec_codec_cap {
	EC_CODEC_CAP_WOV_AUDIO_SHM = 0,
	EC_CODEC_CAP_WOV_LANG_SHM = 1,
	EC_CODEC_CAP_LAST = 32,
};

enum ec_codec_shm_id {
	EC_CODEC_SHM_ID_WOV_AUDIO = 0x0,
	EC_CODEC_SHM_ID_WOV_LANG = 0x1,
	EC_CODEC_SHM_ID_LAST,
};

enum ec_codec_shm_type {
	EC_CODEC_SHM_TYPE_EC_RAM = 0x0,
	EC_CODEC_SHM_TYPE_SYSTEM_RAM = 0x1,
};

struct __ec_align1 ec_param_ec_codec_get_shm_addr {
	uint8_t shm_id;
	uint8_t reserved[3];
};

struct __ec_align4 ec_param_ec_codec_set_shm_addr {
	uint64_t phys_addr;
	uint32_t len;
	uint8_t shm_id;
	uint8_t reserved[3];
};

struct __ec_align4 ec_param_ec_codec {
	uint8_t cmd; 
	uint8_t reserved[3];

	union {
		struct ec_param_ec_codec_get_shm_addr
				get_shm_addr_param;
		struct ec_param_ec_codec_set_shm_addr
				set_shm_addr_param;
	};
};

struct __ec_align4 ec_response_ec_codec_get_capabilities {
	uint32_t capabilities;
};

struct __ec_align4 ec_response_ec_codec_get_shm_addr {
	uint64_t phys_addr;
	uint32_t len;
	uint8_t type;
	uint8_t reserved[3];
};




#define EC_CMD_EC_CODEC_DMIC 0x00BD

enum ec_codec_dmic_subcmd {
	EC_CODEC_DMIC_GET_MAX_GAIN = 0x0,
	EC_CODEC_DMIC_SET_GAIN_IDX = 0x1,
	EC_CODEC_DMIC_GET_GAIN_IDX = 0x2,
	EC_CODEC_DMIC_SUBCMD_COUNT,
};

enum ec_codec_dmic_channel {
	EC_CODEC_DMIC_CHANNEL_0 = 0x0,
	EC_CODEC_DMIC_CHANNEL_1 = 0x1,
	EC_CODEC_DMIC_CHANNEL_2 = 0x2,
	EC_CODEC_DMIC_CHANNEL_3 = 0x3,
	EC_CODEC_DMIC_CHANNEL_4 = 0x4,
	EC_CODEC_DMIC_CHANNEL_5 = 0x5,
	EC_CODEC_DMIC_CHANNEL_6 = 0x6,
	EC_CODEC_DMIC_CHANNEL_7 = 0x7,
	EC_CODEC_DMIC_CHANNEL_COUNT,
};

struct __ec_align1 ec_param_ec_codec_dmic_set_gain_idx {
	uint8_t channel; 
	uint8_t gain;
	uint8_t reserved[2];
};

struct __ec_align1 ec_param_ec_codec_dmic_get_gain_idx {
	uint8_t channel; 
	uint8_t reserved[3];
};

struct __ec_align4 ec_param_ec_codec_dmic {
	uint8_t cmd; 
	uint8_t reserved[3];

	union {
		struct ec_param_ec_codec_dmic_set_gain_idx
				set_gain_idx_param;
		struct ec_param_ec_codec_dmic_get_gain_idx
				get_gain_idx_param;
	};
};

struct __ec_align1 ec_response_ec_codec_dmic_get_max_gain {
	uint8_t max_gain;
};

struct __ec_align1 ec_response_ec_codec_dmic_get_gain_idx {
	uint8_t gain;
};





#define EC_CMD_EC_CODEC_I2S_RX 0x00BE

enum ec_codec_i2s_rx_subcmd {
	EC_CODEC_I2S_RX_ENABLE = 0x0,
	EC_CODEC_I2S_RX_DISABLE = 0x1,
	EC_CODEC_I2S_RX_SET_SAMPLE_DEPTH = 0x2,
	EC_CODEC_I2S_RX_SET_DAIFMT = 0x3,
	EC_CODEC_I2S_RX_SET_BCLK = 0x4,
	EC_CODEC_I2S_RX_RESET = 0x5,
	EC_CODEC_I2S_RX_SUBCMD_COUNT,
};

enum ec_codec_i2s_rx_sample_depth {
	EC_CODEC_I2S_RX_SAMPLE_DEPTH_16 = 0x0,
	EC_CODEC_I2S_RX_SAMPLE_DEPTH_24 = 0x1,
	EC_CODEC_I2S_RX_SAMPLE_DEPTH_COUNT,
};

enum ec_codec_i2s_rx_daifmt {
	EC_CODEC_I2S_RX_DAIFMT_I2S = 0x0,
	EC_CODEC_I2S_RX_DAIFMT_RIGHT_J = 0x1,
	EC_CODEC_I2S_RX_DAIFMT_LEFT_J = 0x2,
	EC_CODEC_I2S_RX_DAIFMT_COUNT,
};

struct __ec_align1 ec_param_ec_codec_i2s_rx_set_sample_depth {
	uint8_t depth;
	uint8_t reserved[3];
};

struct __ec_align1 ec_param_ec_codec_i2s_rx_set_gain {
	uint8_t left;
	uint8_t right;
	uint8_t reserved[2];
};

struct __ec_align1 ec_param_ec_codec_i2s_rx_set_daifmt {
	uint8_t daifmt;
	uint8_t reserved[3];
};

struct __ec_align4 ec_param_ec_codec_i2s_rx_set_bclk {
	uint32_t bclk;
};

struct __ec_align4 ec_param_ec_codec_i2s_rx {
	uint8_t cmd; 
	uint8_t reserved[3];

	union {
		struct ec_param_ec_codec_i2s_rx_set_sample_depth
				set_sample_depth_param;
		struct ec_param_ec_codec_i2s_rx_set_daifmt
				set_daifmt_param;
		struct ec_param_ec_codec_i2s_rx_set_bclk
				set_bclk_param;
	};
};




#define EC_CMD_EC_CODEC_WOV 0x00BF

enum ec_codec_wov_subcmd {
	EC_CODEC_WOV_SET_LANG = 0x0,
	EC_CODEC_WOV_SET_LANG_SHM = 0x1,
	EC_CODEC_WOV_GET_LANG = 0x2,
	EC_CODEC_WOV_ENABLE = 0x3,
	EC_CODEC_WOV_DISABLE = 0x4,
	EC_CODEC_WOV_READ_AUDIO = 0x5,
	EC_CODEC_WOV_READ_AUDIO_SHM = 0x6,
	EC_CODEC_WOV_SUBCMD_COUNT,
};


struct __ec_align4 ec_param_ec_codec_wov_set_lang {
	uint8_t hash[32];
	uint32_t total_len;
	uint32_t offset;
	uint8_t buf[128];
	uint32_t len;
};

struct __ec_align4 ec_param_ec_codec_wov_set_lang_shm {
	uint8_t hash[32];
	uint32_t total_len;
};

struct __ec_align4 ec_param_ec_codec_wov {
	uint8_t cmd; 
	uint8_t reserved[3];

	union {
		struct ec_param_ec_codec_wov_set_lang
				set_lang_param;
		struct ec_param_ec_codec_wov_set_lang_shm
				set_lang_shm_param;
	};
};

struct __ec_align4 ec_response_ec_codec_wov_get_lang {
	uint8_t hash[32];
};

struct __ec_align4 ec_response_ec_codec_wov_read_audio {
	uint8_t buf[128];
	uint32_t len;
};

struct __ec_align4 ec_response_ec_codec_wov_read_audio_shm {
	uint32_t offset;
	uint32_t len;
};





#define EC_CMD_REBOOT_EC 0x00D2


enum ec_reboot_cmd {
	EC_REBOOT_CANCEL = 0,        
	EC_REBOOT_JUMP_RO = 1,       
	EC_REBOOT_JUMP_RW = 2,       
	
	EC_REBOOT_COLD = 4,          
	EC_REBOOT_DISABLE_JUMP = 5,  
	EC_REBOOT_HIBERNATE = 6,     
	EC_REBOOT_HIBERNATE_CLEAR_AP_OFF = 7, 
	EC_REBOOT_COLD_AP_OFF = 8,   
};


#define EC_REBOOT_FLAG_RESERVED0      BIT(0)  
#define EC_REBOOT_FLAG_ON_AP_SHUTDOWN BIT(1)  
#define EC_REBOOT_FLAG_SWITCH_RW_SLOT BIT(2)  

struct ec_params_reboot_ec {
	uint8_t cmd;           
	uint8_t flags;         
} __ec_align1;


#define EC_CMD_GET_PANIC_INFO 0x00D3





#define EC_CMD_REBOOT 0x00D1  


#define EC_CMD_RESEND_RESPONSE 0x00DB


#define EC_CMD_VERSION0 0x00DC





#define EC_CMD_PD_EXCHANGE_STATUS 0x0100
#define EC_VER_PD_EXCHANGE_STATUS 2

enum pd_charge_state {
	PD_CHARGE_NO_CHANGE = 0, 
	PD_CHARGE_NONE,          
	PD_CHARGE_5V,            
	PD_CHARGE_MAX            
};


#define EC_STATUS_HIBERNATING	BIT(0)

struct ec_params_pd_status {
	uint8_t status;       
	int8_t batt_soc;      
	uint8_t charge_state; 
} __ec_align1;


#define PD_STATUS_HOST_EVENT      BIT(0) 
#define PD_STATUS_IN_RW           BIT(1) 
#define PD_STATUS_JUMPED_TO_IMAGE BIT(2) 
#define PD_STATUS_TCPC_ALERT_0    BIT(3) 
#define PD_STATUS_TCPC_ALERT_1    BIT(4) 
#define PD_STATUS_TCPC_ALERT_2    BIT(5) 
#define PD_STATUS_TCPC_ALERT_3    BIT(6) 
#define PD_STATUS_EC_INT_ACTIVE  (PD_STATUS_TCPC_ALERT_0 | \
				      PD_STATUS_TCPC_ALERT_1 | \
				      PD_STATUS_HOST_EVENT)
struct ec_response_pd_status {
	uint32_t curr_lim_ma;       
	uint16_t status;            
	int8_t active_charge_port;  
} __ec_align_size1;


#define EC_CMD_PD_HOST_EVENT_STATUS 0x0104


#define PD_EVENT_UPDATE_DEVICE     BIT(0)
#define PD_EVENT_POWER_CHANGE      BIT(1)
#define PD_EVENT_IDENTITY_RECEIVED BIT(2)
#define PD_EVENT_DATA_SWAP         BIT(3)
struct ec_response_host_event_status {
	uint32_t status;      
} __ec_align4;


#define EC_CMD_USB_PD_CONTROL 0x0101

enum usb_pd_control_role {
	USB_PD_CTRL_ROLE_NO_CHANGE = 0,
	USB_PD_CTRL_ROLE_TOGGLE_ON = 1, 
	USB_PD_CTRL_ROLE_TOGGLE_OFF = 2,
	USB_PD_CTRL_ROLE_FORCE_SINK = 3,
	USB_PD_CTRL_ROLE_FORCE_SOURCE = 4,
	USB_PD_CTRL_ROLE_FREEZE = 5,
	USB_PD_CTRL_ROLE_COUNT
};

enum usb_pd_control_mux {
	USB_PD_CTRL_MUX_NO_CHANGE = 0,
	USB_PD_CTRL_MUX_NONE = 1,
	USB_PD_CTRL_MUX_USB = 2,
	USB_PD_CTRL_MUX_DP = 3,
	USB_PD_CTRL_MUX_DOCK = 4,
	USB_PD_CTRL_MUX_AUTO = 5,
	USB_PD_CTRL_MUX_COUNT
};

enum usb_pd_control_swap {
	USB_PD_CTRL_SWAP_NONE = 0,
	USB_PD_CTRL_SWAP_DATA = 1,
	USB_PD_CTRL_SWAP_POWER = 2,
	USB_PD_CTRL_SWAP_VCONN = 3,
	USB_PD_CTRL_SWAP_COUNT
};

struct ec_params_usb_pd_control {
	uint8_t port;
	uint8_t role;
	uint8_t mux;
	uint8_t swap;
} __ec_align1;

#define PD_CTRL_RESP_ENABLED_COMMS      BIT(0) 
#define PD_CTRL_RESP_ENABLED_CONNECTED  BIT(1) 
#define PD_CTRL_RESP_ENABLED_PD_CAPABLE BIT(2) 

#define PD_CTRL_RESP_ROLE_POWER         BIT(0) 
#define PD_CTRL_RESP_ROLE_DATA          BIT(1) 
#define PD_CTRL_RESP_ROLE_VCONN         BIT(2) 
#define PD_CTRL_RESP_ROLE_DR_POWER      BIT(3) 
#define PD_CTRL_RESP_ROLE_DR_DATA       BIT(4) 
#define PD_CTRL_RESP_ROLE_USB_COMM      BIT(5) 
#define PD_CTRL_RESP_ROLE_EXT_POWERED   BIT(6) 

struct ec_response_usb_pd_control {
	uint8_t enabled;
	uint8_t role;
	uint8_t polarity;
	uint8_t state;
} __ec_align1;

struct ec_response_usb_pd_control_v1 {
	uint8_t enabled;
	uint8_t role;
	uint8_t polarity;
	char state[32];
} __ec_align1;


#define USBC_PD_CC_NONE		0 
#define USBC_PD_CC_NO_UFP	1 
#define USBC_PD_CC_AUDIO_ACC	2 
#define USBC_PD_CC_DEBUG_ACC	3 
#define USBC_PD_CC_UFP_ATTACHED	4 
#define USBC_PD_CC_DFP_ATTACHED	5 


#define USB_PD_CTRL_ACTIVE_CABLE        BIT(0)

#define USB_PD_CTRL_OPTICAL_CABLE       BIT(1)

#define USB_PD_CTRL_TBT_LEGACY_ADAPTER  BIT(2)

#define USB_PD_CTRL_ACTIVE_LINK_UNIDIR  BIT(3)

struct ec_response_usb_pd_control_v2 {
	uint8_t enabled;
	uint8_t role;
	uint8_t polarity;
	char state[32];
	uint8_t cc_state;	
	uint8_t dp_mode;	
	uint8_t reserved;	
	uint8_t control_flags;	
	uint8_t cable_speed;	
	uint8_t cable_gen;	
} __ec_align1;

#define EC_CMD_USB_PD_PORTS 0x0102


#define EC_USB_PD_MAX_PORTS 8

struct ec_response_usb_pd_ports {
	uint8_t num_ports;
} __ec_align1;

#define EC_CMD_USB_PD_POWER_INFO 0x0103

#define PD_POWER_CHARGING_PORT 0xff
struct ec_params_usb_pd_power_info {
	uint8_t port;
} __ec_align1;

enum usb_chg_type {
	USB_CHG_TYPE_NONE,
	USB_CHG_TYPE_PD,
	USB_CHG_TYPE_C,
	USB_CHG_TYPE_PROPRIETARY,
	USB_CHG_TYPE_BC12_DCP,
	USB_CHG_TYPE_BC12_CDP,
	USB_CHG_TYPE_BC12_SDP,
	USB_CHG_TYPE_OTHER,
	USB_CHG_TYPE_VBUS,
	USB_CHG_TYPE_UNKNOWN,
	USB_CHG_TYPE_DEDICATED,
};
enum usb_power_roles {
	USB_PD_PORT_POWER_DISCONNECTED,
	USB_PD_PORT_POWER_SOURCE,
	USB_PD_PORT_POWER_SINK,
	USB_PD_PORT_POWER_SINK_NOT_CHARGING,
};

struct usb_chg_measures {
	uint16_t voltage_max;
	uint16_t voltage_now;
	uint16_t current_max;
	uint16_t current_lim;
} __ec_align2;

struct ec_response_usb_pd_power_info {
	uint8_t role;
	uint8_t type;
	uint8_t dualrole;
	uint8_t reserved1;
	struct usb_chg_measures meas;
	uint32_t max_power;
} __ec_align4;



#define EC_CMD_CHARGE_PORT_COUNT 0x0105
struct ec_response_charge_port_count {
	uint8_t port_count;
} __ec_align1;


#define EC_CMD_USB_PD_FW_UPDATE 0x0110

enum usb_pd_fw_update_cmds {
	USB_PD_FW_REBOOT,
	USB_PD_FW_FLASH_ERASE,
	USB_PD_FW_FLASH_WRITE,
	USB_PD_FW_ERASE_SIG,
};

struct ec_params_usb_pd_fw_update {
	uint16_t dev_id;
	uint8_t cmd;
	uint8_t port;
	uint32_t size;     
	
} __ec_align4;


#define EC_CMD_USB_PD_RW_HASH_ENTRY 0x0111

#define PD_RW_HASH_SIZE 20
struct ec_params_usb_pd_rw_hash_entry {
	uint16_t dev_id;
	uint8_t dev_rw_hash[PD_RW_HASH_SIZE];
	uint8_t reserved;        
	uint32_t current_image;  
} __ec_align1;


#define EC_CMD_USB_PD_DEV_INFO 0x0112

struct ec_params_usb_pd_info_request {
	uint8_t port;
} __ec_align1;


#define EC_CMD_USB_PD_DISCOVERY 0x0113
struct ec_params_usb_pd_discovery_entry {
	uint16_t vid;  
	uint16_t pid;  
	uint8_t ptype; 
} __ec_align_size1;


#define EC_CMD_PD_CHARGE_PORT_OVERRIDE 0x0114


enum usb_pd_override_ports {
	OVERRIDE_DONT_CHARGE = -2,
	OVERRIDE_OFF = -1,
	
};

struct ec_params_charge_port_override {
	int16_t override_port; 
} __ec_align2;


#define EC_CMD_PD_GET_LOG_ENTRY 0x0115

struct ec_response_pd_log {
	uint32_t timestamp; 
	uint8_t type;       
	uint8_t size_port;  
	uint16_t data;      
	uint8_t payload[];  
} __ec_align4;


#define PD_LOG_TIMESTAMP_SHIFT 10 

#define PD_LOG_SIZE_MASK  0x1f
#define PD_LOG_PORT_MASK  0xe0
#define PD_LOG_PORT_SHIFT    5
#define PD_LOG_PORT_SIZE(port, size) (((port) << PD_LOG_PORT_SHIFT) | \
				      ((size) & PD_LOG_SIZE_MASK))
#define PD_LOG_PORT(size_port) ((size_port) >> PD_LOG_PORT_SHIFT)
#define PD_LOG_SIZE(size_port) ((size_port) & PD_LOG_SIZE_MASK)



#define PD_EVENT_MCU_BASE       0x00
#define PD_EVENT_MCU_CHARGE             (PD_EVENT_MCU_BASE+0)
#define PD_EVENT_MCU_CONNECT            (PD_EVENT_MCU_BASE+1)

#define PD_EVENT_MCU_BOARD_CUSTOM       (PD_EVENT_MCU_BASE+2)

#define PD_EVENT_ACC_BASE       0x20
#define PD_EVENT_ACC_RW_FAIL   (PD_EVENT_ACC_BASE+0)
#define PD_EVENT_ACC_RW_ERASE  (PD_EVENT_ACC_BASE+1)

#define PD_EVENT_PS_BASE        0x40
#define PD_EVENT_PS_FAULT      (PD_EVENT_PS_BASE+0)

#define PD_EVENT_VIDEO_BASE     0x60
#define PD_EVENT_VIDEO_DP_MODE (PD_EVENT_VIDEO_BASE+0)
#define PD_EVENT_VIDEO_CODEC   (PD_EVENT_VIDEO_BASE+1)

#define PD_EVENT_NO_ENTRY       0xff



#define CHARGE_FLAGS_DUAL_ROLE         BIT(15)

#define CHARGE_FLAGS_DELAYED_OVERRIDE  BIT(14)

#define CHARGE_FLAGS_OVERRIDE          BIT(13)

#define CHARGE_FLAGS_TYPE_SHIFT               3
#define CHARGE_FLAGS_TYPE_MASK       (0xf << CHARGE_FLAGS_TYPE_SHIFT)

#define CHARGE_FLAGS_ROLE_MASK         (7 <<  0)


#define PS_FAULT_OCP                          1
#define PS_FAULT_FAST_OCP                     2
#define PS_FAULT_OVP                          3
#define PS_FAULT_DISCH                        4


struct mcdp_version {
	uint8_t major;
	uint8_t minor;
	uint16_t build;
} __ec_align4;

struct mcdp_info {
	uint8_t family[2];
	uint8_t chipid[2];
	struct mcdp_version irom;
	struct mcdp_version fw;
} __ec_align4;


#define MCDP_CHIPID(chipid) ((chipid[0] << 8) | chipid[1])
#define MCDP_FAMILY(family) ((family[0] << 8) | family[1])


#define EC_CMD_USB_PD_GET_AMODE 0x0116
struct ec_params_usb_pd_get_mode_request {
	uint16_t svid_idx; 
	uint8_t port;      
} __ec_align_size1;

struct ec_params_usb_pd_get_mode_response {
	uint16_t svid;   
	uint16_t opos;    
	uint32_t vdo[6]; 
} __ec_align4;

#define EC_CMD_USB_PD_SET_AMODE 0x0117

enum pd_mode_cmd {
	PD_EXIT_MODE = 0,
	PD_ENTER_MODE = 1,
	
	PD_MODE_CMD_COUNT,
};

struct ec_params_usb_pd_set_mode_request {
	uint32_t cmd;  
	uint16_t svid; 
	uint8_t opos;  
	uint8_t port;  
} __ec_align4;


#define EC_CMD_PD_WRITE_LOG_ENTRY 0x0118

struct ec_params_pd_write_log_entry {
	uint8_t type; 
	uint8_t port; 
} __ec_align1;



#define EC_CMD_PD_CONTROL 0x0119

enum ec_pd_control_cmd {
	PD_SUSPEND = 0,      
	PD_RESUME,           
	PD_RESET,            
	PD_CONTROL_DISABLE,  
	PD_CHIP_ON,          
};

struct ec_params_pd_control {
	uint8_t chip;         
	uint8_t subcmd;
} __ec_align1;


#define EC_CMD_USB_PD_MUX_INFO 0x011A

struct ec_params_usb_pd_mux_info {
	uint8_t port; 
} __ec_align1;


#define USB_PD_MUX_NONE               0      
#define USB_PD_MUX_USB_ENABLED        BIT(0) 
#define USB_PD_MUX_DP_ENABLED         BIT(1) 
#define USB_PD_MUX_POLARITY_INVERTED  BIT(2) 
#define USB_PD_MUX_HPD_IRQ            BIT(3) 
#define USB_PD_MUX_HPD_LVL            BIT(4) 
#define USB_PD_MUX_SAFE_MODE          BIT(5) 
#define USB_PD_MUX_TBT_COMPAT_ENABLED BIT(6) 
#define USB_PD_MUX_USB4_ENABLED       BIT(7) 

struct ec_response_usb_pd_mux_info {
	uint8_t flags; 
} __ec_align1;

#define EC_CMD_PD_CHIP_INFO		0x011B

struct ec_params_pd_chip_info {
	uint8_t port;	
	uint8_t renew;	
} __ec_align1;

struct ec_response_pd_chip_info {
	uint16_t vendor_id;
	uint16_t product_id;
	uint16_t device_id;
	union {
		uint8_t fw_version_string[8];
		uint64_t fw_version_number;
	};
} __ec_align2;

struct ec_response_pd_chip_info_v1 {
	uint16_t vendor_id;
	uint16_t product_id;
	uint16_t device_id;
	union {
		uint8_t fw_version_string[8];
		uint64_t fw_version_number;
	};
	union {
		uint8_t min_req_fw_version_string[8];
		uint64_t min_req_fw_version_number;
	};
} __ec_align2;


#define EC_CMD_RWSIG_CHECK_STATUS	0x011C

struct ec_response_rwsig_check_status {
	uint32_t status;
} __ec_align4;


#define EC_CMD_RWSIG_ACTION	0x011D

enum rwsig_action {
	RWSIG_ACTION_ABORT = 0,		
	RWSIG_ACTION_CONTINUE = 1,	
};

struct ec_params_rwsig_action {
	uint32_t action;
} __ec_align4;


#define EC_CMD_EFS_VERIFY	0x011E

struct ec_params_efs_verify {
	uint8_t region;		
} __ec_align1;


#define EC_CMD_GET_CROS_BOARD_INFO	0x011F

#define EC_CMD_SET_CROS_BOARD_INFO	0x0120

enum cbi_data_tag {
	CBI_TAG_BOARD_VERSION = 0, 
	CBI_TAG_OEM_ID = 1,        
	CBI_TAG_SKU_ID = 2,        
	CBI_TAG_DRAM_PART_NUM = 3, 
	CBI_TAG_OEM_NAME = 4,      
	CBI_TAG_MODEL_ID = 5,      
	CBI_TAG_COUNT,
};


#define CBI_GET_RELOAD		BIT(0)

struct ec_params_get_cbi {
	uint32_t tag;		
	uint32_t flag;		
} __ec_align4;


#define CBI_SET_NO_SYNC		BIT(0)
#define CBI_SET_INIT		BIT(1)

struct ec_params_set_cbi {
	uint32_t tag;		
	uint32_t flag;		
	uint32_t size;		
	uint8_t data[];		
} __ec_align1;


#define EC_CMD_GET_UPTIME_INFO 0x0121

struct ec_response_uptime_info {
	
	uint32_t time_since_ec_boot_ms;

	
	uint32_t ap_resets_since_ec_boot;

	
	uint32_t ec_reset_flags;

	
	struct ap_reset_log_entry {
		
		uint16_t reset_cause;

		
		uint16_t reserved;

		
		uint32_t reset_time_ms;
	} recent_ap_reset[4];
} __ec_align4;


#define EC_CMD_ADD_ENTROPY	0x0122

enum add_entropy_action {
	
	ADD_ENTROPY_ASYNC = 0,
	
	ADD_ENTROPY_RESET_ASYNC = 1,
	
	ADD_ENTROPY_GET_RESULT = 2,
};

struct ec_params_rollback_add_entropy {
	uint8_t action;
} __ec_align1;


#define EC_CMD_ADC_READ		0x0123

struct ec_params_adc_read {
	uint8_t adc_channel;
} __ec_align1;

struct ec_response_adc_read {
	int32_t adc_value;
} __ec_align4;


#define EC_CMD_ROLLBACK_INFO		0x0124

struct ec_response_rollback_info {
	int32_t id; 
	int32_t rollback_min_version;
	int32_t rw_rollback_version;
} __ec_align4;



#define EC_CMD_AP_RESET 0x0125


#define EC_CMD_PCHG_COUNT 0x0134

#define EC_PCHG_MAX_PORTS 8

struct ec_response_pchg_count {
	uint8_t port_count;
} __ec_align1;


#define EC_CMD_PCHG 0x0135

struct ec_params_pchg {
	uint8_t port;
} __ec_align1;

struct ec_response_pchg {
	uint32_t error;			
	uint8_t state;			
	uint8_t battery_percentage;
	uint8_t unused0;
	uint8_t unused1;
	
	uint32_t fw_version;
	uint32_t dropped_event_count;
} __ec_align2;

enum pchg_state {
	
	PCHG_STATE_RESET = 0,
	
	PCHG_STATE_INITIALIZED,
	
	PCHG_STATE_ENABLED,
	
	PCHG_STATE_DETECTED,
	
	PCHG_STATE_CHARGING,
	
	PCHG_STATE_FULL,
	
	PCHG_STATE_DOWNLOAD,
	
	PCHG_STATE_DOWNLOADING,
	
	PCHG_STATE_CONNECTED,
	
	PCHG_STATE_COUNT,
};

#define EC_PCHG_STATE_TEXT { \
	[PCHG_STATE_RESET] = "RESET", \
	[PCHG_STATE_INITIALIZED] = "INITIALIZED", \
	[PCHG_STATE_ENABLED] = "ENABLED", \
	[PCHG_STATE_DETECTED] = "DETECTED", \
	[PCHG_STATE_CHARGING] = "CHARGING", \
	[PCHG_STATE_FULL] = "FULL", \
	[PCHG_STATE_DOWNLOAD] = "DOWNLOAD", \
	[PCHG_STATE_DOWNLOADING] = "DOWNLOADING", \
	[PCHG_STATE_CONNECTED] = "CONNECTED", \
	}


#define EC_CMD_PCHG_UPDATE 0x0136


#define EC_MKBP_PCHG_PORT_SHIFT		28

#define EC_MKBP_PCHG_EVENT_TO_PORT(e)	(((e) >> EC_MKBP_PCHG_PORT_SHIFT) & 0xf)

#define EC_MKBP_PCHG_EVENT_MASK(e)	((e) \
					& GENMASK(EC_MKBP_PCHG_PORT_SHIFT-1, 0))

#define EC_MKBP_PCHG_UPDATE_OPENED	BIT(0)
#define EC_MKBP_PCHG_WRITE_COMPLETE	BIT(1)
#define EC_MKBP_PCHG_UPDATE_CLOSED	BIT(2)
#define EC_MKBP_PCHG_UPDATE_ERROR	BIT(3)
#define EC_MKBP_PCHG_DEVICE_EVENT	BIT(4)

enum ec_pchg_update_cmd {
	
	EC_PCHG_UPDATE_CMD_RESET_TO_NORMAL = 0,
	
	EC_PCHG_UPDATE_CMD_OPEN,
	
	EC_PCHG_UPDATE_CMD_WRITE,
	
	EC_PCHG_UPDATE_CMD_CLOSE,
	
	EC_PCHG_UPDATE_CMD_COUNT,
};

struct ec_params_pchg_update {
	
	uint8_t port;
	
	uint8_t cmd;
	
	uint8_t reserved0;
	uint8_t reserved1;
	
	uint32_t version;
	
	uint32_t crc32;
	
	uint32_t addr;
	
	uint32_t size;
	
	uint8_t data[];
} __ec_align4;

BUILD_ASSERT(EC_PCHG_UPDATE_CMD_COUNT
	     < BIT(sizeof(((struct ec_params_pchg_update *)0)->cmd)*8));

struct ec_response_pchg_update {
	
	uint32_t block_size;
} __ec_align4;






#define EC_CMD_REGULATOR_GET_INFO 0x012C


#define EC_REGULATOR_NAME_MAX_LEN 16


#define EC_REGULATOR_VOLTAGE_MAX_COUNT 16

struct ec_params_regulator_get_info {
	uint32_t index;
} __ec_align4;

struct ec_response_regulator_get_info {
	char name[EC_REGULATOR_NAME_MAX_LEN];
	uint16_t num_voltages;
	uint16_t voltages_mv[EC_REGULATOR_VOLTAGE_MAX_COUNT];
} __ec_align2;


#define EC_CMD_REGULATOR_ENABLE 0x012D

struct ec_params_regulator_enable {
	uint32_t index;
	uint8_t enable;
} __ec_align4;


#define EC_CMD_REGULATOR_IS_ENABLED 0x012E

struct ec_params_regulator_is_enabled {
	uint32_t index;
} __ec_align4;

struct ec_response_regulator_is_enabled {
	uint8_t enabled;
} __ec_align1;


#define EC_CMD_REGULATOR_SET_VOLTAGE 0x012F

struct ec_params_regulator_set_voltage {
	uint32_t index;
	uint32_t min_mv;
	uint32_t max_mv;
} __ec_align4;


#define EC_CMD_REGULATOR_GET_VOLTAGE 0x0130

struct ec_params_regulator_get_voltage {
	uint32_t index;
} __ec_align4;

struct ec_response_regulator_get_voltage {
	uint32_t voltage_mv;
} __ec_align4;


#define EC_CMD_TYPEC_DISCOVERY 0x0131

enum typec_partner_type {
	TYPEC_PARTNER_SOP = 0,
	TYPEC_PARTNER_SOP_PRIME = 1,
};

struct ec_params_typec_discovery {
	uint8_t port;
	uint8_t partner_type; 
} __ec_align1;

struct svid_mode_info {
	uint16_t svid;
	uint16_t mode_count;  
	uint32_t mode_vdo[6]; 
};

struct ec_response_typec_discovery {
	uint8_t identity_count;    
	uint8_t svid_count;	   
	uint16_t reserved;
	uint32_t discovery_vdo[6]; 
	struct svid_mode_info svids[];
} __ec_align1;


#define EC_CMD_TYPEC_CONTROL 0x0132

enum typec_control_command {
	TYPEC_CONTROL_COMMAND_EXIT_MODES,
	TYPEC_CONTROL_COMMAND_CLEAR_EVENTS,
	TYPEC_CONTROL_COMMAND_ENTER_MODE,
	TYPEC_CONTROL_COMMAND_TBT_UFP_REPLY,
	TYPEC_CONTROL_COMMAND_USB_MUX_SET,
	TYPEC_CONTROL_COMMAND_BIST_SHARE_MODE,
	TYPEC_CONTROL_COMMAND_SEND_VDM_REQ,
};


enum typec_tbt_ufp_reply {
	TYPEC_TBT_UFP_REPLY_NAK,
	TYPEC_TBT_UFP_REPLY_ACK,
};

struct typec_usb_mux_set {
	uint8_t mux_index;	
	uint8_t mux_flags;	
} __ec_align1;

#define VDO_MAX_SIZE 7

struct typec_vdm_req {
	
	uint32_t vdm_data[VDO_MAX_SIZE];
	
	uint8_t vdm_data_objects;
	
	uint8_t partner_type;
} __ec_align1;

struct ec_params_typec_control {
	uint8_t port;
	uint8_t command;	
	uint16_t reserved;

	
	union {
		uint32_t clear_events_mask;
		uint8_t mode_to_enter;      
		uint8_t tbt_ufp_reply;      
		struct typec_usb_mux_set mux_params;
		
		struct typec_vdm_req vdm_req_params;
		uint8_t placeholder[128];
	};
} __ec_align1;


#define EC_CMD_TYPEC_STATUS 0x0133


enum pd_power_role {
	PD_ROLE_SINK = 0,
	PD_ROLE_SOURCE = 1
};


enum pd_data_role {
	PD_ROLE_UFP = 0,
	PD_ROLE_DFP = 1,
	PD_ROLE_DISCONNECTED = 2,
};

enum pd_vconn_role {
	PD_ROLE_VCONN_OFF = 0,
	PD_ROLE_VCONN_SRC = 1,
};


enum tcpc_cc_polarity {
	
	POLARITY_CC1 = 0,
	POLARITY_CC2 = 1,

	
	POLARITY_CC1_DTS = 2,
	POLARITY_CC2_DTS = 3,

	
	POLARITY_COUNT
};

#define PD_STATUS_EVENT_SOP_DISC_DONE		BIT(0)
#define PD_STATUS_EVENT_SOP_PRIME_DISC_DONE	BIT(1)
#define PD_STATUS_EVENT_HARD_RESET		BIT(2)
#define PD_STATUS_EVENT_DISCONNECTED		BIT(3)
#define PD_STATUS_EVENT_MUX_0_SET_DONE		BIT(4)
#define PD_STATUS_EVENT_MUX_1_SET_DONE		BIT(5)
#define PD_STATUS_EVENT_VDM_REQ_REPLY		BIT(6)
#define PD_STATUS_EVENT_VDM_REQ_FAILED		BIT(7)
#define PD_STATUS_EVENT_VDM_ATTENTION		BIT(8)

struct ec_params_typec_status {
	uint8_t port;
} __ec_align1;

struct ec_response_typec_status {
	uint8_t pd_enabled;		
	uint8_t dev_connected;		
	uint8_t sop_connected;		
	uint8_t source_cap_count;	

	uint8_t power_role;		
	uint8_t data_role;		
	uint8_t vconn_role;		
	uint8_t sink_cap_count;		

	uint8_t polarity;		
	uint8_t cc_state;		
	uint8_t dp_pin;			
	uint8_t mux_state;		

	char tc_state[32];		

	uint32_t events;		

	
	uint16_t sop_revision;
	uint16_t sop_prime_revision;

	uint32_t source_cap_pdos[7];	

	uint32_t sink_cap_pdos[7];	
} __ec_align1;


#define EC_CMD_TYPEC_VDM_RESPONSE 0x013C

struct ec_params_typec_vdm_response {
	uint8_t port;
} __ec_align1;

struct ec_response_typec_vdm_response {
	
	uint8_t vdm_data_objects;
	
	uint8_t partner_type;
	
	uint16_t vdm_response_err;
	
	uint32_t vdm_response[VDO_MAX_SIZE];
	
	uint8_t vdm_attention_objects;
	
	uint8_t vdm_attention_left;
	
	uint16_t reserved1;
	
	uint32_t vdm_attention[2];
} __ec_align1;

#undef VDO_MAX_SIZE






#define EC_CMD_CR51_BASE 0x0300
#define EC_CMD_CR51_LAST 0x03FF





#define EC_CMD_FP_PASSTHRU 0x0400

#define EC_FP_FLAG_NOT_COMPLETE 0x1

struct ec_params_fp_passthru {
	uint16_t len;		
	uint16_t flags;		
	uint8_t data[];		
} __ec_align2;


#define EC_CMD_FP_MODE 0x0402


#define FP_MODE_DEEPSLEEP      BIT(0)

#define FP_MODE_FINGER_DOWN    BIT(1)

#define FP_MODE_FINGER_UP      BIT(2)

#define FP_MODE_CAPTURE        BIT(3)

#define FP_MODE_ENROLL_SESSION BIT(4)

#define FP_MODE_ENROLL_IMAGE   BIT(5)

#define FP_MODE_MATCH          BIT(6)

#define FP_MODE_RESET_SENSOR   BIT(7)

#define FP_MODE_DONT_CHANGE    BIT(31)

#define FP_VALID_MODES (FP_MODE_DEEPSLEEP      | \
			FP_MODE_FINGER_DOWN    | \
			FP_MODE_FINGER_UP      | \
			FP_MODE_CAPTURE        | \
			FP_MODE_ENROLL_SESSION | \
			FP_MODE_ENROLL_IMAGE   | \
			FP_MODE_MATCH          | \
			FP_MODE_RESET_SENSOR   | \
			FP_MODE_DONT_CHANGE)


#define FP_MODE_CAPTURE_TYPE_SHIFT 28
#define FP_MODE_CAPTURE_TYPE_MASK  (0x7 << FP_MODE_CAPTURE_TYPE_SHIFT)

enum fp_capture_type {
	
	FP_CAPTURE_VENDOR_FORMAT = 0,
	
	FP_CAPTURE_SIMPLE_IMAGE = 1,
	
	FP_CAPTURE_PATTERN0 = 2,
	
	FP_CAPTURE_PATTERN1 = 3,
	
	FP_CAPTURE_QUALITY_TEST = 4,
	
	FP_CAPTURE_RESET_TEST = 5,
	FP_CAPTURE_TYPE_MAX,
};

#define FP_CAPTURE_TYPE(mode) (((mode) & FP_MODE_CAPTURE_TYPE_MASK) \
				       >> FP_MODE_CAPTURE_TYPE_SHIFT)

struct ec_params_fp_mode {
	uint32_t mode; 
} __ec_align4;

struct ec_response_fp_mode {
	uint32_t mode; 
} __ec_align4;


#define EC_CMD_FP_INFO 0x0403


#define FP_ERROR_DEAD_PIXELS(errors) ((errors) & 0x3FF)

#define FP_ERROR_DEAD_PIXELS_UNKNOWN (0x3FF)

#define FP_ERROR_NO_IRQ    BIT(12)

#define FP_ERROR_SPI_COMM  BIT(13)

#define FP_ERROR_BAD_HWID  BIT(14)

#define FP_ERROR_INIT_FAIL BIT(15)

struct ec_response_fp_info_v0 {
	
	uint32_t vendor_id;
	uint32_t product_id;
	uint32_t model_id;
	uint32_t version;
	
	uint32_t frame_size;
	uint32_t pixel_format; 
	uint16_t width;
	uint16_t height;
	uint16_t bpp;
	uint16_t errors; 
} __ec_align4;

struct ec_response_fp_info {
	
	uint32_t vendor_id;
	uint32_t product_id;
	uint32_t model_id;
	uint32_t version;
	
	uint32_t frame_size;
	uint32_t pixel_format; 
	uint16_t width;
	uint16_t height;
	uint16_t bpp;
	uint16_t errors; 
	
	uint32_t template_size;  
	uint16_t template_max;   
	uint16_t template_valid; 
	uint32_t template_dirty; 
	uint32_t template_version; 
} __ec_align4;


#define EC_CMD_FP_FRAME 0x0404


#define FP_FRAME_INDEX_SHIFT       28

#define FP_FRAME_INDEX_RAW_IMAGE    0

#define FP_FRAME_INDEX_TEMPLATE     1
#define FP_FRAME_GET_BUFFER_INDEX(offset) ((offset) >> FP_FRAME_INDEX_SHIFT)
#define FP_FRAME_OFFSET_MASK       0x0FFFFFFF


#define FP_TEMPLATE_FORMAT_VERSION 3


#define FP_CONTEXT_NONCE_BYTES 12
#define FP_CONTEXT_USERID_WORDS (32 / sizeof(uint32_t))
#define FP_CONTEXT_TAG_BYTES 16
#define FP_CONTEXT_SALT_BYTES 16
#define FP_CONTEXT_TPM_BYTES 32

struct ec_fp_template_encryption_metadata {
	
	uint16_t struct_version;
	
	uint16_t reserved;
	
	uint8_t nonce[FP_CONTEXT_NONCE_BYTES];
	uint8_t salt[FP_CONTEXT_SALT_BYTES];
	uint8_t tag[FP_CONTEXT_TAG_BYTES];
};

struct ec_params_fp_frame {
	
	uint32_t offset;
	uint32_t size;
} __ec_align4;


#define EC_CMD_FP_TEMPLATE 0x0405


#define FP_TEMPLATE_COMMIT 0x80000000

struct ec_params_fp_template {
	uint32_t offset;
	uint32_t size;
	uint8_t data[];
} __ec_align4;


#define EC_CMD_FP_CONTEXT 0x0406

struct ec_params_fp_context {
	uint32_t userid[FP_CONTEXT_USERID_WORDS];
} __ec_align4;

#define EC_CMD_FP_STATS 0x0407

#define FPSTATS_CAPTURE_INV  BIT(0)
#define FPSTATS_MATCHING_INV BIT(1)

struct ec_response_fp_stats {
	uint32_t capture_time_us;
	uint32_t matching_time_us;
	uint32_t overall_time_us;
	struct {
		uint32_t lo;
		uint32_t hi;
	} overall_t0;
	uint8_t timestamps_invalid;
	int8_t template_matched;
} __ec_align2;

#define EC_CMD_FP_SEED 0x0408
struct ec_params_fp_seed {
	
	uint16_t struct_version;
	
	uint16_t reserved;
	
	uint8_t seed[FP_CONTEXT_TPM_BYTES];
} __ec_align4;

#define EC_CMD_FP_ENC_STATUS 0x0409


#define FP_ENC_STATUS_SEED_SET BIT(0)

struct ec_response_fp_encryption_status {
	
	uint32_t valid_flags;
	
	uint32_t status;
} __ec_align4;





#define EC_CMD_TP_SELF_TEST 0x0500


#define EC_CMD_TP_FRAME_INFO 0x0501

struct ec_response_tp_frame_info {
	uint32_t n_frames;
	uint32_t frame_sizes[];
} __ec_align4;


#define EC_CMD_TP_FRAME_SNAPSHOT 0x0502


#define EC_CMD_TP_FRAME_GET 0x0503

struct ec_params_tp_frame_get {
	uint32_t frame_index;
	uint32_t offset;
	uint32_t size;
} __ec_align4;




#define EC_COMM_TEXT_MAX 8


#define EC_CMD_BATTERY_GET_STATIC 0x0600


struct ec_params_battery_static_info {
	uint8_t index;
} __ec_align_size1;


struct ec_response_battery_static_info {
	uint16_t design_capacity;
	uint16_t design_voltage;
	char manufacturer[EC_COMM_TEXT_MAX];
	char model[EC_COMM_TEXT_MAX];
	char serial[EC_COMM_TEXT_MAX];
	char type[EC_COMM_TEXT_MAX];
	
	uint32_t cycle_count;
} __ec_align4;


#define EC_CMD_BATTERY_GET_DYNAMIC 0x0601


struct ec_params_battery_dynamic_info {
	uint8_t index;
} __ec_align_size1;


struct ec_response_battery_dynamic_info {
	int16_t actual_voltage;
	int16_t actual_current;
	int16_t remaining_capacity;
	int16_t full_capacity;
	int16_t flags;
	int16_t desired_voltage;
	int16_t desired_current;
} __ec_align2;


#define EC_CMD_CHARGER_CONTROL 0x0602


struct ec_params_charger_control {
	int16_t max_current;
	uint16_t otg_voltage;
	uint8_t allow_charging;
} __ec_align_size1;


#define EC_CMD_USB_PD_MUX_ACK 0x0603

struct ec_params_usb_pd_mux_ack {
	uint8_t port; 
} __ec_align1;



#define EC_CMD_BOARD_SPECIFIC_BASE 0x3E00
#define EC_CMD_BOARD_SPECIFIC_LAST 0x3FFF


#define EC_PRIVATE_HOST_COMMAND_VALUE(command) \
	(EC_CMD_BOARD_SPECIFIC_BASE + (command))





#define EC_CMD_PASSTHRU_OFFSET(n) (0x4000 * (n))
#define EC_CMD_PASSTHRU_MAX(n) (EC_CMD_PASSTHRU_OFFSET(n) + 0x3fff)



#define EC_HOST_PARAM_SIZE      EC_PROTO2_MAX_PARAM_SIZE
#define EC_LPC_ADDR_OLD_PARAM   EC_HOST_CMD_REGION1
#define EC_OLD_PARAM_SIZE       EC_HOST_CMD_REGION_SIZE



#endif  
