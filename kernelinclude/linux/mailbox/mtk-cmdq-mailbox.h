/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __MTK_CMDQ_MAILBOX_H__
#define __MTK_CMDQ_MAILBOX_H__

#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/types.h>

#define CMDQ_INST_SIZE			8 
#define CMDQ_SUBSYS_SHIFT		16
#define CMDQ_OP_CODE_SHIFT		24
#define CMDQ_JUMP_PASS			CMDQ_INST_SIZE

#define CMDQ_WFE_UPDATE			BIT(31)
#define CMDQ_WFE_UPDATE_VALUE		BIT(16)
#define CMDQ_WFE_WAIT			BIT(15)
#define CMDQ_WFE_WAIT_VALUE		0x1


#define CMDQ_WFE_OPTION			(CMDQ_WFE_WAIT | CMDQ_WFE_WAIT_VALUE)


#define CMDQ_MAX_EVENT			0x3ff


enum cmdq_code {
	CMDQ_CODE_MASK = 0x02,
	CMDQ_CODE_WRITE = 0x04,
	CMDQ_CODE_POLL = 0x08,
	CMDQ_CODE_JUMP = 0x10,
	CMDQ_CODE_WFE = 0x20,
	CMDQ_CODE_EOC = 0x40,
	CMDQ_CODE_READ_S = 0x80,
	CMDQ_CODE_WRITE_S = 0x90,
	CMDQ_CODE_WRITE_S_MASK = 0x91,
	CMDQ_CODE_LOGIC = 0xa0,
};

struct cmdq_cb_data {
	int			sta;
	struct cmdq_pkt		*pkt;
};

struct cmdq_pkt {
	void			*va_base;
	dma_addr_t		pa_base;
	size_t			cmd_buf_size; 
	size_t			buf_size; 
	void			*cl;
};

u8 cmdq_get_shift_pa(struct mbox_chan *chan);

#endif 
