/* SPDX-License-Identifier: GPL-2.0 */
#ifndef __LINUX_ENTRYKVM_H
#define __LINUX_ENTRYKVM_H

#include <linux/static_call_types.h>
#include <linux/resume_user_mode.h>
#include <linux/syscalls.h>
#include <linux/seccomp.h>
#include <linux/sched.h>
#include <linux/tick.h>


#ifdef CONFIG_KVM_XFER_TO_GUEST_WORK

#ifndef ARCH_XFER_TO_GUEST_MODE_WORK
# define ARCH_XFER_TO_GUEST_MODE_WORK	(0)
#endif

#define XFER_TO_GUEST_MODE_WORK						\
	(_TIF_NEED_RESCHED | _TIF_SIGPENDING | _TIF_NOTIFY_SIGNAL |	\
	 _TIF_NOTIFY_RESUME | ARCH_XFER_TO_GUEST_MODE_WORK)

struct kvm_vcpu;


static inline int arch_xfer_to_guest_mode_handle_work(struct kvm_vcpu *vcpu,
						      unsigned long ti_work);

#ifndef arch_xfer_to_guest_mode_work
static inline int arch_xfer_to_guest_mode_handle_work(struct kvm_vcpu *vcpu,
						      unsigned long ti_work)
{
	return 0;
}
#endif


int xfer_to_guest_mode_handle_work(struct kvm_vcpu *vcpu);


static inline void xfer_to_guest_mode_prepare(void)
{
	lockdep_assert_irqs_disabled();
	tick_nohz_user_enter_prepare();
}


static inline bool __xfer_to_guest_mode_work_pending(void)
{
	unsigned long ti_work = read_thread_flags();

	return !!(ti_work & XFER_TO_GUEST_MODE_WORK);
}


static inline bool xfer_to_guest_mode_work_pending(void)
{
	lockdep_assert_irqs_disabled();
	return __xfer_to_guest_mode_work_pending();
}
#endif 

#endif
