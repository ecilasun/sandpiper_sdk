
#ifndef _LINUX_PERF_EVENT_H
#define _LINUX_PERF_EVENT_H

#include <uapi/linux/perf_event.h>
#include <uapi/linux/bpf_perf_event.h>



#ifdef CONFIG_PERF_EVENTS
# include <asm/perf_event.h>
# include <asm/local64.h>
#endif

#define PERF_GUEST_ACTIVE	0x01
#define PERF_GUEST_USER	0x02

struct perf_guest_info_callbacks {
	unsigned int			(*state)(void);
	unsigned long			(*get_ip)(void);
	unsigned int			(*handle_intel_pt_intr)(void);
};

#ifdef CONFIG_HAVE_HW_BREAKPOINT
#include <linux/rhashtable-types.h>
#include <asm/hw_breakpoint.h>
#endif

#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/rculist.h>
#include <linux/rcupdate.h>
#include <linux/spinlock.h>
#include <linux/hrtimer.h>
#include <linux/fs.h>
#include <linux/pid_namespace.h>
#include <linux/workqueue.h>
#include <linux/ftrace.h>
#include <linux/cpu.h>
#include <linux/irq_work.h>
#include <linux/static_key.h>
#include <linux/jump_label_ratelimit.h>
#include <linux/atomic.h>
#include <linux/sysfs.h>
#include <linux/perf_regs.h>
#include <linux/cgroup.h>
#include <linux/refcount.h>
#include <linux/security.h>
#include <linux/static_call.h>
#include <linux/lockdep.h>
#include <asm/local.h>

struct perf_callchain_entry {
	__u64				nr;
	__u64				ip[]; 
};

struct perf_callchain_entry_ctx {
	struct perf_callchain_entry *entry;
	u32			    max_stack;
	u32			    nr;
	short			    contexts;
	bool			    contexts_maxed;
};

typedef unsigned long (*perf_copy_f)(void *dst, const void *src,
				     unsigned long off, unsigned long len);

struct perf_raw_frag {
	union {
		struct perf_raw_frag	*next;
		unsigned long		pad;
	};
	perf_copy_f			copy;
	void				*data;
	u32				size;
} __packed;

struct perf_raw_record {
	struct perf_raw_frag		frag;
	u32				size;
};

static __always_inline bool perf_raw_frag_last(const struct perf_raw_frag *frag)
{
	return frag->pad < sizeof(u64);
}


struct perf_branch_stack {
	__u64				nr;
	__u64				hw_idx;
	struct perf_branch_entry	entries[];
};

struct task_struct;


struct hw_perf_event_extra {
	u64		config;	
	unsigned int	reg;	
	int		alloc;	
	int		idx;	
};


#define PERF_EVENT_FLAG_ARCH			0x000fffff
#define PERF_EVENT_FLAG_USER_READ_CNT		0x80000000

static_assert((PERF_EVENT_FLAG_USER_READ_CNT & PERF_EVENT_FLAG_ARCH) == 0);


struct hw_perf_event {
#ifdef CONFIG_PERF_EVENTS
	union {
		struct { 
			u64		config;
			u64		last_tag;
			unsigned long	config_base;
			unsigned long	event_base;
			int		event_base_rdpmc;
			int		idx;
			int		last_cpu;
			int		flags;

			struct hw_perf_event_extra extra_reg;
			struct hw_perf_event_extra branch_reg;
		};
		struct { 
			struct hrtimer	hrtimer;
		};
		struct { 
			
			struct list_head	tp_list;
		};
		struct { 
			u64	pwr_acc;
			u64	ptsc;
		};
#ifdef CONFIG_HAVE_HW_BREAKPOINT
		struct { 
			
			struct arch_hw_breakpoint	info;
			struct rhlist_head		bp_list;
		};
#endif
		struct { 
			u8	iommu_bank;
			u8	iommu_cntr;
			u16	padding;
			u64	conf;
			u64	conf1;
		};
	};
	
	struct task_struct		*target;

	
	void				*addr_filters;

	
	unsigned long			addr_filters_gen;


#define PERF_HES_STOPPED	0x01 
#define PERF_HES_UPTODATE	0x02 
#define PERF_HES_ARCH		0x04

	int				state;

	
	local64_t			prev_count;

	
	u64				sample_period;

	union {
		struct { 
			
			u64				last_period;

			
			local64_t			period_left;
		};
		struct { 
			u64				saved_metric;
			u64				saved_slots;
		};
	};

	
	u64                             interrupts_seq;
	u64				interrupts;

	
	u64				freq_time_stamp;
	u64				freq_count_stamp;
#endif
};

struct perf_event;
struct perf_event_pmu_context;


#define PERF_PMU_TXN_ADD  0x1		
#define PERF_PMU_TXN_READ 0x2		


#define PERF_PMU_CAP_NO_INTERRUPT		0x0001
#define PERF_PMU_CAP_NO_NMI			0x0002
#define PERF_PMU_CAP_AUX_NO_SG			0x0004
#define PERF_PMU_CAP_EXTENDED_REGS		0x0008
#define PERF_PMU_CAP_EXCLUSIVE			0x0010
#define PERF_PMU_CAP_ITRACE			0x0020
#define PERF_PMU_CAP_NO_EXCLUDE			0x0040
#define PERF_PMU_CAP_AUX_OUTPUT			0x0080
#define PERF_PMU_CAP_EXTENDED_HW_TYPE		0x0100

struct perf_output_handle;

#define PMU_NULL_DEV	((void *)(~0UL))


struct pmu {
	struct list_head		entry;

	struct module			*module;
	struct device			*dev;
	struct device			*parent;
	const struct attribute_group	**attr_groups;
	const struct attribute_group	**attr_update;
	const char			*name;
	int				type;

	
	int				capabilities;

