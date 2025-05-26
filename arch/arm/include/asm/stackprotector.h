/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _ASM_STACKPROTECTOR_H
#define _ASM_STACKPROTECTOR_H 1

#include <asm/thread_info.h>

extern unsigned long __stack_chk_guard;


static __always_inline void boot_init_stack_canary(void)
{
	unsigned long canary = get_random_canary();

	current->stack_canary = canary;
#ifndef CONFIG_STACKPROTECTOR_PER_TASK
	__stack_chk_guard = current->stack_canary;
#endif
}

#endif	
