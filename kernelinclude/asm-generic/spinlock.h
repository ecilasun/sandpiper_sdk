/* SPDX-License-Identifier: GPL-2.0 */



#ifndef __ASM_GENERIC_SPINLOCK_H
#define __ASM_GENERIC_SPINLOCK_H

#include <linux/atomic.h>
#include <asm-generic/spinlock_types.h>

static __always_inline void arch_spin_lock(arch_spinlock_t *lock)
{
	u32 val = atomic_fetch_add(1<<16, lock);
	u16 ticket = val >> 16;

	if (ticket == (u16)val)
		return;

	
	atomic_cond_read_acquire(lock, ticket == (u16)VAL);
	smp_mb();
}

static __always_inline bool arch_spin_trylock(arch_spinlock_t *lock)
{
	u32 old = atomic_read(lock);

	if ((old >> 16) != (old & 0xffff))
		return false;

	return atomic_try_cmpxchg(lock, &old, old + (1<<16)); 
}

static __always_inline void arch_spin_unlock(arch_spinlock_t *lock)
{
	u16 *ptr = (u16 *)lock + IS_ENABLED(CONFIG_CPU_BIG_ENDIAN);
	u32 val = atomic_read(lock);

	smp_store_release(ptr, (u16)val + 1);
}

static __always_inline int arch_spin_is_locked(arch_spinlock_t *lock)
{
	u32 val = atomic_read(lock);

	return ((val >> 16) != (val & 0xffff));
}

static __always_inline int arch_spin_is_contended(arch_spinlock_t *lock)
{
	u32 val = atomic_read(lock);

	return (s16)((val >> 16) - (val & 0xffff)) > 1;
}

static __always_inline int arch_spin_value_unlocked(arch_spinlock_t lock)
{
	return !arch_spin_is_locked(&lock);
}

#include <asm/qrwlock.h>

#endif 
