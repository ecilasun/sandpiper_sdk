/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _UAPI_LINUX_CROS_EC_DEV_H_
#define _UAPI_LINUX_CROS_EC_DEV_H_

#include <linux/bits.h>
#include <linux/ioctl.h>
#include <linux/types.h>

#include <linux/platform_data/cros_ec_commands.h>

#define CROS_EC_DEV_VERSION "1.0.0"


struct cros_ec_readmem {
	uint32_t offset;
	uint32_t bytes;
	uint8_t buffer[EC_MEMMAP_SIZE];
};

#define CROS_EC_DEV_IOC       0xEC
#define CROS_EC_DEV_IOCXCMD   _IOWR(CROS_EC_DEV_IOC, 0, struct cros_ec_command)
#define CROS_EC_DEV_IOCRDMEM  _IOWR(CROS_EC_DEV_IOC, 1, struct cros_ec_readmem)
#define CROS_EC_DEV_IOCEVENTMASK _IO(CROS_EC_DEV_IOC, 2)

#endif 
