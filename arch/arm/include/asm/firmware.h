/* SPDX-License-Identifier: GPL-2.0-only */


#ifndef __ASM_ARM_FIRMWARE_H
#define __ASM_ARM_FIRMWARE_H

#include <linux/bug.h>


struct firmware_ops {
	
	int (*prepare_idle)(unsigned long mode);
	
	int (*do_idle)(unsigned long mode);
	
	int (*set_cpu_boot_addr)(int cpu, unsigned long boot_addr);
	
	int (*get_cpu_boot_addr)(int cpu, unsigned long *boot_addr);
	
	int (*cpu_boot)(int cpu);
	
	int (*l2x0_init)(void);
	
	int (*suspend)(void);
	
	int (*resume)(void);
};


extern const struct firmware_ops *firmware_ops;


#define call_firmware_op(op, ...)					\
	((firmware_ops->op) ? firmware_ops->op(__VA_ARGS__) : (-ENOSYS))


static inline void register_firmware_ops(const struct firmware_ops *ops)
{
	BUG_ON(!ops);

	firmware_ops = ops;
}

#endif
