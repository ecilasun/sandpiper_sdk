/* SPDX-License-Identifier: MIT */


#ifndef __PSP_TEE_H_
#define __PSP_TEE_H_

#include <linux/types.h>
#include <linux/errno.h>




enum tee_cmd_id {
	TEE_CMD_ID_LOAD_TA = 1,
	TEE_CMD_ID_UNLOAD_TA,
	TEE_CMD_ID_OPEN_SESSION,
	TEE_CMD_ID_CLOSE_SESSION,
	TEE_CMD_ID_INVOKE_CMD,
	TEE_CMD_ID_MAP_SHARED_MEM,
	TEE_CMD_ID_UNMAP_SHARED_MEM,
};

#ifdef CONFIG_CRYPTO_DEV_SP_PSP

int psp_tee_process_cmd(enum tee_cmd_id cmd_id, void *buf, size_t len,
			u32 *status);


int psp_check_tee_status(void);

#else 

static inline int psp_tee_process_cmd(enum tee_cmd_id cmd_id, void *buf,
				      size_t len, u32 *status)
{
	return -ENODEV;
}

static inline int psp_check_tee_status(void)
{
	return -ENODEV;
}
#endif 
#endif 