	int __percpu			*pmu_disable_count;
	struct perf_cpu_pmu_context __percpu *cpu_pmu_context;
	atomic_t			exclusive_cnt; 
	int				task_ctx_nr;
	int				hrtimer_interval_ms;

	
	unsigned int			nr_addr_filters;

	
	void (*pmu_enable)		(struct pmu *pmu); 
	void (*pmu_disable)		(struct pmu *pmu); 

	
	int (*event_init)		(struct perf_event *event);

	
	void (*event_mapped)		(struct perf_event *event, struct mm_struct *mm); 
	void (*event_unmapped)		(struct perf_event *event, struct mm_struct *mm); 

	
#define PERF_EF_START	0x01		
#define PERF_EF_RELOAD	0x02		
#define PERF_EF_UPDATE	0x04		

	
	int  (*add)			(struct perf_event *event, int flags);
	void (*del)			(struct perf_event *event, int flags);

	
	void (*start)			(struct perf_event *event, int flags);
	void (*stop)			(struct perf_event *event, int flags);

	
	void (*read)			(struct perf_event *event);

	
	void (*start_txn)		(struct pmu *pmu, unsigned int txn_flags);
	
	int  (*commit_txn)		(struct pmu *pmu);
	
	void (*cancel_txn)		(struct pmu *pmu);

	
	int (*event_idx)		(struct perf_event *event); 

	
	void (*sched_task)		(struct perf_event_pmu_context *pmu_ctx,
					bool sched_in);

	
	struct kmem_cache		*task_ctx_cache;

	
	void (*swap_task_ctx)		(struct perf_event_pmu_context *prev_epc,
					 struct perf_event_pmu_context *next_epc);
					

	
	void *(*setup_aux)		(struct perf_event *event, void **pages,
					 int nr_pages, bool overwrite);
					

	
	void (*free_aux)		(void *aux); 

	
	long (*snapshot_aux)		(struct perf_event *event,
					 struct perf_output_handle *handle,
					 unsigned long size);

	
	int (*addr_filters_validate)	(struct list_head *filters);
					

	
	void (*addr_filters_sync)	(struct perf_event *event);
					

	
	int (*aux_output_match)		(struct perf_event *event);
					

	
	bool (*filter)			(struct pmu *pmu, int cpu); 

	
	int (*check_period)		(struct perf_event *event, u64 value); 
};

enum perf_addr_filter_action_t {
	PERF_ADDR_FILTER_ACTION_STOP = 0,
	PERF_ADDR_FILTER_ACTION_START,
	PERF_ADDR_FILTER_ACTION_FILTER,
};


struct perf_addr_filter {
	struct list_head	entry;
	struct path		path;
	unsigned long		offset;
	unsigned long		size;
	enum perf_addr_filter_action_t	action;
};


struct perf_addr_filters_head {
	struct list_head	list;
	raw_spinlock_t		lock;
	unsigned int		nr_file_filters;
};

struct perf_addr_filter_range {
	unsigned long		start;
	unsigned long		size;
};


enum perf_event_state {
	PERF_EVENT_STATE_DEAD		= -4,
	PERF_EVENT_STATE_EXIT		= -3,
	PERF_EVENT_STATE_ERROR		= -2,
	PERF_EVENT_STATE_OFF		= -1,
	PERF_EVENT_STATE_INACTIVE	=  0,
	PERF_EVENT_STATE_ACTIVE		=  1,
};

struct file;
struct perf_sample_data;

typedef void (*perf_overflow_handler_t)(struct perf_event *,
					struct perf_sample_data *,
					struct pt_regs *regs);


#define PERF_EV_CAP_SOFTWARE		BIT(0)
#define PERF_EV_CAP_READ_ACTIVE_PKG	BIT(1)
#define PERF_EV_CAP_SIBLING		BIT(2)

#define SWEVENT_HLIST_BITS		8
#define SWEVENT_HLIST_SIZE		(1 << SWEVENT_HLIST_BITS)

struct swevent_hlist {
	struct hlist_head		heads[SWEVENT_HLIST_SIZE];
	struct rcu_head			rcu_head;
};

#define PERF_ATTACH_CONTEXT	0x01
#define PERF_ATTACH_GROUP	0x02
#define PERF_ATTACH_TASK	0x04
#define PERF_ATTACH_TASK_DATA	0x08
#define PERF_ATTACH_ITRACE	0x10
#define PERF_ATTACH_SCHED_CB	0x20
#define PERF_ATTACH_CHILD	0x40

struct bpf_prog;
struct perf_cgroup;
struct perf_buffer;

struct pmu_event_list {
	raw_spinlock_t		lock;
	struct list_head	list;
};


#ifdef CONFIG_PROVE_LOCKING
#define lockdep_assert_event_ctx(event)				\
	WARN_ON_ONCE(__lockdep_enabled &&			\
		     (this_cpu_read(hardirqs_enabled) &&	\
		      lockdep_is_held(&(event)->ctx->mutex) != LOCK_STATE_HELD))
#else
#define lockdep_assert_event_ctx(event)
#endif

#define for_each_sibling_event(sibling, event)			\
	lockdep_assert_event_ctx(event);			\
	if ((event)->group_leader == (event))			\
		list_for_each_entry((sibling), &(event)->sibling_list, sibling_list)


struct perf_event {
#ifdef CONFIG_PERF_EVENTS
	
	struct list_head		event_entry;

	
	struct list_head		sibling_list;
	struct list_head		active_list;
	
	struct rb_node			group_node;
	u64				group_index;
	
	struct list_head		migrate_entry;

	struct hlist_node		hlist_entry;
	struct list_head		active_entry;
	int				nr_siblings;

	
	int				event_caps;
	
	int				group_caps;

	unsigned int			group_generation;
	struct perf_event		*group_leader;
	
