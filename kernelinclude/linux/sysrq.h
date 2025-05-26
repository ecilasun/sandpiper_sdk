/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_SYSRQ_H
#define _LINUX_SYSRQ_H

#include <linux/errno.h>
#include <linux/types.h>



#define SYSRQ_ENABLE_LOG	0x0002
#define SYSRQ_ENABLE_KEYBOARD	0x0004
#define SYSRQ_ENABLE_DUMP	0x0008
#define SYSRQ_ENABLE_SYNC	0x0010
#define SYSRQ_ENABLE_REMOUNT	0x0020
#define SYSRQ_ENABLE_SIGNAL	0x0040
#define SYSRQ_ENABLE_BOOT	0x0080
#define SYSRQ_ENABLE_RTNICE	0x0100

struct sysrq_key_op {
	void (* const handler)(u8);
	const char * const help_msg;
	const char * const action_msg;
	const int enable_mask;
};

#ifdef CONFIG_MAGIC_SYSRQ



void handle_sysrq(u8 key);
void __handle_sysrq(u8 key, bool check_mask);
int register_sysrq_key(u8 key, const struct sysrq_key_op *op);
int unregister_sysrq_key(u8 key, const struct sysrq_key_op *op);
extern const struct sysrq_key_op *__sysrq_reboot_op;

int sysrq_toggle_support(int enable_mask);
int sysrq_mask(void);

#else

static inline void handle_sysrq(u8 key)
{
}

static inline void __handle_sysrq(u8 key, bool check_mask)
{
}

static inline int register_sysrq_key(u8 key, const struct sysrq_key_op *op)
{
	return -EINVAL;
}

static inline int unregister_sysrq_key(u8 key, const struct sysrq_key_op *op)
{
	return -EINVAL;
}

static inline int sysrq_mask(void)
{
	
	return 0;
}

#endif

#endif 
