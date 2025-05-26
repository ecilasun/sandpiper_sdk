/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __ASM_GENERIC_CMPXCHG_H
#define __ASM_GENERIC_CMPXCHG_H

#ifdef CONFIG_SMP
#error "Cannot use generic cmpxchg on SMP"
#endif

#include <linux/types.h>
#include <linux/irqflags.h>


extern void __generic_xchg_called_with_bad_pointer(void);

static inline
unsigned long __generic_xchg(unsigned long x, volatile void *ptr, int size)
{
	unsigned long ret, flags;

	switch (size) {
	case 1:
#ifdef __xchg_u8
		return __xchg_u8(x, ptr);
#else
		local_irq_save(flags);
		ret = *(volatile u8 *)ptr;
		*(volatile u8 *)ptr = (x & 0xffu);
		local_irq_restore(flags);
		return ret;
#endif 

	case 2:
#ifdef __xchg_u16
		return __xchg_u16(x, ptr);
#else
		local_irq_save(flags);
		ret = *(volatile u16 *)ptr;
		*(volatile u16 *)ptr = (x & 0xffffu);
		local_irq_restore(flags);
		return ret;
#endif 

	case 4:
#ifdef __xchg_u32
		return __xchg_u32(x, ptr);
#else
		local_irq_save(flags);
		ret = *(volatile u32 *)ptr;
		*(volatile u32 *)ptr = (x & 0xffffffffu);
		local_irq_restore(flags);
		return ret;
#endif 

#ifdef CONFIG_64BIT
	case 8:
#ifdef __xchg_u64
		return __xchg_u64(x, ptr);
#else
		local_irq_save(flags);
		ret = *(volatile u64 *)ptr;
		*(volatile u64 *)ptr = x;
		local_irq_restore(flags);
		return ret;
#endif 
#endif 

	default:
		__generic_xchg_called_with_bad_pointer();
		return x;
	}
}

#define generic_xchg(ptr, x) ({							\
	((__typeof__(*(ptr)))							\
		__generic_xchg((unsigned long)(x), (ptr), sizeof(*(ptr))));	\
})


#include <asm-generic/cmpxchg-local.h>

#define generic_cmpxchg_local(ptr, o, n) ({					\
	((__typeof__(*(ptr)))__generic_cmpxchg_local((ptr), (unsigned long)(o),	\
			(unsigned long)(n), sizeof(*(ptr))));			\
})

#define generic_cmpxchg64_local(ptr, o, n) \
	__generic_cmpxchg64_local((ptr), (o), (n))


#ifndef arch_xchg
#define arch_xchg		generic_xchg
#endif

#ifndef arch_cmpxchg_local
#define arch_cmpxchg_local	generic_cmpxchg_local
#endif

#ifndef arch_cmpxchg64_local
#define arch_cmpxchg64_local	generic_cmpxchg64_local
#endif

#define arch_cmpxchg		arch_cmpxchg_local
#define arch_cmpxchg64		arch_cmpxchg64_local

#endif 
