/* SPDX-License-Identifier: GPL-2.0 */

#ifndef _LINUX_TRACE_EVENT_H
#define _LINUX_TRACE_EVENT_H

#include <linux/ring_buffer.h>
#include <linux/trace_seq.h>
#include <linux/percpu.h>
#include <linux/hardirq.h>
#include <linux/perf_event.h>
#include <linux/tracepoint.h>

struct trace_array;
struct array_buffer;
struct tracer;
struct dentry;
struct bpf_prog;
union bpf_attr;

const char *trace_print_flags_seq(struct trace_seq *p, const char *delim,
				  unsigned long flags,
				  const struct trace_print_flags *flag_array);

const char *trace_print_symbols_seq(struct trace_seq *p, unsigned long val,
				    const struct trace_print_flags *symbol_array);

#if BITS_PER_LONG == 32
const char *trace_print_flags_seq_u64(struct trace_seq *p, const char *delim,
		      unsigned long long flags,
		      const struct trace_print_flags_u64 *flag_array);

const char *trace_print_symbols_seq_u64(struct trace_seq *p,
					unsigned long long val,
					const struct trace_print_flags_u64
								 *symbol_array);
#endif

const char *trace_print_bitmask_seq(struct trace_seq *p, void *bitmask_ptr,
				    unsigned int bitmask_size);

const char *trace_print_hex_seq(struct trace_seq *p,
				const unsigned char *buf, int len,
				bool concatenate);

const char *trace_print_array_seq(struct trace_seq *p,
				   const void *buf, int count,
				   size_t el_size);

const char *
trace_print_hex_dump_seq(struct trace_seq *p, const char *prefix_str,
			 int prefix_type, int rowsize, int groupsize,
			 const void *buf, size_t len, bool ascii);

struct trace_iterator;
struct trace_event;

int trace_raw_output_prep(struct trace_iterator *iter,
			  struct trace_event *event);
extern __printf(2, 3)
void trace_event_printf(struct trace_iterator *iter, const char *fmt, ...);


struct trace_dynamic_info {
#ifdef CONFIG_CPU_BIG_ENDIAN
	u16	len;
	u16	offset;
#else
	u16	offset;
	u16	len;
#endif
} __packed;


struct trace_entry {
	unsigned short		type;
	unsigned char		flags;
	unsigned char		preempt_count;
	int			pid;
};

#define TRACE_EVENT_TYPE_MAX						\
	((1 << (sizeof(((struct trace_entry *)0)->type) * 8)) - 1)


struct trace_iterator {
	struct trace_array	*tr;
	struct tracer		*trace;
	struct array_buffer	*array_buffer;
	void			*private;
	int			cpu_file;
	struct mutex		mutex;
	struct ring_buffer_iter	**buffer_iter;
	unsigned long		iter_flags;
	void			*temp;	
	unsigned int		temp_size;
	char			*fmt;	
	unsigned int		fmt_size;
	long			wait_index;

	
	struct trace_seq	tmp_seq;

	cpumask_var_t		started;

	
	bool			snapshot;

	
	struct trace_seq	seq;
	struct trace_entry	*ent;
	unsigned long		lost_events;
	int			leftover;
	int			ent_size;
	int			cpu;
	u64			ts;

	loff_t			pos;
	long			idx;

	
};

enum trace_iter_flags {
	TRACE_FILE_LAT_FMT	= 1,
	TRACE_FILE_ANNOTATE	= 2,
	TRACE_FILE_TIME_IN_NS	= 4,
};


typedef enum print_line_t (*trace_print_func)(struct trace_iterator *iter,
				      int flags, struct trace_event *event);

struct trace_event_functions {
	trace_print_func	trace;
	trace_print_func	raw;
	trace_print_func	hex;
	trace_print_func	binary;
};

struct trace_event {
	struct hlist_node		node;
	int				type;
	struct trace_event_functions	*funcs;
};

extern int register_trace_event(struct trace_event *event);
extern int unregister_trace_event(struct trace_event *event);


enum print_line_t {
	TRACE_TYPE_PARTIAL_LINE	= 0,	
	TRACE_TYPE_HANDLED	= 1,
	TRACE_TYPE_UNHANDLED	= 2,	
	TRACE_TYPE_NO_CONSUME	= 3	
};

