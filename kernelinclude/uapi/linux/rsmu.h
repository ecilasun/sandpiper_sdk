/* SPDX-License-Identifier: GPL-2.0+ WITH Linux-syscall-note */


#ifndef __UAPI_LINUX_RSMU_CDEV_H
#define __UAPI_LINUX_RSMU_CDEV_H

#include <linux/types.h>
#include <linux/ioctl.h>

#define MAX_NUM_PRIORITY_ENTRIES 32
#define TDC_FIFO_SIZE 16


struct rsmu_combomode {
	__u8 dpll;
	__u8 mode;
};


struct rsmu_get_state {
	__u8 dpll;
	__u8 state;
};


struct rsmu_get_ffo {
	__u8 dpll;
	__s64 ffo;
};


struct rsmu_holdover_mode {
	__u8 dpll;
	__u8 enable;
	__u8 mode;
};


struct rsmu_set_output_tdc_go {
	__u8 tdc;
	__u8 enable;
};


struct rsmu_reg_rw {
	__u32 offset;
	__u8 byte_count;
	__u8 bytes[256];
};


struct rsmu_current_clock_index {
	__u8 dpll;
	__s8 clock_index;
};

struct rsmu_priority_entry {
	__u8 clock_index;
	__u8 priority;
};


struct rsmu_clock_priorities {
	__u8 dpll;
	__u8 num_entries;
	struct rsmu_priority_entry priority_entry[MAX_NUM_PRIORITY_ENTRIES];
};

struct rsmu_reference_monitor_status_alarms {
	__u8 los;
	__u8 no_activity;
	__u8 frequency_offset_limit;
};


struct rsmu_reference_monitor_status {
	__u8 clock_index;
	struct rsmu_reference_monitor_status_alarms alarms;
};


struct rsmu_get_tdc_meas {
	bool continuous;
	__s64 offset;
};


#define RSMU_MAGIC '?'


#define RSMU_SET_COMBOMODE  _IOW(RSMU_MAGIC, 1, struct rsmu_combomode)


#define RSMU_GET_STATE  _IOR(RSMU_MAGIC, 2, struct rsmu_get_state)


#define RSMU_GET_FFO  _IOR(RSMU_MAGIC, 3, struct rsmu_get_ffo)


#define RSMU_SET_HOLDOVER_MODE  _IOW(RSMU_MAGIC, 4, struct rsmu_holdover_mode)


#define RSMU_SET_OUTPUT_TDC_GO  _IOW(RSMU_MAGIC, 5, struct rsmu_set_output_tdc_go)


#define RSMU_GET_CURRENT_CLOCK_INDEX  _IOR(RSMU_MAGIC, 6, struct rsmu_current_clock_index)


#define RSMU_SET_CLOCK_PRIORITIES  _IOW(RSMU_MAGIC, 7, struct rsmu_clock_priorities)


#define RSMU_GET_REFERENCE_MONITOR_STATUS  _IOR(RSMU_MAGIC, 8, struct rsmu_reference_monitor_status)


#define RSMU_GET_TDC_MEAS  _IOR(RSMU_MAGIC, 9, struct rsmu_get_tdc_meas)

#define RSMU_REG_READ   _IOR(RSMU_MAGIC, 100, struct rsmu_reg_rw)
#define RSMU_REG_WRITE  _IOR(RSMU_MAGIC, 101, struct rsmu_reg_rw)
#endif 
