/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_MSG_H
#define _LINUX_MSG_H

#include <linux/list.h>
#include <uapi/linux/msg.h>


struct msg_msg {
	struct list_head m_list;
	long m_type;
	size_t m_ts;		
	struct msg_msgseg *next;
	void *security;
	
};

#endif 