	struct pmu			*pmu;
	void				*pmu_private;

	enum perf_event_state		state;
	unsigned int			attach_state;
	local64_t			count;
	atomic64_t			child_count;

	
	u64				total_time_enabled;
	u64				total_time_running;
	u64				tstamp;

	struct perf_event_attr		attr;
	u16				header_size;
	u16				id_header_size;
	u16				read_size;
	struct hw_perf_event		hw;

	struct perf_event_context	*ctx;
	
	struct perf_event_pmu_context	*pmu_ctx;
	atomic_long_t			refcount;

	
	atomic64_t			child_total_time_enabled;
	atomic64_t			child_total_time_running;

	
	struct mutex			child_mutex;
	struct list_head		child_list;
	struct perf_event		*parent;

	int				oncpu;
	int				cpu;

	struct list_head		owner_entry;
	struct task_struct		*owner;

	
	struct mutex			mmap_mutex;
	atomic_t			mmap_count;

	struct perf_buffer		*rb;
	struct list_head		rb_entry;
	unsigned long			rcu_batches;
	int				rcu_pending;

	
	wait_queue_head_t		waitq;
	struct fasync_struct		*fasync;

	
	unsigned int			pending_wakeup;
	unsigned int			pending_kill;
	unsigned int			pending_disable;
	unsigned int			pending_sigtrap;
	unsigned long			pending_addr;	
	struct irq_work			pending_irq;
	struct callback_head		pending_task;
	unsigned int			pending_work;

	atomic_t			event_limit;

	
	struct perf_addr_filters_head	addr_filters;
	
	struct perf_addr_filter_range	*addr_filter_ranges;
	unsigned long			addr_filters_gen;

	
	struct perf_event		*aux_event;

	void (*destroy)(struct perf_event *);
	struct rcu_head			rcu_head;

	struct pid_namespace		*ns;
	u64				id;

	atomic64_t			lost_samples;

	u64				(*clock)(void);
	perf_overflow_handler_t		overflow_handler;
	void				*overflow_handler_context;
#ifdef CONFIG_BPF_SYSCALL
	perf_overflow_handler_t		orig_overflow_handler;
	struct bpf_prog			*prog;
	u64				bpf_cookie;
#endif

#ifdef CONFIG_EVENT_TRACING
	struct trace_event_call		*tp_event;
	struct event_filter		*filter;
#ifdef CONFIG_FUNCTION_TRACER
	struct ftrace_ops               ftrace_ops;
#endif
#endif

#ifdef CONFIG_CGROUP_PERF
	struct perf_cgroup		*cgrp; 
#endif

#ifdef CONFIG_SECURITY
	void *security;
#endif
	struct list_head		sb_list;

	
	__u32				orig_type;
#endif 
};


struct perf_event_pmu_context {
	struct pmu			*pmu;
	struct perf_event_context       *ctx;

	struct list_head		pmu_ctx_entry;

	struct list_head		pinned_active;
	struct list_head		flexible_active;

	
	unsigned int			embedded : 1;

	unsigned int			nr_events;
	unsigned int			nr_cgroups;

	atomic_t			refcount; 
	struct rcu_head			rcu_head;

	void				*task_ctx_data; 
	
	int				rotate_necessary;
};

struct perf_event_groups {
	struct rb_root	tree;
	u64		index;
};



struct perf_event_context {
	
	raw_spinlock_t			lock;
	
	struct mutex			mutex;

	struct list_head		pmu_ctx_list;
	struct perf_event_groups	pinned_groups;
	struct perf_event_groups	flexible_groups;
	struct list_head		event_list;

	int				nr_events;
	int				nr_user;
	int				is_active;

	int				nr_task_data;
	int				nr_stat;
	int				nr_freq;
	int				rotate_disable;

	refcount_t			refcount; 
	struct task_struct		*task;

	
	u64				time;
	u64				timestamp;
	u64				timeoffset;

	
	struct perf_event_context	*parent_ctx;
	u64				parent_gen;
	u64				generation;
	int				pin_count;
#ifdef CONFIG_CGROUP_PERF
	int				nr_cgroups;	 
#endif
	struct rcu_head			rcu_head;

	
	local_t				nr_pending;
};


#define PERF_NR_CONTEXTS	4

struct perf_cpu_pmu_context {
	struct perf_event_pmu_context	epc;
	struct perf_event_pmu_context	*task_epc;

	struct list_head		sched_cb_entry;
	int				sched_cb_usage;

	int				active_oncpu;
	int				exclusive;

	raw_spinlock_t			hrtimer_lock;
	struct hrtimer			hrtimer;
	ktime_t				hrtimer_interval;
	unsigned int			hrtimer_active;
};


struct perf_cpu_context {
	struct perf_event_context	ctx;
	struct perf_event_context	*task_ctx;
	int				online;

#ifdef CONFIG_CGROUP_PERF
	struct perf_cgroup		*cgrp;
#endif

	
	int				heap_size;
	struct perf_event		**heap;
	struct perf_event		*heap_default[2];
};

struct perf_output_handle {
	struct perf_event		*event;
	struct perf_buffer		*rb;
	unsigned long			wakeup;
	unsigned long			size;
	u64				aux_flags;
	union {
		void			*addr;
		unsigned long		head;
	};
	int				page;
};

struct bpf_perf_event_data_kern {
	bpf_user_pt_regs_t *regs;
	struct perf_sample_data *data;
	struct perf_event *event;
};

#ifdef CONFIG_CGROUP_PERF


struct perf_cgroup_info {
	u64				time;
	u64				timestamp;
	u64				timeoffset;
	int				active;
};

struct perf_cgroup {
	struct cgroup_subsys_state	css;
	struct perf_cgroup_info	__percpu *info;
};


