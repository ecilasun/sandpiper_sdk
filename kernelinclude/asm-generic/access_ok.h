/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __ASM_GENERIC_ACCESS_OK_H__
#define __ASM_GENERIC_ACCESS_OK_H__




#ifndef TASK_SIZE_MAX
#define TASK_SIZE_MAX			TASK_SIZE
#endif

#ifndef __access_ok

static inline int __access_ok(const void __user *ptr, unsigned long size)
{
	unsigned long limit = TASK_SIZE_MAX;
	unsigned long addr = (unsigned long)ptr;

	if (IS_ENABLED(CONFIG_ALTERNATE_USER_ADDRESS_SPACE) ||
	    !IS_ENABLED(CONFIG_MMU))
		return true;

	return (size <= limit) && (addr <= (limit - size));
}
#endif

#ifndef access_ok
#define access_ok(addr, size) likely(__access_ok(addr, size))
#endif

#endif
