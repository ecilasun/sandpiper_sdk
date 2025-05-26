/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_LOCAL_LOCK_H
#define _LINUX_LOCAL_LOCK_H

#include <linux/local_lock_internal.h>


#define local_lock_init(lock)		__local_lock_init(lock)


#define local_lock(lock)		__local_lock(lock)


#define local_lock_irq(lock)		__local_lock_irq(lock)


#define local_lock_irqsave(lock, flags)				\
	__local_lock_irqsave(lock, flags)


#define local_unlock(lock)		__local_unlock(lock)


#define local_unlock_irq(lock)		__local_unlock_irq(lock)


#define local_unlock_irqrestore(lock, flags)			\
	__local_unlock_irqrestore(lock, flags)

#endif