static inline struct perf_cgroup *
perf_cgroup_from_task(struct task_struct *task, struct perf_event_context *ctx)
{
	return container_of(task_css_check(task, perf_event_cgrp_id,
					   ctx ? lockdep_is_held(&ctx->lock)
					       : true),
			    struct perf_cgroup, css);
}
#endif 

#ifdef CONFIG_PERF_EVENTS

extern struct perf_event_context *perf_cpu_task_ctx(void);

extern void *perf_aux_output_begin(struct perf_output_handle *handle,
				   struct perf_event *event);
extern void perf_aux_output_end(struct perf_output_handle *handle,
				unsigned long size);
extern int perf_aux_output_skip(struct perf_output_handle *handle,
				unsigned long size);
extern void *perf_get_aux(struct perf_output_handle *handle);
extern void perf_aux_output_flag(struct perf_output_handle *handle, u64 flags);
extern void perf_event_itrace_started(struct perf_event *event);

extern int perf_pmu_register(struct pmu *pmu, const char *name, int type);
extern void perf_pmu_unregister(struct pmu *pmu);

extern void __perf_event_task_sched_in(struct task_struct *prev,
				       struct task_struct *task);
extern void __perf_event_task_sched_out(struct task_struct *prev,
					struct task_struct *next);
extern int perf_event_init_task(struct task_struct *child, u64 clone_flags);
extern void perf_event_exit_task(struct task_struct *child);
extern void perf_event_free_task(struct task_struct *task);
extern void perf_event_delayed_put(struct task_struct *task);
extern struct file *perf_event_get(unsigned int fd);
extern const struct perf_event *perf_get_event(struct file *file);
extern const struct perf_event_attr *perf_event_attrs(struct perf_event *event);
extern void perf_event_print_debug(void);
extern void perf_pmu_disable(struct pmu *pmu);
extern void perf_pmu_enable(struct pmu *pmu);
extern void perf_sched_cb_dec(struct pmu *pmu);
extern void perf_sched_cb_inc(struct pmu *pmu);
extern int perf_event_task_disable(void);
extern int perf_event_task_enable(void);

extern void perf_pmu_resched(struct pmu *pmu);

extern int perf_event_refresh(struct perf_event *event, int refresh);
extern void perf_event_update_userpage(struct perf_event *event);
extern int perf_event_release_kernel(struct perf_event *event);
extern struct perf_event *
perf_event_create_kernel_counter(struct perf_event_attr *attr,
				int cpu,
				struct task_struct *task,
				perf_overflow_handler_t callback,
				void *context);
extern void perf_pmu_migrate_context(struct pmu *pmu,
				int src_cpu, int dst_cpu);
int perf_event_read_local(struct perf_event *event, u64 *value,
			  u64 *enabled, u64 *running);
extern u64 perf_event_read_value(struct perf_event *event,
				 u64 *enabled, u64 *running);

extern struct perf_callchain_entry *perf_callchain(struct perf_event *event, struct pt_regs *regs);

static inline bool branch_sample_no_flags(const struct perf_event *event)
{
	return event->attr.branch_sample_type & PERF_SAMPLE_BRANCH_NO_FLAGS;
}

static inline bool branch_sample_no_cycles(const struct perf_event *event)
{
	return event->attr.branch_sample_type & PERF_SAMPLE_BRANCH_NO_CYCLES;
}

static inline bool branch_sample_type(const struct perf_event *event)
{
	return event->attr.branch_sample_type & PERF_SAMPLE_BRANCH_TYPE_SAVE;
}

static inline bool branch_sample_hw_index(const struct perf_event *event)
{
	return event->attr.branch_sample_type & PERF_SAMPLE_BRANCH_HW_INDEX;
}

static inline bool branch_sample_priv(const struct perf_event *event)
{
	return event->attr.branch_sample_type & PERF_SAMPLE_BRANCH_PRIV_SAVE;
}


struct perf_sample_data {
	
	u64				sample_flags;
	u64				period;
	u64				dyn_size;

	
	u64				type;
	struct {
		u32	pid;
		u32	tid;
	}				tid_entry;
	u64				time;
	u64				id;
	struct {
		u32	cpu;
		u32	reserved;
	}				cpu_entry;

	
	u64				ip;
	struct perf_callchain_entry	*callchain;
	struct perf_raw_record		*raw;
	struct perf_branch_stack	*br_stack;
	union perf_sample_weight	weight;
	union  perf_mem_data_src	data_src;
	u64				txn;

	struct perf_regs		regs_user;
	struct perf_regs		regs_intr;
	u64				stack_user_size;

	u64				stream_id;
	u64				cgroup;
	u64				addr;
	u64				phys_addr;
	u64				data_page_size;
	u64				code_page_size;
	u64				aux_size;
} ____cacheline_aligned;


#define PERF_MEM_NA (PERF_MEM_S(OP, NA)   |\
		    PERF_MEM_S(LVL, NA)   |\
		    PERF_MEM_S(SNOOP, NA) |\
		    PERF_MEM_S(LOCK, NA)  |\
		    PERF_MEM_S(TLB, NA)   |\
		    PERF_MEM_S(LVLNUM, NA))

static inline void perf_sample_data_init(struct perf_sample_data *data,
					 u64 addr, u64 period)
{
	
	data->sample_flags = PERF_SAMPLE_PERIOD;
	data->period = period;
	data->dyn_size = 0;

	if (addr) {
		data->addr = addr;
		data->sample_flags |= PERF_SAMPLE_ADDR;
	}
}

static inline void perf_sample_save_callchain(struct perf_sample_data *data,
					      struct perf_event *event,
					      struct pt_regs *regs)
{
	int size = 1;

	data->callchain = perf_callchain(event, regs);
	size += data->callchain->nr;

	data->dyn_size += size * sizeof(u64);
	data->sample_flags |= PERF_SAMPLE_CALLCHAIN;
}

