/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef TI_MSGMGR_H
#define TI_MSGMGR_H

struct mbox_chan;


struct ti_msgmgr_message {
	size_t len;
	u8 *buf;
	struct mbox_chan *chan_rx;
	int timeout_rx_ms;
};

#endif 
