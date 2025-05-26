/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _ASM_GENERIC_FUTEX_H
#define _ASM_GENERIC_FUTEX_H

#include <linux/futex.h>
#include <linux/uaccess.h>
#include <asm/errno.h>

#ifndef futex_atomic_cmpxchg_inatomic
#ifndef CONFIG_SMP

#define futex_atomic_cmpxchg_inatomic(uval, uaddr, oldval, newval) \
	futex_atomic_cmpxchg_inatomic_local(uval, uaddr, oldval, newval)
#define arch_futex_atomic_op_inuser(op, oparg, oval, uaddr) \
	futex_atomic_op_inuser_local(op, oparg, oval, uaddr)
#endif 
#endif


static inline int
futex_atomic_op_inuser_local(int op, u32 oparg, int *oval, u32 __user *uaddr)
{
	int oldval, ret;
	u32 tmp;

	preempt_disable();

	ret = -EFAULT;
	if (unlikely(get_user(oldval, uaddr) != 0))
		goto out_pagefault_enable;

	ret = 0;
	tmp = oldval;

	switch (op) {
	case FUTEX_OP_SET:
		tmp = oparg;
		break;
	case FUTEX_OP_ADD:
		tmp += oparg;
		break;
	case FUTEX_OP_OR:
		tmp |= oparg;
		break;
	case FUTEX_OP_ANDN:
		tmp &= ~oparg;
		break;
	case FUTEX_OP_XOR:
		tmp ^= oparg;
		break;
	default:
		ret = -ENOSYS;
	}

	if (ret == 0 && unlikely(put_user(tmp, uaddr) != 0))
		ret = -EFAULT;

out_pagefault_enable:
	preempt_enable();

	if (ret == 0)
		*oval = oldval;

	return ret;
}


static inline int
futex_atomic_cmpxchg_inatomic_local(u32 *uval, u32 __user *uaddr,
			      u32 oldval, u32 newval)
{
	u32 val;

	preempt_disable();
	if (unlikely(get_user(val, uaddr) != 0)) {
		preempt_enable();
		return -EFAULT;
	}

	if (val == oldval && unlikely(put_user(newval, uaddr) != 0)) {
		preempt_enable();
		return -EFAULT;
	}

	*uval = val;
	preempt_enable();

	return 0;
}

#endif
