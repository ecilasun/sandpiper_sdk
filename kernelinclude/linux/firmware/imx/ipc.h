/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef _SC_IPC_H
#define _SC_IPC_H

#include <linux/device.h>
#include <linux/types.h>

#define IMX_SC_RPC_VERSION	1
#define IMX_SC_RPC_MAX_MSG	8

struct imx_sc_ipc;

enum imx_sc_rpc_svc {
	IMX_SC_RPC_SVC_UNKNOWN = 0,
	IMX_SC_RPC_SVC_RETURN = 1,
	IMX_SC_RPC_SVC_PM = 2,
	IMX_SC_RPC_SVC_RM = 3,
	IMX_SC_RPC_SVC_TIMER = 5,
	IMX_SC_RPC_SVC_PAD = 6,
	IMX_SC_RPC_SVC_MISC = 7,
	IMX_SC_RPC_SVC_IRQ = 8,
};

struct imx_sc_rpc_msg {
	uint8_t ver;
	uint8_t size;
	uint8_t svc;
	uint8_t func;
};

#ifdef CONFIG_IMX_SCU

int imx_scu_call_rpc(struct imx_sc_ipc *ipc, void *msg, bool have_resp);


int imx_scu_get_handle(struct imx_sc_ipc **ipc);
#else
static inline int imx_scu_call_rpc(struct imx_sc_ipc *ipc, void *msg,
				   bool have_resp)
{
	return -ENOTSUPP;
}

static inline int imx_scu_get_handle(struct imx_sc_ipc **ipc)
{
	return -ENOTSUPP;
}
#endif
#endif 
