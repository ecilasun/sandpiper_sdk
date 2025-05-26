/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef __LINUX_RCUTREE_H
#define __LINUX_RCUTREE_H

void rcu_softirq_qs(void);
void rcu_note_context_switch(bool preempt);
int rcu_needs_cpu(void);
void rcu_cpu_stall_reset(void);
void rcu_request_urgent_qs_task(struct task_struct *t);


static inline void rcu_virt_note_context_switch(void)
{
	rcu_note_context_switch(false);
}

void synchronize_rcu_expedited(void);
void kvfree_call_rcu(struct rcu_head *head, void *ptr);

void rcu_barrier(void);
bool rcu_eqs_special_set(int cpu);
void rcu_momentary_dyntick_idle(void);
void kfree_rcu_scheduler_running(void);
bool rcu_gp_might_be_stalled(void);

struct rcu_gp_oldstate {
	unsigned long rgos_norm;
	unsigned long rgos_exp;
};

// Maximum number of rcu_gp_oldstate values corresponding to
// not-yet-completed RCU grace periods.
#define NUM_ACTIVE_RCU_POLL_FULL_OLDSTATE 4


static inline bool same_state_synchronize_rcu_full(struct rcu_gp_oldstate *rgosp1,
						   struct rcu_gp_oldstate *rgosp2)
{
	return rgosp1->rgos_norm == rgosp2->rgos_norm && rgosp1->rgos_exp == rgosp2->rgos_exp;
}

unsigned long start_poll_synchronize_rcu_expedited(void);
void start_poll_synchronize_rcu_expedited_full(struct rcu_gp_oldstate *rgosp);
void cond_synchronize_rcu_expedited(unsigned long oldstate);
void cond_synchronize_rcu_expedited_full(struct rcu_gp_oldstate *rgosp);
unsigned long get_state_synchronize_rcu(void);
void get_state_synchronize_rcu_full(struct rcu_gp_oldstate *rgosp);
unsigned long start_poll_synchronize_rcu(void);
void start_poll_synchronize_rcu_full(struct rcu_gp_oldstate *rgosp);
bool poll_state_synchronize_rcu(unsigned long oldstate);
bool poll_state_synchronize_rcu_full(struct rcu_gp_oldstate *rgosp);
void cond_synchronize_rcu(unsigned long oldstate);
void cond_synchronize_rcu_full(struct rcu_gp_oldstate *rgosp);

#ifdef CONFIG_PROVE_RCU
void rcu_irq_exit_check_preempt(void);
#else
static inline void rcu_irq_exit_check_preempt(void) { }
#endif

struct task_struct;
void rcu_preempt_deferred_qs(struct task_struct *t);

void exit_rcu(void);

void rcu_scheduler_starting(void);
extern int rcu_scheduler_active;
void rcu_end_inkernel_boot(void);
bool rcu_inkernel_boot_has_ended(void);
bool rcu_is_watching(void);
#ifndef CONFIG_PREEMPTION
void rcu_all_qs(void);
#endif


int rcutree_prepare_cpu(unsigned int cpu);
int rcutree_online_cpu(unsigned int cpu);
int rcutree_offline_cpu(unsigned int cpu);
int rcutree_dead_cpu(unsigned int cpu);
int rcutree_dying_cpu(unsigned int cpu);
void rcu_cpu_starting(unsigned int cpu);

#endif 