enum print_line_t trace_handle_return(struct trace_seq *s);

static inline void tracing_generic_entry_update(struct trace_entry *entry,
						unsigned short type,
						unsigned int trace_ctx)
{
	entry->preempt_count		= trace_ctx & 0xff;
	entry->pid			= current->pid;
	entry->type			= type;
	entry->flags =			trace_ctx >> 16;
}

unsigned int tracing_gen_ctx_irq_test(unsigned int irqs_status);

enum trace_flag_type {
	TRACE_FLAG_IRQS_OFF		= 0x01,
	TRACE_FLAG_IRQS_NOSUPPORT	= 0x02,
	TRACE_FLAG_NEED_RESCHED		= 0x04,
	TRACE_FLAG_HARDIRQ		= 0x08,
	TRACE_FLAG_SOFTIRQ		= 0x10,
	TRACE_FLAG_PREEMPT_RESCHED	= 0x20,
	TRACE_FLAG_NMI			= 0x40,
	TRACE_FLAG_BH_OFF		= 0x80,
};

#ifdef CONFIG_TRACE_IRQFLAGS_SUPPORT
static inline unsigned int tracing_gen_ctx_flags(unsigned long irqflags)
{
	unsigned int irq_status = irqs_disabled_flags(irqflags) ?
		TRACE_FLAG_IRQS_OFF : 0;
	return tracing_gen_ctx_irq_test(irq_status);
}
static inline unsigned int tracing_gen_ctx(void)
{
	unsigned long irqflags;

	local_save_flags(irqflags);
	return tracing_gen_ctx_flags(irqflags);
}
#else

static inline unsigned int tracing_gen_ctx_flags(unsigned long irqflags)
{
	return tracing_gen_ctx_irq_test(TRACE_FLAG_IRQS_NOSUPPORT);
}
static inline unsigned int tracing_gen_ctx(void)
{
	return tracing_gen_ctx_irq_test(TRACE_FLAG_IRQS_NOSUPPORT);
}
#endif

static inline unsigned int tracing_gen_ctx_dec(void)
{
	unsigned int trace_ctx;

	trace_ctx = tracing_gen_ctx();
	
	if (IS_ENABLED(CONFIG_PREEMPTION))
		trace_ctx--;
	return trace_ctx;
}

struct trace_event_file;

struct ring_buffer_event *
trace_event_buffer_lock_reserve(struct trace_buffer **current_buffer,
				struct trace_event_file *trace_file,
				int type, unsigned long len,
				unsigned int trace_ctx);

#define TRACE_RECORD_CMDLINE	BIT(0)
#define TRACE_RECORD_TGID	BIT(1)

void tracing_record_taskinfo(struct task_struct *task, int flags);
void tracing_record_taskinfo_sched_switch(struct task_struct *prev,
					  struct task_struct *next, int flags);

void tracing_record_cmdline(struct task_struct *task);
void tracing_record_tgid(struct task_struct *task);

int trace_output_call(struct trace_iterator *iter, char *name, char *fmt, ...)
	 __printf(3, 4);

struct event_filter;

enum trace_reg {
	TRACE_REG_REGISTER,
	TRACE_REG_UNREGISTER,
#ifdef CONFIG_PERF_EVENTS
	TRACE_REG_PERF_REGISTER,
	TRACE_REG_PERF_UNREGISTER,
	TRACE_REG_PERF_OPEN,
	TRACE_REG_PERF_CLOSE,
	
	TRACE_REG_PERF_ADD,
	TRACE_REG_PERF_DEL,
#endif
};

struct trace_event_call;

#define TRACE_FUNCTION_TYPE ((const char *)~0UL)

struct trace_event_fields {
	const char *type;
	union {
		struct {
			const char *name;
			const int  size;
			const int  align;
			const int  is_signed;
			const int  filter_type;
			const int  len;
		};
		int (*define_fields)(struct trace_event_call *);
	};
};

