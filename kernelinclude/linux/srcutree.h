/* SPDX-License-Identifier: GPL-2.0+ */


#ifndef _LINUX_SRCU_TREE_H
#define _LINUX_SRCU_TREE_H

#include <linux/rcu_node_tree.h>
#include <linux/completion.h>

struct srcu_node;
struct srcu_struct;


struct srcu_data {
	
	atomic_long_t srcu_lock_count[2];	
	atomic_long_t srcu_unlock_count[2];	
	int srcu_nmi_safety;			

	
	spinlock_t __private lock ____cacheline_internodealigned_in_smp;
	struct rcu_segcblist srcu_cblist;	
	unsigned long srcu_gp_seq_needed;	
	unsigned long srcu_gp_seq_needed_exp;	
	bool srcu_cblist_invoking;		
	struct timer_list delay_work;		
	struct work_struct work;		
	struct rcu_head srcu_barrier_head;	
	struct srcu_node *mynode;		
	unsigned long grpmask;			
						
	int cpu;
	struct srcu_struct *ssp;
};


struct srcu_node {
	spinlock_t __private lock;
	unsigned long srcu_have_cbs[4];		
						
	unsigned long srcu_data_have_cbs[4];	
	unsigned long srcu_gp_seq_needed_exp;	
	struct srcu_node *srcu_parent;		
	int grplo;				
	int grphi;				
};


struct srcu_usage {
	struct srcu_node *node;			
	struct srcu_node *level[RCU_NUM_LVLS + 1];
						
	int srcu_size_state;			
	struct mutex srcu_cb_mutex;		
	spinlock_t __private lock;		
	struct mutex srcu_gp_mutex;		
	unsigned long srcu_gp_seq;		
	unsigned long srcu_gp_seq_needed;	
	unsigned long srcu_gp_seq_needed_exp;	
	unsigned long srcu_gp_start;		
	unsigned long srcu_last_gp_end;		
	unsigned long srcu_size_jiffies;	
	unsigned long srcu_n_lock_retries;	
	unsigned long srcu_n_exp_nodelay;	
	bool sda_is_static;			
	unsigned long srcu_barrier_seq;		
	struct mutex srcu_barrier_mutex;	
	struct completion srcu_barrier_completion;
						
	atomic_t srcu_barrier_cpu_cnt;		
						
						
	unsigned long reschedule_jiffies;
	unsigned long reschedule_count;
	struct delayed_work work;
	struct srcu_struct *srcu_ssp;
};


struct srcu_struct {
	unsigned int srcu_idx;			
	struct srcu_data __percpu *sda;		
	struct lockdep_map dep_map;
	struct srcu_usage *srcu_sup;		
};

// Values for size state variable (->srcu_size_state).  Once the state
// has been set to SRCU_SIZE_ALLOC, the grace-period code advances through
// this state machine one step per grace period until the SRCU_SIZE_BIG state
// is reached.  Otherwise, the state machine remains in the SRCU_SIZE_SMALL
// state indefinitely.
#define SRCU_SIZE_SMALL		0	// No srcu_node combining tree, ->node == NULL
#define SRCU_SIZE_ALLOC		1	// An srcu_node tree is being allocated, initialized,
					//  and then referenced by ->node.  It will not be used.
#define SRCU_SIZE_WAIT_BARRIER	2	// The srcu_node tree starts being used by everything
					//  except call_srcu(), especially by srcu_barrier().
					//  By the end of this state, all CPUs and threads
					//  are aware of this tree's existence.
#define SRCU_SIZE_WAIT_CALL	3	// The srcu_node tree starts being used by call_srcu().
					//  By the end of this state, all of the call_srcu()
					//  invocations that were running on a non-boot CPU
					//  and using the boot CPU's callback queue will have
					//  completed.
#define SRCU_SIZE_WAIT_CBS1	4	// Don't trust the ->srcu_have_cbs[] grace-period
#define SRCU_SIZE_WAIT_CBS2	5	//  sequence elements or the ->srcu_data_have_cbs[]
#define SRCU_SIZE_WAIT_CBS3	6	//  CPU-bitmask elements until all four elements of
#define SRCU_SIZE_WAIT_CBS4	7	//  each array have been initialized.
#define SRCU_SIZE_BIG		8	// The srcu_node combining tree is fully initialized
					//  and all aspects of it are being put to use.


#define SRCU_STATE_IDLE		0
#define SRCU_STATE_SCAN1	1
#define SRCU_STATE_SCAN2	2

#define __SRCU_USAGE_INIT(name)									\
{												\
	.lock = __SPIN_LOCK_UNLOCKED(name.lock),						\
	.srcu_gp_seq_needed = -1UL,								\
	.work = __DELAYED_WORK_INITIALIZER(name.work, NULL, 0),					\
}

#define __SRCU_STRUCT_INIT_COMMON(name, usage_name)						\
	.srcu_sup = &usage_name,								\
	__SRCU_DEP_MAP_INIT(name)

#define __SRCU_STRUCT_INIT_MODULE(name, usage_name)						\
{												\
	__SRCU_STRUCT_INIT_COMMON(name, usage_name)						\
}

#define __SRCU_STRUCT_INIT(name, usage_name, pcpu_name)						\
{												\
	.sda = &pcpu_name,									\
	__SRCU_STRUCT_INIT_COMMON(name, usage_name)						\
}


#ifdef MODULE
# define __DEFINE_SRCU(name, is_static)								\
	static struct srcu_usage name##_srcu_usage = __SRCU_USAGE_INIT(name##_srcu_usage);	\
	is_static struct srcu_struct name = __SRCU_STRUCT_INIT_MODULE(name, name##_srcu_usage);	\
	extern struct srcu_struct * const __srcu_struct_##name;					\
	struct srcu_struct * const __srcu_struct_##name						\
		__section("___srcu_struct_ptrs") = &name
#else
# define __DEFINE_SRCU(name, is_static)								\
	static DEFINE_PER_CPU(struct srcu_data, name##_srcu_data);				\
	static struct srcu_usage name##_srcu_usage = __SRCU_USAGE_INIT(name##_srcu_usage);	\
	is_static struct srcu_struct name =							\
		__SRCU_STRUCT_INIT(name, name##_srcu_usage, name##_srcu_data)
#endif
#define DEFINE_SRCU(name)		__DEFINE_SRCU(name, )
#define DEFINE_STATIC_SRCU(name)	__DEFINE_SRCU(name, static)

void synchronize_srcu_expedited(struct srcu_struct *ssp);
void srcu_barrier(struct srcu_struct *ssp);
void srcu_torture_stats_print(struct srcu_struct *ssp, char *tt, char *tf);

#endif
