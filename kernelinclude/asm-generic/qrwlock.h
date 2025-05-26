/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __ASM_GENERIC_QRWLOCK_H
#define __ASM_GENERIC_QRWLOCK_H

#include <linux/atomic.h>
#include <asm/barrier.h>
#include <asm/processor.h>

#include <asm-generic/qrwlock_types.h>




#define	_QW_WAITING	0x100		
#define	_QW_LOCKED	0x0ff		
#define	_QW_WMASK	0x1ff		
#define	_QR_SHIFT	9		
#define _QR_BIAS	(1U << _QR_SHIFT)


extern void queued_read_lock_slowpath(struct qrwlock *lock);
extern void queued_write_lock_slowpath(struct qrwlock *lock);


static inline int queued_read_trylock(struct qrwlock *lock)
{
	int cnts;

	cnts = atomic_read(&lock->cnts);
	if (likely(!(cnts & _QW_WMASK))) {
		cnts = (u32)atomic_add_return_acquire(_QR_BIAS, &lock->cnts);
		if (likely(!(cnts & _QW_WMASK)))
			return 1;
		atomic_sub(_QR_BIAS, &lock->cnts);
	}
	return 0;
}


static inline int queued_write_trylock(struct qrwlock *lock)
{
	int cnts;

	cnts = atomic_read(&lock->cnts);
	if (unlikely(cnts))
		return 0;

	return likely(atomic_try_cmpxchg_acquire(&lock->cnts, &cnts,
				_QW_LOCKED));
}

static inline void queued_read_lock(struct qrwlock *lock)
{
	int cnts;

	cnts = atomic_add_return_acquire(_QR_BIAS, &lock->cnts);
	if (likely(!(cnts & _QW_WMASK)))
		return;

	
	queued_read_lock_slowpath(lock);
}


static inline void queued_write_lock(struct qrwlock *lock)
{
	int cnts = 0;
	
	if (likely(atomic_try_cmpxchg_acquire(&lock->cnts, &cnts, _QW_LOCKED)))
		return;

	queued_write_lock_slowpath(lock);
}


static inline void queued_read_unlock(struct qrwlock *lock)
{
	
	(void)atomic_sub_return_release(_QR_BIAS, &lock->cnts);
}


static inline void queued_write_unlock(struct qrwlock *lock)
{
	smp_store_release(&lock->wlocked, 0);
}


static inline int queued_rwlock_is_contended(struct qrwlock *lock)
{
	return arch_spin_is_locked(&lock->wait_lock);
}


#define arch_read_lock(l)		queued_read_lock(l)
#define arch_write_lock(l)		queued_write_lock(l)
#define arch_read_trylock(l)		queued_read_trylock(l)
#define arch_write_trylock(l)		queued_write_trylock(l)
#define arch_read_unlock(l)		queued_read_unlock(l)
#define arch_write_unlock(l)		queued_write_unlock(l)
#define arch_rwlock_is_contended(l)	queued_rwlock_is_contended(l)

#endif 