struct trace_event_class {
	const char		*system;
	void			*probe;
#ifdef CONFIG_PERF_EVENTS
	void			*perf_probe;
#endif
	int			(*reg)(struct trace_event_call *event,
				       enum trace_reg type, void *data);
	struct trace_event_fields *fields_array;
	struct list_head	*(*get_fields)(struct trace_event_call *);
	struct list_head	fields;
	int			(*raw_init)(struct trace_event_call *);
};

extern int trace_event_reg(struct trace_event_call *event,
			    enum trace_reg type, void *data);

struct trace_event_buffer {
	struct trace_buffer		*buffer;
	struct ring_buffer_event	*event;
	struct trace_event_file		*trace_file;
	void				*entry;
	unsigned int			trace_ctx;
	struct pt_regs			*regs;
};

void *trace_event_buffer_reserve(struct trace_event_buffer *fbuffer,
				  struct trace_event_file *trace_file,
				  unsigned long len);

void trace_event_buffer_commit(struct trace_event_buffer *fbuffer);

enum {
	TRACE_EVENT_FL_FILTERED_BIT,
	TRACE_EVENT_FL_CAP_ANY_BIT,
	TRACE_EVENT_FL_NO_SET_FILTER_BIT,
	TRACE_EVENT_FL_IGNORE_ENABLE_BIT,
	TRACE_EVENT_FL_TRACEPOINT_BIT,
	TRACE_EVENT_FL_DYNAMIC_BIT,
	TRACE_EVENT_FL_KPROBE_BIT,
	TRACE_EVENT_FL_UPROBE_BIT,
	TRACE_EVENT_FL_EPROBE_BIT,
	TRACE_EVENT_FL_FPROBE_BIT,
	TRACE_EVENT_FL_CUSTOM_BIT,
};


enum {
	TRACE_EVENT_FL_FILTERED		= (1 << TRACE_EVENT_FL_FILTERED_BIT),
	TRACE_EVENT_FL_CAP_ANY		= (1 << TRACE_EVENT_FL_CAP_ANY_BIT),
	TRACE_EVENT_FL_NO_SET_FILTER	= (1 << TRACE_EVENT_FL_NO_SET_FILTER_BIT),
	TRACE_EVENT_FL_IGNORE_ENABLE	= (1 << TRACE_EVENT_FL_IGNORE_ENABLE_BIT),
	TRACE_EVENT_FL_TRACEPOINT	= (1 << TRACE_EVENT_FL_TRACEPOINT_BIT),
	TRACE_EVENT_FL_DYNAMIC		= (1 << TRACE_EVENT_FL_DYNAMIC_BIT),
	TRACE_EVENT_FL_KPROBE		= (1 << TRACE_EVENT_FL_KPROBE_BIT),
	TRACE_EVENT_FL_UPROBE		= (1 << TRACE_EVENT_FL_UPROBE_BIT),
	TRACE_EVENT_FL_EPROBE		= (1 << TRACE_EVENT_FL_EPROBE_BIT),
	TRACE_EVENT_FL_FPROBE		= (1 << TRACE_EVENT_FL_FPROBE_BIT),
	TRACE_EVENT_FL_CUSTOM		= (1 << TRACE_EVENT_FL_CUSTOM_BIT),
};

#define TRACE_EVENT_FL_UKPROBE (TRACE_EVENT_FL_KPROBE | TRACE_EVENT_FL_UPROBE)

struct trace_event_call {
	struct list_head	list;
	struct trace_event_class *class;
	union {
		char			*name;
		
		struct tracepoint	*tp;
	};
	struct trace_event	event;
	char			*print_fmt;
	struct event_filter	*filter;
	
	union {
		void				*module;
		atomic_t			refcnt;
	};
	void			*data;

	
	int			flags; 

#ifdef CONFIG_PERF_EVENTS
	int				perf_refcount;
	struct hlist_head __percpu	*perf_events;
	struct bpf_prog_array __rcu	*prog_array;

	int	(*perf_perm)(struct trace_event_call *,
			     struct perf_event *);
#endif
};

