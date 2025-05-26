
#include <linux/semaphore.h>
#include <linux/atomic.h>





#ifndef DRM_TASK_BARRIER_H_
#define DRM_TASK_BARRIER_H_


struct task_barrier {
	unsigned int n;
	atomic_t count;
	struct semaphore enter_turnstile;
	struct semaphore exit_turnstile;
};

static inline void task_barrier_signal_turnstile(struct semaphore *turnstile,
						 unsigned int n)
{
	int i;

	for (i = 0 ; i < n; i++)
		up(turnstile);
}

static inline void task_barrier_init(struct task_barrier *tb)
{
	tb->n = 0;
	atomic_set(&tb->count, 0);
	sema_init(&tb->enter_turnstile, 0);
	sema_init(&tb->exit_turnstile, 0);
}

static inline void task_barrier_add_task(struct task_barrier *tb)
{
	tb->n++;
}

static inline void task_barrier_rem_task(struct task_barrier *tb)
{
	tb->n--;
}


static inline void task_barrier_enter(struct task_barrier *tb)
{
	if (atomic_inc_return(&tb->count) == tb->n)
		task_barrier_signal_turnstile(&tb->enter_turnstile, tb->n);

	down(&tb->enter_turnstile);
}


static inline void task_barrier_exit(struct task_barrier *tb)
{
	if (atomic_dec_return(&tb->count) == 0)
		task_barrier_signal_turnstile(&tb->exit_turnstile, tb->n);

	down(&tb->exit_turnstile);
}


static inline void task_barrier_full(struct task_barrier *tb)
{
	task_barrier_enter(tb);
	task_barrier_exit(tb);
}

#endif
