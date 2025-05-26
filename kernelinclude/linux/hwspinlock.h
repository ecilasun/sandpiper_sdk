/* SPDX-License-Identifier: GPL-2.0 */


#ifndef __LINUX_HWSPINLOCK_H
#define __LINUX_HWSPINLOCK_H

#include <linux/err.h>
#include <linux/sched.h>


#define HWLOCK_IRQSTATE		0x01 
#define HWLOCK_IRQ		0x02 
#define HWLOCK_RAW		0x03
#define HWLOCK_IN_ATOMIC	0x04 

struct device;
struct device_node;
struct hwspinlock;
struct hwspinlock_device;
struct hwspinlock_ops;


struct hwspinlock_pdata {
	int base_id;
};

#ifdef CONFIG_HWSPINLOCK

int hwspin_lock_register(struct hwspinlock_device *bank, struct device *dev,
		const struct hwspinlock_ops *ops, int base_id, int num_locks);
int hwspin_lock_unregister(struct hwspinlock_device *bank);
struct hwspinlock *hwspin_lock_request(void);
struct hwspinlock *hwspin_lock_request_specific(unsigned int id);
int hwspin_lock_free(struct hwspinlock *hwlock);
int of_hwspin_lock_get_id(struct device_node *np, int index);
int hwspin_lock_get_id(struct hwspinlock *hwlock);
int __hwspin_lock_timeout(struct hwspinlock *, unsigned int, int,
							unsigned long *);
int __hwspin_trylock(struct hwspinlock *, int, unsigned long *);
void __hwspin_unlock(struct hwspinlock *, int, unsigned long *);
int of_hwspin_lock_get_id_byname(struct device_node *np, const char *name);
int devm_hwspin_lock_free(struct device *dev, struct hwspinlock *hwlock);
struct hwspinlock *devm_hwspin_lock_request(struct device *dev);
struct hwspinlock *devm_hwspin_lock_request_specific(struct device *dev,
						     unsigned int id);
int devm_hwspin_lock_unregister(struct device *dev,
				struct hwspinlock_device *bank);
int devm_hwspin_lock_register(struct device *dev,
			      struct hwspinlock_device *bank,
			      const struct hwspinlock_ops *ops,
			      int base_id, int num_locks);

#else 


static inline struct hwspinlock *hwspin_lock_request(void)
{
	return ERR_PTR(-ENODEV);
}

static inline struct hwspinlock *hwspin_lock_request_specific(unsigned int id)
{
	return ERR_PTR(-ENODEV);
}

static inline int hwspin_lock_free(struct hwspinlock *hwlock)
{
	return 0;
}

static inline
int __hwspin_lock_timeout(struct hwspinlock *hwlock, unsigned int to,
					int mode, unsigned long *flags)
{
	return 0;
}

static inline
int __hwspin_trylock(struct hwspinlock *hwlock, int mode, unsigned long *flags)
{
	return 0;
}

static inline
void __hwspin_unlock(struct hwspinlock *hwlock, int mode, unsigned long *flags)
{
}

static inline int of_hwspin_lock_get_id(struct device_node *np, int index)
{
	return 0;
}

static inline int hwspin_lock_get_id(struct hwspinlock *hwlock)
{
	return 0;
}

static inline
int of_hwspin_lock_get_id_byname(struct device_node *np, const char *name)
{
	return 0;
}

static inline
int devm_hwspin_lock_free(struct device *dev, struct hwspinlock *hwlock)
{
	return 0;
}

static inline struct hwspinlock *devm_hwspin_lock_request(struct device *dev)
{
	return ERR_PTR(-ENODEV);
}

static inline
struct hwspinlock *devm_hwspin_lock_request_specific(struct device *dev,
						     unsigned int id)
{
	return ERR_PTR(-ENODEV);
}

#endif 


static inline
int hwspin_trylock_irqsave(struct hwspinlock *hwlock, unsigned long *flags)
{
	return __hwspin_trylock(hwlock, HWLOCK_IRQSTATE, flags);
}


static inline int hwspin_trylock_irq(struct hwspinlock *hwlock)
{
	return __hwspin_trylock(hwlock, HWLOCK_IRQ, NULL);
}


static inline int hwspin_trylock_raw(struct hwspinlock *hwlock)
{
	return __hwspin_trylock(hwlock, HWLOCK_RAW, NULL);
}


static inline int hwspin_trylock_in_atomic(struct hwspinlock *hwlock)
{
	return __hwspin_trylock(hwlock, HWLOCK_IN_ATOMIC, NULL);
}


static inline int hwspin_trylock(struct hwspinlock *hwlock)
{
	return __hwspin_trylock(hwlock, 0, NULL);
}


static inline int hwspin_lock_timeout_irqsave(struct hwspinlock *hwlock,
				unsigned int to, unsigned long *flags)
{
	return __hwspin_lock_timeout(hwlock, to, HWLOCK_IRQSTATE, flags);
}


static inline
int hwspin_lock_timeout_irq(struct hwspinlock *hwlock, unsigned int to)
{
	return __hwspin_lock_timeout(hwlock, to, HWLOCK_IRQ, NULL);
}


static inline
int hwspin_lock_timeout_raw(struct hwspinlock *hwlock, unsigned int to)
{
	return __hwspin_lock_timeout(hwlock, to, HWLOCK_RAW, NULL);
}


static inline
int hwspin_lock_timeout_in_atomic(struct hwspinlock *hwlock, unsigned int to)
{
	return __hwspin_lock_timeout(hwlock, to, HWLOCK_IN_ATOMIC, NULL);
}


static inline
int hwspin_lock_timeout(struct hwspinlock *hwlock, unsigned int to)
{
	return __hwspin_lock_timeout(hwlock, to, 0, NULL);
}


static inline void hwspin_unlock_irqrestore(struct hwspinlock *hwlock,
							unsigned long *flags)
{
	__hwspin_unlock(hwlock, HWLOCK_IRQSTATE, flags);
}


static inline void hwspin_unlock_irq(struct hwspinlock *hwlock)
{
	__hwspin_unlock(hwlock, HWLOCK_IRQ, NULL);
}


static inline void hwspin_unlock_raw(struct hwspinlock *hwlock)
{
	__hwspin_unlock(hwlock, HWLOCK_RAW, NULL);
}


static inline void hwspin_unlock_in_atomic(struct hwspinlock *hwlock)
{
	__hwspin_unlock(hwlock, HWLOCK_IN_ATOMIC, NULL);
}


static inline void hwspin_unlock(struct hwspinlock *hwlock)
{
	__hwspin_unlock(hwlock, 0, NULL);
}

#endif 