#ifdef CONFIG_DYNAMIC_EVENTS
bool trace_event_dyn_try_get_ref(struct trace_event_call *call);
void trace_event_dyn_put_ref(struct trace_event_call *call);
bool trace_event_dyn_busy(struct trace_event_call *call);
#else
static inline bool trace_event_dyn_try_get_ref(struct trace_event_call *call)
{
	
	return false;
}
static inline void trace_event_dyn_put_ref(struct trace_event_call *call)
{
}
static inline bool trace_event_dyn_busy(struct trace_event_call *call)
{
	
	return true;
}
#endif

static inline bool trace_event_try_get_ref(struct trace_event_call *call)
{
	if (call->flags & TRACE_EVENT_FL_DYNAMIC)
		return trace_event_dyn_try_get_ref(call);
	else
		return try_module_get(call->module);
}

static inline void trace_event_put_ref(struct trace_event_call *call)
{
	if (call->flags & TRACE_EVENT_FL_DYNAMIC)
		trace_event_dyn_put_ref(call);
	else
		module_put(call->module);
}

#ifdef CONFIG_PERF_EVENTS
static inline bool bpf_prog_array_valid(struct trace_event_call *call)
{
	
	return !!READ_ONCE(call->prog_array);
}
#endif

static inline const char *
trace_event_name(struct trace_event_call *call)
{
	if (call->flags & TRACE_EVENT_FL_CUSTOM)
		return call->name;
	else if (call->flags & TRACE_EVENT_FL_TRACEPOINT)
		return call->tp ? call->tp->name : NULL;
	else
		return call->name;
}

static inline struct list_head *
trace_get_fields(struct trace_event_call *event_call)
{
	if (!event_call->class->get_fields)
		return &event_call->class->fields;
	return event_call->class->get_fields(event_call);
}

struct trace_subsystem_dir;

enum {
	EVENT_FILE_FL_ENABLED_BIT,
	EVENT_FILE_FL_RECORDED_CMD_BIT,
	EVENT_FILE_FL_RECORDED_TGID_BIT,
	EVENT_FILE_FL_FILTERED_BIT,
	EVENT_FILE_FL_NO_SET_FILTER_BIT,
	EVENT_FILE_FL_SOFT_MODE_BIT,
	EVENT_FILE_FL_SOFT_DISABLED_BIT,
	EVENT_FILE_FL_TRIGGER_MODE_BIT,
	EVENT_FILE_FL_TRIGGER_COND_BIT,
	EVENT_FILE_FL_PID_FILTER_BIT,
	EVENT_FILE_FL_WAS_ENABLED_BIT,
	EVENT_FILE_FL_FREED_BIT,
};

extern struct trace_event_file *trace_get_event_file(const char *instance,
						     const char *system,
						     const char *event);
extern void trace_put_event_file(struct trace_event_file *file);

#define MAX_DYNEVENT_CMD_LEN	(2048)

enum dynevent_type {
	DYNEVENT_TYPE_SYNTH = 1,
	DYNEVENT_TYPE_KPROBE,
	DYNEVENT_TYPE_NONE,
};

struct dynevent_cmd;

typedef int (*dynevent_create_fn_t)(struct dynevent_cmd *cmd);

struct dynevent_cmd {
	struct seq_buf		seq;
	const char		*event_name;
	unsigned int		n_fields;
	enum dynevent_type	type;
	dynevent_create_fn_t	run_command;
	void			*private_data;
};

extern int dynevent_create(struct dynevent_cmd *cmd);

extern int synth_event_delete(const char *name);

extern void synth_event_cmd_init(struct dynevent_cmd *cmd,
				 char *buf, int maxlen);

extern int __synth_event_gen_cmd_start(struct dynevent_cmd *cmd,
				       const char *name,
				       struct module *mod, ...);