static inline void perf_sample_save_raw_data(struct perf_sample_data *data,
					     struct perf_raw_record *raw)
{
	struct perf_raw_frag *frag = &raw->frag;
	u32 sum = 0;
	int size;

	do {
		sum += frag->size;
		if (perf_raw_frag_last(frag))
			break;
		frag = frag->next;
	} while (1);

	size = round_up(sum + sizeof(u32), sizeof(u64));
	raw->size = size - sizeof(u32);
	frag->pad = raw->size - sum;

	data->raw = raw;
	data->dyn_size += size;
	data->sample_flags |= PERF_SAMPLE_RAW;
}

static inline void perf_sample_save_brstack(struct perf_sample_data *data,
					    struct perf_event *event,
					    struct perf_branch_stack *brs)
{
	int size = sizeof(u64); 

	if (branch_sample_hw_index(event))
		size += sizeof(u64);
	size += brs->nr * sizeof(struct perf_branch_entry);

	data->br_stack = brs;
	data->dyn_size += size;
	data->sample_flags |= PERF_SAMPLE_BRANCH_STACK;
}

static inline u32 perf_sample_data_size(struct perf_sample_data *data,
					struct perf_event *event)
{
	u32 size = sizeof(struct perf_event_header);

	size += event->header_size + event->id_header_size;
	size += data->dyn_size;

	return size;
}


static inline void perf_clear_branch_entry_bitfields(struct perf_branch_entry *br)
{
	br->mispred = 0;
	br->predicted = 0;
	br->in_tx = 0;
	br->abort = 0;
	br->cycles = 0;
	br->type = 0;
	br->spec = PERF_BR_SPEC_NA;
	br->reserved = 0;
}

extern void perf_output_sample(struct perf_output_handle *handle,
			       struct perf_event_header *header,
			       struct perf_sample_data *data,
			       struct perf_event *event);
extern void perf_prepare_sample(struct perf_sample_data *data,
				struct perf_event *event,
				struct pt_regs *regs);
extern void perf_prepare_header(struct perf_event_header *header,
				struct perf_sample_data *data,
				struct perf_event *event,
				struct pt_regs *regs);

extern int perf_event_overflow(struct perf_event *event,
				 struct perf_sample_data *data,
				 struct pt_regs *regs);

extern void perf_event_output_forward(struct perf_event *event,
				     struct perf_sample_data *data,
				     struct pt_regs *regs);
extern void perf_event_output_backward(struct perf_event *event,
				       struct perf_sample_data *data,
				       struct pt_regs *regs);
extern int perf_event_output(struct perf_event *event,
			     struct perf_sample_data *data,
			     struct pt_regs *regs);

static inline bool
__is_default_overflow_handler(perf_overflow_handler_t overflow_handler)
{
	if (likely(overflow_handler == perf_event_output_forward))
		return true;
	if (unlikely(overflow_handler == perf_event_output_backward))
		return true;
	return false;
}

#define is_default_overflow_handler(event) \
	__is_default_overflow_handler((event)->overflow_handler)

#ifdef CONFIG_BPF_SYSCALL
static inline bool uses_default_overflow_handler(struct perf_event *event)
{
	if (likely(is_default_overflow_handler(event)))
		return true;

	return __is_default_overflow_handler(event->orig_overflow_handler);
}
#else
#define uses_default_overflow_handler(event) \
	is_default_overflow_handler(event)
#endif

extern void
perf_event_header__init_id(struct perf_event_header *header,
			   struct perf_sample_data *data,
			   struct perf_event *event);
extern void
perf_event__output_id_sample(struct perf_event *event,
			     struct perf_output_handle *handle,
			     struct perf_sample_data *sample);

extern void
perf_log_lost_samples(struct perf_event *event, u64 lost);

static inline bool event_has_any_exclude_flag(struct perf_event *event)
{
	struct perf_event_attr *attr = &event->attr;

	return attr->exclude_idle || attr->exclude_user ||
	       attr->exclude_kernel || attr->exclude_hv ||
	       attr->exclude_guest || attr->exclude_host;
}

static inline bool is_sampling_event(struct perf_event *event)
{
	return event->attr.sample_period != 0;
}


static inline int is_software_event(struct perf_event *event)
{
	return event->event_caps & PERF_EV_CAP_SOFTWARE;
}


static inline int in_software_context(struct perf_event *event)
{
	return event->pmu_ctx->pmu->task_ctx_nr == perf_sw_context;
}

static inline int is_exclusive_pmu(struct pmu *pmu)
{
	return pmu->capabilities & PERF_PMU_CAP_EXCLUSIVE;
}

extern struct static_key perf_swevent_enabled[PERF_COUNT_SW_MAX];

extern void ___perf_sw_event(u32, u64, struct pt_regs *, u64);
extern void __perf_sw_event(u32, u64, struct pt_regs *, u64);

#ifndef perf_arch_fetch_caller_regs
static inline void perf_arch_fetch_caller_regs(struct pt_regs *regs, unsigned long ip) { }
#endif


static inline void perf_fetch_caller_regs(struct pt_regs *regs)
{
	perf_arch_fetch_caller_regs(regs, CALLER_ADDR0);
}

static __always_inline void
perf_sw_event(u32 event_id, u64 nr, struct pt_regs *regs, u64 addr)
{
	if (static_key_false(&perf_swevent_enabled[event_id]))
		__perf_sw_event(event_id, nr, regs, addr);
}

DECLARE_PER_CPU(struct pt_regs, __perf_regs[4]);


static __always_inline void __perf_sw_event_sched(u32 event_id, u64 nr, u64 addr)
{
	struct pt_regs *regs = this_cpu_ptr(&__perf_regs[0]);

	perf_fetch_caller_regs(regs);
	___perf_sw_event(event_id, nr, regs, addr);
}

