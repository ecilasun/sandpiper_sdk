/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __PSP_PLATFORM_ACCESS_H
#define __PSP_PLATFORM_ACCESS_H

#include <linux/psp.h>

enum psp_platform_access_msg {
	PSP_CMD_NONE = 0x0,
	PSP_I2C_REQ_BUS_CMD = 0x64,
	PSP_DYNAMIC_BOOST_GET_NONCE,
	PSP_DYNAMIC_BOOST_SET_UID,
	PSP_DYNAMIC_BOOST_GET_PARAMETER,
	PSP_DYNAMIC_BOOST_SET_PARAMETER,
};

struct psp_req_buffer_hdr {
	u32 payload_size;
	u32 status;
} __packed;

struct psp_request {
	struct psp_req_buffer_hdr header;
	void *buf;
} __packed;


int psp_send_platform_access_msg(enum psp_platform_access_msg, struct psp_request *req);


int psp_ring_platform_doorbell(int msg, u32 *result);


int psp_check_platform_access_status(void);

#endif 