#define synth_event_gen_cmd_start(cmd, name, mod, ...)	\
	__synth_event_gen_cmd_start(cmd, name, mod, ## __VA_ARGS__, NULL)

struct synth_field_desc {
	const char *type;
	const char *name;
};

extern int synth_event_gen_cmd_array_start(struct dynevent_cmd *cmd,
					   const char *name,
					   struct module *mod,
					   struct synth_field_desc *fields,
					   unsigned int n_fields);
extern int synth_event_create(const char *name,
			      struct synth_field_desc *fields,
			      unsigned int n_fields, struct module *mod);

extern int synth_event_add_field(struct dynevent_cmd *cmd,
				 const char *type,
				 const char *name);
extern int synth_event_add_field_str(struct dynevent_cmd *cmd,
				     const char *type_name);
extern int synth_event_add_fields(struct dynevent_cmd *cmd,
				  struct synth_field_desc *fields,
				  unsigned int n_fields);

#define synth_event_gen_cmd_end(cmd)	\
	dynevent_create(cmd)

struct synth_event;

struct synth_event_trace_state {
	struct trace_event_buffer fbuffer;
	struct synth_trace_event *entry;
	struct trace_buffer *buffer;
	struct synth_event *event;
	unsigned int cur_field;
	unsigned int n_u64;
	bool disabled;
	bool add_next;
	bool add_name;
};

extern int synth_event_trace(struct trace_event_file *file,
			     unsigned int n_vals, ...);
extern int synth_event_trace_array(struct trace_event_file *file, u64 *vals,
				   unsigned int n_vals);
extern int synth_event_trace_start(struct trace_event_file *file,
				   struct synth_event_trace_state *trace_state);
extern int synth_event_add_next_val(u64 val,
				    struct synth_event_trace_state *trace_state);
extern int synth_event_add_val(const char *field_name, u64 val,
			       struct synth_event_trace_state *trace_state);
extern int synth_event_trace_end(struct synth_event_trace_state *trace_state);

extern int kprobe_event_delete(const char *name);

extern void kprobe_event_cmd_init(struct dynevent_cmd *cmd,
				  char *buf, int maxlen);

#define kprobe_event_gen_cmd_start(cmd, name, loc, ...)			\
	__kprobe_event_gen_cmd_start(cmd, false, name, loc, ## __VA_ARGS__, NULL)

#define kretprobe_event_gen_cmd_start(cmd, name, loc, ...)		\
	__kprobe_event_gen_cmd_start(cmd, true, name, loc, ## __VA_ARGS__, NULL)

extern int __kprobe_event_gen_cmd_start(struct dynevent_cmd *cmd,
					bool kretprobe,
					const char *name,
					const char *loc, ...);

#define kprobe_event_add_fields(cmd, ...)	\
	__kprobe_event_add_fields(cmd, ## __VA_ARGS__, NULL)

#define kprobe_event_add_field(cmd, field)	\
	__kprobe_event_add_fields(cmd, field, NULL)

extern int __kprobe_event_add_fields(struct dynevent_cmd *cmd, ...);

#define kprobe_event_gen_cmd_end(cmd)		\
	dynevent_create(cmd)

#define kretprobe_event_gen_cmd_end(cmd)	\
	dynevent_create(cmd)


enum {
	EVENT_FILE_FL_ENABLED		= (1 << EVENT_FILE_FL_ENABLED_BIT),
	EVENT_FILE_FL_RECORDED_CMD	= (1 << EVENT_FILE_FL_RECORDED_CMD_BIT),
	EVENT_FILE_FL_RECORDED_TGID	= (1 << EVENT_FILE_FL_RECORDED_TGID_BIT),
	EVENT_FILE_FL_FILTERED		= (1 << EVENT_FILE_FL_FILTERED_BIT),
	EVENT_FILE_FL_NO_SET_FILTER	= (1 << EVENT_FILE_FL_NO_SET_FILTER_BIT),
	EVENT_FILE_FL_SOFT_MODE		= (1 << EVENT_FILE_FL_SOFT_MODE_BIT),
	EVENT_FILE_FL_SOFT_DISABLED	= (1 << EVENT_FILE_FL_SOFT_DISABLED_BIT),
	EVENT_FILE_FL_TRIGGER_MODE	= (1 << EVENT_FILE_FL_TRIGGER_MODE_BIT),
	EVENT_FILE_FL_TRIGGER_COND	= (1 << EVENT_FILE_FL_TRIGGER_COND_BIT),
	EVENT_FILE_FL_PID_FILTER	= (1 << EVENT_FILE_FL_PID_FILTER_BIT),
	EVENT_FILE_FL_WAS_ENABLED	= (1 << EVENT_FILE_FL_WAS_ENABLED_BIT),
	EVENT_FILE_FL_FREED		= (1 << EVENT_FILE_FL_FREED_BIT),
};

struct trace_event_file {
	struct list_head		list;
	struct trace_event_call		*event_call;
	struct event_filter __rcu	*filter;
	struct eventfs_inode		*ei;
	struct trace_array		*tr;
	struct trace_subsystem_dir	*system;
	struct list_head		triggers;

	
	unsigned long		flags;
	atomic_t		ref;	
	atomic_t		sm_ref;	
	atomic_t		tm_ref;	
};

#define __TRACE_EVENT_FLAGS(name, value)				\
	static int __init trace_init_flags_##name(void)			\
	{								\
		event_##name.flags |= value;				\
		return 0;						\
	}								\
	early_initcall(trace_init_flags_##name);

#define __TRACE_EVENT_PERF_PERM(name, expr...)				\
	static int perf_perm_##name(struct trace_event_call *tp_event, \
				    struct perf_event *p_event)		\
	{								\
		return ({ expr; });					\
	}								\
	static int __init trace_init_perf_perm_##name(void)		\
	{								\
		event_##name.perf_perm = &perf_perm_##name;		\
		return 0;						\
	}								\
	early_initcall(trace_init_perf_perm_##name);

#define PERF_MAX_TRACE_SIZE	8192

#define MAX_FILTER_STR_VAL	256U	

enum event_trigger_type {
	ETT_NONE		= (0),
	ETT_TRACE_ONOFF		= (1 << 0),
	ETT_SNAPSHOT		= (1 << 1),
	ETT_STACKTRACE		= (1 << 2),
	ETT_EVENT_ENABLE	= (1 << 3),
	ETT_EVENT_HIST		= (1 << 4),
	ETT_HIST_ENABLE		= (1 << 5),
	ETT_EVENT_EPROBE	= (1 << 6),
};

extern int filter_match_preds(struct event_filter *filter, void *rec);

extern enum event_trigger_type
event_triggers_call(struct trace_event_file *file,
		    struct trace_buffer *buffer, void *rec,
		    struct ring_buffer_event *event);
extern void
event_triggers_post_call(struct trace_event_file *file,
			 enum event_trigger_type tt);

bool trace_event_ignore_this_pid(struct trace_event_file *trace_file);

bool __trace_trigger_soft_disabled(struct trace_event_file *file);


static __always_inline bool
trace_trigger_soft_disabled(struct trace_event_file *file)
{
	unsigned long eflags = file->flags;

	if (likely(!(eflags & (EVENT_FILE_FL_TRIGGER_MODE |
			       EVENT_FILE_FL_SOFT_DISABLED |
			       EVENT_FILE_FL_PID_FILTER))))
		return false;

	if (likely(eflags & EVENT_FILE_FL_TRIGGER_COND))
		return false;

	return __trace_trigger_soft_disabled(file);
}

#ifdef CONFIG_BPF_EVENTS
unsigned int trace_call_bpf(struct trace_event_call *call, void *ctx);
int perf_event_attach_bpf_prog(struct perf_event *event, struct bpf_prog *prog, u64 bpf_cookie);
void perf_event_detach_bpf_prog(struct perf_event *event);
int perf_event_query_prog_array(struct perf_event *event, void __user *info);
int bpf_probe_register(struct bpf_raw_event_map *btp, struct bpf_prog *prog);
int bpf_probe_unregister(struct bpf_raw_event_map *btp, struct bpf_prog *prog);
struct bpf_raw_event_map *bpf_get_raw_tracepoint(const char *name);
void bpf_put_raw_tracepoint(struct bpf_raw_event_map *btp);
int bpf_get_perf_event_info(const struct perf_event *event, u32 *prog_id,
			    u32 *fd_type, const char **buf,
			    u64 *probe_offset, u64 *probe_addr);
int bpf_kprobe_multi_link_attach(const union bpf_attr *attr, struct bpf_prog *prog);
int bpf_uprobe_multi_link_attach(const union bpf_attr *attr, struct bpf_prog *prog);
#else
static inline unsigned int trace_call_bpf(struct trace_event_call *call, void *ctx)
{
	return 1;
}

static inline int
perf_event_attach_bpf_prog(struct perf_event *event, struct bpf_prog *prog, u64 bpf_cookie)
{
	return -EOPNOTSUPP;
}

static inline void perf_event_detach_bpf_prog(struct perf_event *event) { }

static inline int
perf_event_query_prog_array(struct perf_event *event, void __user *info)
{
	return -EOPNOTSUPP;
}
static inline int bpf_probe_register(struct bpf_raw_event_map *btp, struct bpf_prog *p)
{
	return -EOPNOTSUPP;
}
static inline int bpf_probe_unregister(struct bpf_raw_event_map *btp, struct bpf_prog *p)
{
	return -EOPNOTSUPP;
}
static inline struct bpf_raw_event_map *bpf_get_raw_tracepoint(const char *name)
{
	return NULL;
}
static inline void bpf_put_raw_tracepoint(struct bpf_raw_event_map *btp)
{
}
static inline int bpf_get_perf_event_info(const struct perf_event *event,
					  u32 *prog_id, u32 *fd_type,
					  const char **buf, u64 *probe_offset,
					  u64 *probe_addr)
{
	return -EOPNOTSUPP;
}
static inline int
bpf_kprobe_multi_link_attach(const union bpf_attr *attr, struct bpf_prog *prog)
{
	return -EOPNOTSUPP;
}
static inline int
bpf_uprobe_multi_link_attach(const union bpf_attr *attr, struct bpf_prog *prog)
{
	return -EOPNOTSUPP;
}
#endif

enum {
	FILTER_OTHER = 0,
	FILTER_STATIC_STRING,
	FILTER_DYN_STRING,
	FILTER_RDYN_STRING,
	FILTER_PTR_STRING,
	FILTER_TRACE_FN,
	FILTER_CPUMASK,
	FILTER_COMM,
	FILTER_CPU,
	FILTER_STACKTRACE,
};

extern int trace_event_raw_init(struct trace_event_call *call);
extern int trace_define_field(struct trace_event_call *call, const char *type,
			      const char *name, int offset, int size,
			      int is_signed, int filter_type);
extern int trace_add_event_call(struct trace_event_call *call);
extern int trace_remove_event_call(struct trace_event_call *call);
extern int trace_event_get_offsets(struct trace_event_call *call);

int ftrace_set_clr_event(struct trace_array *tr, char *buf, int set);
int trace_set_clr_event(const char *system, const char *event, int set);
int trace_array_set_clr_event(struct trace_array *tr, const char *system,
		const char *event, bool enable);

#define event_trace_printk(ip, fmt, args...)				\
do {									\
	__trace_printk_check_format(fmt, ##args);			\
	tracing_record_cmdline(current);				\
	if (__builtin_constant_p(fmt)) {				\
		static const char *trace_printk_fmt			\
		  __section("__trace_printk_fmt") =			\
			__builtin_constant_p(fmt) ? fmt : NULL;		\
									\
		__trace_bprintk(ip, trace_printk_fmt, ##args);		\
	} else								\
		__trace_printk(ip, fmt, ##args);			\
} while (0)

#ifdef CONFIG_PERF_EVENTS
struct perf_event;

DECLARE_PER_CPU(struct pt_regs, perf_trace_regs);
DECLARE_PER_CPU(int, bpf_kprobe_override);

extern int  perf_trace_init(struct perf_event *event);
extern void perf_trace_destroy(struct perf_event *event);
extern int  perf_trace_add(struct perf_event *event, int flags);
extern void perf_trace_del(struct perf_event *event, int flags);
#ifdef CONFIG_KPROBE_EVENTS
extern int  perf_kprobe_init(struct perf_event *event, bool is_retprobe);
extern void perf_kprobe_destroy(struct perf_event *event);
extern int bpf_get_kprobe_info(const struct perf_event *event,
			       u32 *fd_type, const char **symbol,
			       u64 *probe_offset, u64 *probe_addr,
			       bool perf_type_tracepoint);
#endif
#ifdef CONFIG_UPROBE_EVENTS
extern int  perf_uprobe_init(struct perf_event *event,
			     unsigned long ref_ctr_offset, bool is_retprobe);
extern void perf_uprobe_destroy(struct perf_event *event);
extern int bpf_get_uprobe_info(const struct perf_event *event,
			       u32 *fd_type, const char **filename,
			       u64 *probe_offset, u64 *probe_addr,
			       bool perf_type_tracepoint);
#endif
extern int  ftrace_profile_set_filter(struct perf_event *event, int event_id,
				     char *filter_str);
extern void ftrace_profile_free_filter(struct perf_event *event);
void perf_trace_buf_update(void *record, u16 type);
void *perf_trace_buf_alloc(int size, struct pt_regs **regs, int *rctxp);

int perf_event_set_bpf_prog(struct perf_event *event, struct bpf_prog *prog, u64 bpf_cookie);
void perf_event_free_bpf_prog(struct perf_event *event);

void bpf_trace_run1(struct bpf_prog *prog, u64 arg1);
void bpf_trace_run2(struct bpf_prog *prog, u64 arg1, u64 arg2);
void bpf_trace_run3(struct bpf_prog *prog, u64 arg1, u64 arg2,
		    u64 arg3);
void bpf_trace_run4(struct bpf_prog *prog, u64 arg1, u64 arg2,
		    u64 arg3, u64 arg4);
void bpf_trace_run5(struct bpf_prog *prog, u64 arg1, u64 arg2,
		    u64 arg3, u64 arg4, u64 arg5);
void bpf_trace_run6(struct bpf_prog *prog, u64 arg1, u64 arg2,
		    u64 arg3, u64 arg4, u64 arg5, u64 arg6);
void bpf_trace_run7(struct bpf_prog *prog, u64 arg1, u64 arg2,
		    u64 arg3, u64 arg4, u64 arg5, u64 arg6, u64 arg7);
void bpf_trace_run8(struct bpf_prog *prog, u64 arg1, u64 arg2,
		    u64 arg3, u64 arg4, u64 arg5, u64 arg6, u64 arg7,
		    u64 arg8);
void bpf_trace_run9(struct bpf_prog *prog, u64 arg1, u64 arg2,
		    u64 arg3, u64 arg4, u64 arg5, u64 arg6, u64 arg7,
		    u64 arg8, u64 arg9);
void bpf_trace_run10(struct bpf_prog *prog, u64 arg1, u64 arg2,
		     u64 arg3, u64 arg4, u64 arg5, u64 arg6, u64 arg7,
		     u64 arg8, u64 arg9, u64 arg10);
void bpf_trace_run11(struct bpf_prog *prog, u64 arg1, u64 arg2,
		     u64 arg3, u64 arg4, u64 arg5, u64 arg6, u64 arg7,
		     u64 arg8, u64 arg9, u64 arg10, u64 arg11);
void bpf_trace_run12(struct bpf_prog *prog, u64 arg1, u64 arg2,
		     u64 arg3, u64 arg4, u64 arg5, u64 arg6, u64 arg7,
		     u64 arg8, u64 arg9, u64 arg10, u64 arg11, u64 arg12);
void perf_trace_run_bpf_submit(void *raw_data, int size, int rctx,
			       struct trace_event_call *call, u64 count,
			       struct pt_regs *regs, struct hlist_head *head,
			       struct task_struct *task);

static inline void
perf_trace_buf_submit(void *raw_data, int size, int rctx, u16 type,
		       u64 count, struct pt_regs *regs, void *head,
		       struct task_struct *task)
{
	perf_tp_event(type, count, raw_data, size, regs, head, rctx, task);
}

#endif

#define TRACE_EVENT_STR_MAX	512


#define __trace_event_vstr_len(fmt, va)			\
({							\
	va_list __ap;					\
	int __ret;					\
							\
	va_copy(__ap, *(va));				\
	__ret = vsnprintf(NULL, 0, fmt, __ap) + 1;	\
	va_end(__ap);					\
							\
	min(__ret, TRACE_EVENT_STR_MAX);		\
})

#endif 



#ifndef TRACE_CUSTOM_EVENT

#define DECLARE_CUSTOM_EVENT_CLASS(name, proto, args, tstruct, assign, print)
#define DEFINE_CUSTOM_EVENT(template, name, proto, args)
#define TRACE_CUSTOM_EVENT(name, proto, args, struct, assign, print)

#endif 