extern struct static_key_false perf_sched_events;

static __always_inline bool __perf_sw_enabled(int swevt)
{
	return static_key_false(&perf_swevent_enabled[swevt]);
}

static inline void perf_event_task_migrate(struct task_struct *task)
{
	if (__perf_sw_enabled(PERF_COUNT_SW_CPU_MIGRATIONS))
		task->sched_migrated = 1;
}

static inline void perf_event_task_sched_in(struct task_struct *prev,
					    struct task_struct *task)
{
	if (static_branch_unlikely(&perf_sched_events))
		__perf_event_task_sched_in(prev, task);

	if (__perf_sw_enabled(PERF_COUNT_SW_CPU_MIGRATIONS) &&
	    task->sched_migrated) {
		__perf_sw_event_sched(PERF_COUNT_SW_CPU_MIGRATIONS, 1, 0);
		task->sched_migrated = 0;
	}
}

static inline void perf_event_task_sched_out(struct task_struct *prev,
					     struct task_struct *next)
{
	if (__perf_sw_enabled(PERF_COUNT_SW_CONTEXT_SWITCHES))
		__perf_sw_event_sched(PERF_COUNT_SW_CONTEXT_SWITCHES, 1, 0);

#ifdef CONFIG_CGROUP_PERF
	if (__perf_sw_enabled(PERF_COUNT_SW_CGROUP_SWITCHES) &&
	    perf_cgroup_from_task(prev, NULL) !=
	    perf_cgroup_from_task(next, NULL))
		__perf_sw_event_sched(PERF_COUNT_SW_CGROUP_SWITCHES, 1, 0);
#endif

	if (static_branch_unlikely(&perf_sched_events))
		__perf_event_task_sched_out(prev, next);
}

extern void perf_event_mmap(struct vm_area_struct *vma);

extern void perf_event_ksymbol(u16 ksym_type, u64 addr, u32 len,
			       bool unregister, const char *sym);
extern void perf_event_bpf_event(struct bpf_prog *prog,
				 enum perf_bpf_event_type type,
				 u16 flags);

#ifdef CONFIG_GUEST_PERF_EVENTS
extern struct perf_guest_info_callbacks __rcu *perf_guest_cbs;

DECLARE_STATIC_CALL(__perf_guest_state, *perf_guest_cbs->state);
DECLARE_STATIC_CALL(__perf_guest_get_ip, *perf_guest_cbs->get_ip);
DECLARE_STATIC_CALL(__perf_guest_handle_intel_pt_intr, *perf_guest_cbs->handle_intel_pt_intr);

static inline unsigned int perf_guest_state(void)
{
	return static_call(__perf_guest_state)();
}
static inline unsigned long perf_guest_get_ip(void)
{
	return static_call(__perf_guest_get_ip)();
}
static inline unsigned int perf_guest_handle_intel_pt_intr(void)
{
	return static_call(__perf_guest_handle_intel_pt_intr)();
}
extern void perf_register_guest_info_callbacks(struct perf_guest_info_callbacks *cbs);
extern void perf_unregister_guest_info_callbacks(struct perf_guest_info_callbacks *cbs);
#else
static inline unsigned int perf_guest_state(void)		 { return 0; }
static inline unsigned long perf_guest_get_ip(void)		 { return 0; }
static inline unsigned int perf_guest_handle_intel_pt_intr(void) { return 0; }
#endif 

extern void perf_event_exec(void);
extern void perf_event_comm(struct task_struct *tsk, bool exec);
extern void perf_event_namespaces(struct task_struct *tsk);
extern void perf_event_fork(struct task_struct *tsk);
extern void perf_event_text_poke(const void *addr,
				 const void *old_bytes, size_t old_len,
				 const void *new_bytes, size_t new_len);


DECLARE_PER_CPU(struct perf_callchain_entry, perf_callchain_entry);

extern void perf_callchain_user(struct perf_callchain_entry_ctx *entry, struct pt_regs *regs);
extern void perf_callchain_kernel(struct perf_callchain_entry_ctx *entry, struct pt_regs *regs);
extern struct perf_callchain_entry *
get_perf_callchain(struct pt_regs *regs, u32 init_nr, bool kernel, bool user,
		   u32 max_stack, bool crosstask, bool add_mark);
extern int get_callchain_buffers(int max_stack);
extern void put_callchain_buffers(void);
extern struct perf_callchain_entry *get_callchain_entry(int *rctx);
extern void put_callchain_entry(int rctx);

extern int sysctl_perf_event_max_stack;
extern int sysctl_perf_event_max_contexts_per_stack;

static inline int perf_callchain_store_context(struct perf_callchain_entry_ctx *ctx, u64 ip)
{
	if (ctx->contexts < sysctl_perf_event_max_contexts_per_stack) {
		struct perf_callchain_entry *entry = ctx->entry;
		entry->ip[entry->nr++] = ip;
		++ctx->contexts;
		return 0;
	} else {
		ctx->contexts_maxed = true;
		return -1; 
	}
}

static inline int perf_callchain_store(struct perf_callchain_entry_ctx *ctx, u64 ip)
{
	if (ctx->nr < ctx->max_stack && !ctx->contexts_maxed) {
		struct perf_callchain_entry *entry = ctx->entry;
		entry->ip[entry->nr++] = ip;
		++ctx->nr;
		return 0;
	} else {
		return -1; 
	}
}

extern int sysctl_perf_event_paranoid;
extern int sysctl_perf_event_mlock;
extern int sysctl_perf_event_sample_rate;
extern int sysctl_perf_cpu_time_max_percent;

extern void perf_sample_event_took(u64 sample_len_ns);

int perf_proc_update_handler(struct ctl_table *table, int write,
		void *buffer, size_t *lenp, loff_t *ppos);
