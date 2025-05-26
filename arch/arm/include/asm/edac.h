/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef ASM_EDAC_H
#define ASM_EDAC_H


static inline void edac_atomic_scrub(void *va, u32 size)
{
#if __LINUX_ARM_ARCH__ >= 6
	unsigned int *virt_addr = va;
	unsigned int temp, temp2;
	unsigned int i;

	for (i = 0; i < size / sizeof(*virt_addr); i++, virt_addr++) {
		
		__asm__ __volatile__("\n"
			"1:	ldrex	%0, [%2]\n"
			"	strex	%1, %0, [%2]\n"
			"	teq	%1, #0\n"
			"	bne	1b\n"
			: "=&r"(temp), "=&r"(temp2)
			: "r"(virt_addr)
			: "cc");
	}
#endif
}

#endif
