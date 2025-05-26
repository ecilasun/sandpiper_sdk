/* SPDX-License-Identifier: (GPL-2.0 OR CDDL-1.0) */


#ifndef __VBOX_UTILS_H__
#define __VBOX_UTILS_H__

#include <linux/printk.h>
#include <linux/vbox_vmmdev_types.h>

struct vbg_dev;


__printf(1, 2) void vbg_info(const char *fmt, ...);
__printf(1, 2) void vbg_warn(const char *fmt, ...);
__printf(1, 2) void vbg_err(const char *fmt, ...);
__printf(1, 2) void vbg_err_ratelimited(const char *fmt, ...);


#if defined(DEBUG) && !defined(CONFIG_DYNAMIC_DEBUG)
__printf(1, 2) void vbg_debug(const char *fmt, ...);
#else
#define vbg_debug pr_debug
#endif

int vbg_hgcm_connect(struct vbg_dev *gdev, u32 requestor,
		     struct vmmdev_hgcm_service_location *loc,
		     u32 *client_id, int *vbox_status);

int vbg_hgcm_disconnect(struct vbg_dev *gdev, u32 requestor,
			u32 client_id, int *vbox_status);

int vbg_hgcm_call(struct vbg_dev *gdev, u32 requestor, u32 client_id,
		  u32 function, u32 timeout_ms,
		  struct vmmdev_hgcm_function_parameter *parms, u32 parm_count,
		  int *vbox_status);


int vbg_status_code_to_errno(int rc);


struct vbg_dev *vbg_get_gdev(void);


void vbg_put_gdev(struct vbg_dev *gdev);

#endif