int perf_cpu_time_max_percent_handler(struct ctl_table *table, int write,
		void *buffer, size_t *lenp, loff_t *ppos);
int perf_event_max_stack_handler(struct ctl_table *table, int write,
		void *buffer, size_t *lenp, loff_t *ppos);


#define PERF_SECURITY_OPEN		0


#define PERF_SECURITY_CPU		1
#define PERF_SECURITY_KERNEL		2
#define PERF_SECURITY_TRACEPOINT	3

static inline int perf_is_paranoid(void)
{
	return sysctl_perf_event_paranoid > -1;
}

static inline int perf_allow_kernel(struct perf_event_attr *attr)
{
	if (sysctl_perf_event_paranoid > 1 && !perfmon_capable())
		return -EACCES;

	return security_perf_event_open(attr, PERF_SECURITY_KERNEL);
}

static inline int perf_allow_cpu(struct perf_event_attr *attr)
{
	if (sysctl_perf_event_paranoid > 0 && !perfmon_capable())
		return -EACCES;

	return security_perf_event_open(attr, PERF_SECURITY_CPU);
}

static inline int perf_allow_tracepoint(struct perf_event_attr *attr)
{
	if (sysctl_perf_event_paranoid > -1 && !perfmon_capable())
		return -EPERM;

	return security_perf_event_open(attr, PERF_SECURITY_TRACEPOINT);
}

extern void perf_event_init(void);
extern void perf_tp_event(u16 event_type, u64 count, void *record,
			  int entry_size, struct pt_regs *regs,
			  struct hlist_head *head, int rctx,
			  struct task_struct *task);
extern void perf_bp_event(struct perf_event *event, void *data);

#ifndef perf_misc_flags
# define perf_misc_flags(regs) \
		(user_mode(regs) ? PERF_RECORD_MISC_USER : PERF_RECORD_MISC_KERNEL)
# define perf_instruction_pointer(regs)	instruction_pointer(regs)
#endif
#ifndef perf_arch_bpf_user_pt_regs
# define perf_arch_bpf_user_pt_regs(regs) regs
#endif

static inline bool has_branch_stack(struct perf_event *event)
{
	return event->attr.sample_type & PERF_SAMPLE_BRANCH_STACK;
}

static inline bool needs_branch_stack(struct perf_event *event)
{
	return event->attr.branch_sample_type != 0;
}

static inline bool has_aux(struct perf_event *event)
{
	return event->pmu->setup_aux;
}

static inline bool is_write_backward(struct perf_event *event)
{
	return !!event->attr.write_backward;
}

static inline bool has_addr_filter(struct perf_event *event)
{
	return event->pmu->nr_addr_filters;
}


static inline struct perf_addr_filters_head *
perf_event_addr_filters(struct perf_event *event)
{
	struct perf_addr_filters_head *ifh = &event->addr_filters;

	if (event->parent)
		ifh = &event->parent->addr_filters;

	return ifh;
}

extern void perf_event_addr_filters_sync(struct perf_event *event);
extern void perf_report_aux_output_id(struct perf_event *event, u64 hw_id);

extern int perf_output_begin(struct perf_output_handle *handle,
			     struct perf_sample_data *data,
			     struct perf_event *event, unsigned int size);
extern int perf_output_begin_forward(struct perf_output_handle *handle,
				     struct perf_sample_data *data,
				     struct perf_event *event,
				     unsigned int size);
extern int perf_output_begin_backward(struct perf_output_handle *handle,
				      struct perf_sample_data *data,
				      struct perf_event *event,
				      unsigned int size);

extern void perf_output_end(struct perf_output_handle *handle);
extern unsigned int perf_output_copy(struct perf_output_handle *handle,
			     const void *buf, unsigned int len);
extern unsigned int perf_output_skip(struct perf_output_handle *handle,
				     unsigned int len);
extern long perf_output_copy_aux(struct perf_output_handle *aux_handle,
				 struct perf_output_handle *handle,
				 unsigned long from, unsigned long to);
extern int perf_swevent_get_recursion_context(void);
extern void perf_swevent_put_recursion_context(int rctx);
extern u64 perf_swevent_set_period(struct perf_event *event);
extern void perf_event_enable(struct perf_event *event);
extern void perf_event_disable(struct perf_event *event);
extern void perf_event_disable_local(struct perf_event *event);
extern void perf_event_disable_inatomic(struct perf_event *event);
extern void perf_event_task_tick(void);
extern int perf_event_account_interrupt(struct perf_event *event);
extern int perf_event_period(struct perf_event *event, u64 value);
extern u64 perf_event_pause(struct perf_event *event, bool reset);
#else 
static inline void *
perf_aux_output_begin(struct perf_output_handle *handle,
		      struct perf_event *event)				{ return NULL; }
static inline void
perf_aux_output_end(struct perf_output_handle *handle, unsigned long size)
									{ }
static inline int
perf_aux_output_skip(struct perf_output_handle *handle,
		     unsigned long size)				{ return -EINVAL; }
static inline void *
perf_get_aux(struct perf_output_handle *handle)				{ return NULL; }
static inline void
perf_event_task_migrate(struct task_struct *task)			{ }
static inline void
perf_event_task_sched_in(struct task_struct *prev,
			 struct task_struct *task)			{ }
static inline void
perf_event_task_sched_out(struct task_struct *prev,
			  struct task_struct *next)			{ }
static inline int perf_event_init_task(struct task_struct *child,
				       u64 clone_flags)			{ return 0; }
