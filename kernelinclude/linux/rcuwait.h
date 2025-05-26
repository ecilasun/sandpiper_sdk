/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_RCUWAIT_H_
#define _LINUX_RCUWAIT_H_

#include <linux/rcupdate.h>
#include <linux/sched/signal.h>


struct rcuwait {
	struct task_struct __rcu *task;
};

#define __RCUWAIT_INITIALIZER(name)		\
	{ .task = NULL, }

static inline void rcuwait_init(struct rcuwait *w)
{
	w->task = NULL;
}


static inline int rcuwait_active(struct rcuwait *w)
{
	return !!rcu_access_pointer(w->task);
}

extern int rcuwait_wake_up(struct rcuwait *w);



static inline void prepare_to_rcuwait(struct rcuwait *w)
{
	rcu_assign_pointer(w->task, current);
}

extern void finish_rcuwait(struct rcuwait *w);

#define ___rcuwait_wait_event(w, condition, state, ret, cmd)		\
({									\
	long __ret = ret;						\
	prepare_to_rcuwait(w);						\
	for (;;) {							\
									\
		set_current_state(state);				\
		if (condition)						\
			break;						\
									\
		if (signal_pending_state(state, current)) {		\
			__ret = -EINTR;					\
			break;						\
		}							\
									\
		cmd;							\
	}								\
	finish_rcuwait(w);						\
	__ret;								\
})

#define rcuwait_wait_event(w, condition, state)				\
	___rcuwait_wait_event(w, condition, state, 0, schedule())

#define __rcuwait_wait_event_timeout(w, condition, state, timeout)	\
	___rcuwait_wait_event(w, ___wait_cond_timeout(condition),	\
			      state, timeout,				\
			      __ret = schedule_timeout(__ret))

#define rcuwait_wait_event_timeout(w, condition, state, timeout)	\
({									\
	long __ret = timeout;						\
	if (!___wait_cond_timeout(condition))				\
		__ret = __rcuwait_wait_event_timeout(w, condition,	\
						     state, timeout);	\
	__ret;								\
})

#endif 
