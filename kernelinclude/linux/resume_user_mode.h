/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef LINUX_RESUME_USER_MODE_H
#define LINUX_RESUME_USER_MODE_H

#include <linux/sched.h>
#include <linux/task_work.h>
#include <linux/memcontrol.h>
#include <linux/blk-cgroup.h>


static inline void set_notify_resume(struct task_struct *task)
{
	if (!test_and_set_tsk_thread_flag(task, TIF_NOTIFY_RESUME))
		kick_process(task);
}



static inline void resume_user_mode_work(struct pt_regs *regs)
{
	clear_thread_flag(TIF_NOTIFY_RESUME);
	
	smp_mb__after_atomic();
	if (unlikely(task_work_pending(current)))
		task_work_run();

#ifdef CONFIG_KEYS_REQUEST_CACHE
	if (unlikely(current->cached_requested_key)) {
		key_put(current->cached_requested_key);
		current->cached_requested_key = NULL;
	}
#endif

	mem_cgroup_handle_over_high(GFP_KERNEL);
	blkcg_maybe_throttle_current();

	rseq_handle_notify_resume(NULL, regs);
}

#endif 