static inline void perf_event_exit_task(struct task_struct *child)	{ }
static inline void perf_event_free_task(struct task_struct *task)	{ }
static inline void perf_event_delayed_put(struct task_struct *task)	{ }
static inline struct file *perf_event_get(unsigned int fd)	{ return ERR_PTR(-EINVAL); }
static inline const struct perf_event *perf_get_event(struct file *file)
{
	return ERR_PTR(-EINVAL);
}
static inline const struct perf_event_attr *perf_event_attrs(struct perf_event *event)
{
	return ERR_PTR(-EINVAL);
}
static inline int perf_event_read_local(struct perf_event *event, u64 *value,
					u64 *enabled, u64 *running)
{
	return -EINVAL;
}
static inline void perf_event_print_debug(void)				{ }
static inline int perf_event_task_disable(void)				{ return -EINVAL; }
static inline int perf_event_task_enable(void)				{ return -EINVAL; }
static inline int perf_event_refresh(struct perf_event *event, int refresh)
{
	return -EINVAL;
}

static inline void
perf_sw_event(u32 event_id, u64 nr, struct pt_regs *regs, u64 addr)	{ }
static inline void
perf_bp_event(struct perf_event *event, void *data)			{ }

static inline void perf_event_mmap(struct vm_area_struct *vma)		{ }

typedef int (perf_ksymbol_get_name_f)(char *name, int name_len, void *data);
static inline void perf_event_ksymbol(u16 ksym_type, u64 addr, u32 len,
				      bool unregister, const char *sym)	{ }
static inline void perf_event_bpf_event(struct bpf_prog *prog,
					enum perf_bpf_event_type type,
					u16 flags)			{ }
static inline void perf_event_exec(void)				{ }
static inline void perf_event_comm(struct task_struct *tsk, bool exec)	{ }
static inline void perf_event_namespaces(struct task_struct *tsk)	{ }
static inline void perf_event_fork(struct task_struct *tsk)		{ }
static inline void perf_event_text_poke(const void *addr,
					const void *old_bytes,
					size_t old_len,
					const void *new_bytes,
					size_t new_len)			{ }
static inline void perf_event_init(void)				{ }
static inline int  perf_swevent_get_recursion_context(void)		{ return -1; }
static inline void perf_swevent_put_recursion_context(int rctx)		{ }
static inline u64 perf_swevent_set_period(struct perf_event *event)	{ return 0; }
static inline void perf_event_enable(struct perf_event *event)		{ }
static inline void perf_event_disable(struct perf_event *event)		{ }
static inline int __perf_event_disable(void *info)			{ return -1; }
static inline void perf_event_task_tick(void)				{ }
static inline int perf_event_release_kernel(struct perf_event *event)	{ return 0; }
static inline int perf_event_period(struct perf_event *event, u64 value)
{
	return -EINVAL;
}
static inline u64 perf_event_pause(struct perf_event *event, bool reset)
{
	return 0;
}
#endif

#if defined(CONFIG_PERF_EVENTS) && defined(CONFIG_CPU_SUP_INTEL)
extern void perf_restore_debug_store(void);
#else
static inline void perf_restore_debug_store(void)			{ }
#endif

#define perf_output_put(handle, x) perf_output_copy((handle), &(x), sizeof(x))

struct perf_pmu_events_attr {
	struct device_attribute attr;
	u64 id;
	const char *event_str;
};

struct perf_pmu_events_ht_attr {
	struct device_attribute			attr;
	u64					id;
	const char				*event_str_ht;
	const char				*event_str_noht;
};

struct perf_pmu_events_hybrid_attr {
	struct device_attribute			attr;
	u64					id;
	const char				*event_str;
	u64					pmu_type;
};

struct perf_pmu_format_hybrid_attr {
	struct device_attribute			attr;
	u64					pmu_type;
};

ssize_t perf_event_sysfs_show(struct device *dev, struct device_attribute *attr,
			      char *page);

#define PMU_EVENT_ATTR(_name, _var, _id, _show)				\
static struct perf_pmu_events_attr _var = {				\
	.attr = __ATTR(_name, 0444, _show, NULL),			\
	.id   =  _id,							\
};

#define PMU_EVENT_ATTR_STRING(_name, _var, _str)			    \
static struct perf_pmu_events_attr _var = {				    \
	.attr		= __ATTR(_name, 0444, perf_event_sysfs_show, NULL), \
	.id		= 0,						    \
	.event_str	= _str,						    \
};

#define PMU_EVENT_ATTR_ID(_name, _show, _id)				\
	(&((struct perf_pmu_events_attr[]) {				\
		{ .attr = __ATTR(_name, 0444, _show, NULL),		\
		  .id = _id, }						\
	})[0].attr.attr)

#define PMU_FORMAT_ATTR_SHOW(_name, _format)				\
static ssize_t								\
_name##_show(struct device *dev,					\
			       struct device_attribute *attr,		\
			       char *page)				\
{									\
	BUILD_BUG_ON(sizeof(_format) >= PAGE_SIZE);			\
	return sprintf(page, _format "\n");				\
}									\

#define PMU_FORMAT_ATTR(_name, _format)					\
	PMU_FORMAT_ATTR_SHOW(_name, _format)				\
									\
static struct device_attribute format_attr_##_name = __ATTR_RO(_name)


#ifdef CONFIG_PERF_EVENTS
int perf_event_init_cpu(unsigned int cpu);
int perf_event_exit_cpu(unsigned int cpu);
#else
#define perf_event_init_cpu	NULL
#define perf_event_exit_cpu	NULL
#endif

extern void arch_perf_update_userpage(struct perf_event *event,
				      struct perf_event_mmap_page *userpg,
				      u64 now);




typedef int (perf_snapshot_branch_stack_t)(struct perf_branch_entry *entries,
					   unsigned int cnt);
DECLARE_STATIC_CALL(perf_snapshot_branch_stack, perf_snapshot_branch_stack_t);

#ifndef PERF_NEEDS_LOPWR_CB
static inline void perf_lopwr_cb(bool mode)
{
}
#endif

#endif 
