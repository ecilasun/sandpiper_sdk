/* SPDX-License-Identifier: GPL-2.0 */


#ifndef _LINUX_FTRACE_H
#define _LINUX_FTRACE_H

#include <linux/trace_recursion.h>
#include <linux/trace_clock.h>
#include <linux/jump_label.h>
#include <linux/kallsyms.h>
#include <linux/linkage.h>
#include <linux/bitops.h>
#include <linux/ptrace.h>
#include <linux/ktime.h>
#include <linux/sched.h>
#include <linux/types.h>
#include <linux/init.h>
#include <linux/fs.h>

#include <asm/ftrace.h>


#ifndef ARCH_SUPPORTS_FTRACE_OPS
#define ARCH_SUPPORTS_FTRACE_OPS 0
#endif

#ifdef CONFIG_TRACING
extern void ftrace_boot_snapshot(void);
#else
static inline void ftrace_boot_snapshot(void) { }
#endif

struct ftrace_ops;
struct ftrace_regs;
struct dyn_ftrace;

char *arch_ftrace_match_adjust(char *str, const char *search);

#ifdef CONFIG_HAVE_FUNCTION_GRAPH_RETVAL
struct fgraph_ret_regs;
unsigned long ftrace_return_to_handler(struct fgraph_ret_regs *ret_regs);
#else
unsigned long ftrace_return_to_handler(unsigned long frame_pointer);
#endif

#ifdef CONFIG_FUNCTION_TRACER

#if !ARCH_SUPPORTS_FTRACE_OPS
# define FTRACE_FORCE_LIST_FUNC 1
void arch_ftrace_ops_list_func(unsigned long ip, unsigned long parent_ip);
#else
# define FTRACE_FORCE_LIST_FUNC 0
void arch_ftrace_ops_list_func(unsigned long ip, unsigned long parent_ip,
			       struct ftrace_ops *op, struct ftrace_regs *fregs);
#endif
extern const struct ftrace_ops ftrace_nop_ops;
extern const struct ftrace_ops ftrace_list_ops;
struct ftrace_ops *ftrace_find_unique_ops(struct dyn_ftrace *rec);
#endif 


#ifdef CONFIG_TRACING
void trace_init(void);
void early_trace_init(void);
#else
static inline void trace_init(void) { }
static inline void early_trace_init(void) { }
#endif

struct module;
struct ftrace_hash;
struct ftrace_direct_func;

#if defined(CONFIG_FUNCTION_TRACER) && defined(CONFIG_MODULES) && \
	defined(CONFIG_DYNAMIC_FTRACE)
const char *
ftrace_mod_address_lookup(unsigned long addr, unsigned long *size,
		   unsigned long *off, char **modname, char *sym);
#else
static inline const char *
ftrace_mod_address_lookup(unsigned long addr, unsigned long *size,
		   unsigned long *off, char **modname, char *sym)
{
	return NULL;
}
#endif

#if defined(CONFIG_FUNCTION_TRACER) && defined(CONFIG_DYNAMIC_FTRACE)
int ftrace_mod_get_kallsym(unsigned int symnum, unsigned long *value,
			   char *type, char *name,
			   char *module_name, int *exported);
#else
static inline int ftrace_mod_get_kallsym(unsigned int symnum, unsigned long *value,
					 char *type, char *name,
					 char *module_name, int *exported)
{
	return -1;
}
#endif

#ifdef CONFIG_FUNCTION_TRACER

extern int ftrace_enabled;

#ifndef CONFIG_HAVE_DYNAMIC_FTRACE_WITH_ARGS

struct ftrace_regs {
	struct pt_regs		regs;
};
#define arch_ftrace_get_regs(fregs) (&(fregs)->regs)


#define ftrace_regs_set_instruction_pointer(fregs, ip) do { } while (0)
#endif 

static __always_inline struct pt_regs *ftrace_get_regs(struct ftrace_regs *fregs)
{
	if (!fregs)
		return NULL;

	return arch_ftrace_get_regs(fregs);
}


static __always_inline bool ftrace_regs_has_args(struct ftrace_regs *fregs)
{
	if (IS_ENABLED(CONFIG_HAVE_DYNAMIC_FTRACE_WITH_ARGS))
		return true;

	return ftrace_get_regs(fregs) != NULL;
}

#ifndef CONFIG_HAVE_DYNAMIC_FTRACE_WITH_ARGS
#define ftrace_regs_get_instruction_pointer(fregs) \
	instruction_pointer(ftrace_get_regs(fregs))
#define ftrace_regs_get_argument(fregs, n) \
	regs_get_kernel_argument(ftrace_get_regs(fregs), n)
#define ftrace_regs_get_stack_pointer(fregs) \
	kernel_stack_pointer(ftrace_get_regs(fregs))
#define ftrace_regs_return_value(fregs) \
	regs_return_value(ftrace_get_regs(fregs))
#define ftrace_regs_set_return_value(fregs, ret) \
	regs_set_return_value(ftrace_get_regs(fregs), ret)
#define ftrace_override_function_with_return(fregs) \
	override_function_with_return(ftrace_get_regs(fregs))
#define ftrace_regs_query_register_offset(name) \
	regs_query_register_offset(name)
#endif

typedef void (*ftrace_func_t)(unsigned long ip, unsigned long parent_ip,
			      struct ftrace_ops *op, struct ftrace_regs *fregs);

ftrace_func_t ftrace_ops_get_func(struct ftrace_ops *ops);


enum {
	FTRACE_OPS_FL_ENABLED			= BIT(0),
	FTRACE_OPS_FL_DYNAMIC			= BIT(1),
	FTRACE_OPS_FL_SAVE_REGS			= BIT(2),
	FTRACE_OPS_FL_SAVE_REGS_IF_SUPPORTED	= BIT(3),
	FTRACE_OPS_FL_RECURSION			= BIT(4),
	FTRACE_OPS_FL_STUB			= BIT(5),
	FTRACE_OPS_FL_INITIALIZED		= BIT(6),
	FTRACE_OPS_FL_DELETED			= BIT(7),
	FTRACE_OPS_FL_ADDING			= BIT(8),
	FTRACE_OPS_FL_REMOVING			= BIT(9),
	FTRACE_OPS_FL_MODIFYING			= BIT(10),
	FTRACE_OPS_FL_ALLOC_TRAMP		= BIT(11),
	FTRACE_OPS_FL_IPMODIFY			= BIT(12),
	FTRACE_OPS_FL_PID			= BIT(13),
	FTRACE_OPS_FL_RCU			= BIT(14),
	FTRACE_OPS_FL_TRACE_ARRAY		= BIT(15),
	FTRACE_OPS_FL_PERMANENT                 = BIT(16),
	FTRACE_OPS_FL_DIRECT			= BIT(17),
};

#ifndef CONFIG_DYNAMIC_FTRACE_WITH_ARGS
#define FTRACE_OPS_FL_SAVE_ARGS                        FTRACE_OPS_FL_SAVE_REGS
#else
#define FTRACE_OPS_FL_SAVE_ARGS                        0
#endif


enum ftrace_ops_cmd {
	FTRACE_OPS_CMD_ENABLE_SHARE_IPMODIFY_SELF,
	FTRACE_OPS_CMD_ENABLE_SHARE_IPMODIFY_PEER,
	FTRACE_OPS_CMD_DISABLE_SHARE_IPMODIFY_PEER,
};


typedef int (*ftrace_ops_func_t)(struct ftrace_ops *op, enum ftrace_ops_cmd cmd);

#ifdef CONFIG_DYNAMIC_FTRACE

struct ftrace_ops_hash {
	struct ftrace_hash __rcu	*notrace_hash;
	struct ftrace_hash __rcu	*filter_hash;
	struct mutex			regex_lock;
};

void ftrace_free_init_mem(void);
void ftrace_free_mem(struct module *mod, void *start, void *end);
#else
static inline void ftrace_free_init_mem(void)
{
	ftrace_boot_snapshot();
}
static inline void ftrace_free_mem(struct module *mod, void *start, void *end) { }
#endif


struct ftrace_ops {
	ftrace_func_t			func;
	struct ftrace_ops __rcu		*next;
	unsigned long			flags;
	void				*private;
	ftrace_func_t			saved_func;
#ifdef CONFIG_DYNAMIC_FTRACE
	struct ftrace_ops_hash		local_hash;
	struct ftrace_ops_hash		*func_hash;
	struct ftrace_ops_hash		old_hash;
	unsigned long			trampoline;
	unsigned long			trampoline_size;
	struct list_head		list;
	ftrace_ops_func_t		ops_func;
#ifdef CONFIG_DYNAMIC_FTRACE_WITH_DIRECT_CALLS
	unsigned long			direct_call;
#endif
#endif
};

extern struct ftrace_ops __rcu *ftrace_ops_list;
extern struct ftrace_ops ftrace_list_end;


#define do_for_each_ftrace_op(op, list)			\
	op = rcu_dereference_raw_check(list);			\
	do


#define while_for_each_ftrace_op(op)				\
	while (likely(op = rcu_dereference_raw_check((op)->next)) &&	\
	       unlikely((op) != &ftrace_list_end))


enum ftrace_tracing_type_t {
	FTRACE_TYPE_ENTER = 0, 
	FTRACE_TYPE_RETURN,	
};


extern enum ftrace_tracing_type_t ftrace_tracing_type;


int register_ftrace_function(struct ftrace_ops *ops);
int unregister_ftrace_function(struct ftrace_ops *ops);

extern void ftrace_stub(unsigned long a0, unsigned long a1,
			struct ftrace_ops *op, struct ftrace_regs *fregs);


int ftrace_lookup_symbols(const char **sorted_syms, size_t cnt, unsigned long *addrs);
#else 

#define register_ftrace_function(ops) ({ 0; })
#define unregister_ftrace_function(ops) ({ 0; })
static inline void ftrace_kill(void) { }
static inline void ftrace_free_init_mem(void) { }
static inline void ftrace_free_mem(struct module *mod, void *start, void *end) { }
static inline int ftrace_lookup_symbols(const char **sorted_syms, size_t cnt, unsigned long *addrs)
{
	return -EOPNOTSUPP;
}
#endif 

struct ftrace_func_entry {
	struct hlist_node hlist;
	unsigned long ip;
	unsigned long direct; 
};

#ifdef CONFIG_DYNAMIC_FTRACE_WITH_DIRECT_CALLS
extern int ftrace_direct_func_count;
unsigned long ftrace_find_rec_direct(unsigned long ip);
int register_ftrace_direct(struct ftrace_ops *ops, unsigned long addr);
int unregister_ftrace_direct(struct ftrace_ops *ops, unsigned long addr,
			     bool free_filters);
int modify_ftrace_direct(struct ftrace_ops *ops, unsigned long addr);
int modify_ftrace_direct_nolock(struct ftrace_ops *ops, unsigned long addr);

void ftrace_stub_direct_tramp(void);

#else
struct ftrace_ops;
# define ftrace_direct_func_count 0
static inline unsigned long ftrace_find_rec_direct(unsigned long ip)
{
	return 0;
}
static inline int register_ftrace_direct(struct ftrace_ops *ops, unsigned long addr)
{
	return -ENODEV;
}
static inline int unregister_ftrace_direct(struct ftrace_ops *ops, unsigned long addr,
					   bool free_filters)
{
	return -ENODEV;
}
static inline int modify_ftrace_direct(struct ftrace_ops *ops, unsigned long addr)
{
	return -ENODEV;
}
static inline int modify_ftrace_direct_nolock(struct ftrace_ops *ops, unsigned long addr)
{
	return -ENODEV;
}


static inline void arch_ftrace_set_direct_caller(struct ftrace_regs *fregs,
						 unsigned long addr) { }
#endif 

#ifdef CONFIG_STACK_TRACER

extern int stack_tracer_enabled;

int stack_trace_sysctl(struct ctl_table *table, int write, void *buffer,
		       size_t *lenp, loff_t *ppos);


DECLARE_PER_CPU(int, disable_stack_tracer);


static inline void stack_tracer_disable(void)
{
	
	if (IS_ENABLED(CONFIG_DEBUG_PREEMPT))
		WARN_ON_ONCE(!preempt_count() || !irqs_disabled());
	this_cpu_inc(disable_stack_tracer);
}


static inline void stack_tracer_enable(void)
{
	if (IS_ENABLED(CONFIG_DEBUG_PREEMPT))
		WARN_ON_ONCE(!preempt_count() || !irqs_disabled());
	this_cpu_dec(disable_stack_tracer);
}
#else
static inline void stack_tracer_disable(void) { }
static inline void stack_tracer_enable(void) { }
#endif

#ifdef CONFIG_DYNAMIC_FTRACE

void ftrace_arch_code_modify_prepare(void);
void ftrace_arch_code_modify_post_process(void);

enum ftrace_bug_type {
	FTRACE_BUG_UNKNOWN,
	FTRACE_BUG_INIT,
	FTRACE_BUG_NOP,
	FTRACE_BUG_CALL,
	FTRACE_BUG_UPDATE,
};
extern enum ftrace_bug_type ftrace_bug_type;


extern const void *ftrace_expected;

void ftrace_bug(int err, struct dyn_ftrace *rec);

struct seq_file;

extern int ftrace_text_reserved(const void *start, const void *end);

struct ftrace_ops *ftrace_ops_trampoline(unsigned long addr);

bool is_ftrace_trampoline(unsigned long addr);


enum {
	FTRACE_FL_ENABLED	= (1UL << 31),
	FTRACE_FL_REGS		= (1UL << 30),
	FTRACE_FL_REGS_EN	= (1UL << 29),
	FTRACE_FL_TRAMP		= (1UL << 28),
	FTRACE_FL_TRAMP_EN	= (1UL << 27),
	FTRACE_FL_IPMODIFY	= (1UL << 26),
	FTRACE_FL_DISABLED	= (1UL << 25),
	FTRACE_FL_DIRECT	= (1UL << 24),
	FTRACE_FL_DIRECT_EN	= (1UL << 23),
	FTRACE_FL_CALL_OPS	= (1UL << 22),
	FTRACE_FL_CALL_OPS_EN	= (1UL << 21),
	FTRACE_FL_TOUCHED	= (1UL << 20),
	FTRACE_FL_MODIFIED	= (1UL << 19),
};

#define FTRACE_REF_MAX_SHIFT	19
#define FTRACE_REF_MAX		((1UL << FTRACE_REF_MAX_SHIFT) - 1)

#define ftrace_rec_count(rec)	((rec)->flags & FTRACE_REF_MAX)

struct dyn_ftrace {
	unsigned long		ip; 
	unsigned long		flags;
	struct dyn_arch_ftrace	arch;
};

int ftrace_set_filter_ip(struct ftrace_ops *ops, unsigned long ip,
			 int remove, int reset);
int ftrace_set_filter_ips(struct ftrace_ops *ops, unsigned long *ips,
			  unsigned int cnt, int remove, int reset);
int ftrace_set_filter(struct ftrace_ops *ops, unsigned char *buf,
		       int len, int reset);
int ftrace_set_notrace(struct ftrace_ops *ops, unsigned char *buf,
			int len, int reset);
void ftrace_set_global_filter(unsigned char *buf, int len, int reset);
void ftrace_set_global_notrace(unsigned char *buf, int len, int reset);
void ftrace_free_filter(struct ftrace_ops *ops);
void ftrace_ops_set_global_filter(struct ftrace_ops *ops);

enum {
	FTRACE_UPDATE_CALLS		= (1 << 0),
	FTRACE_DISABLE_CALLS		= (1 << 1),
	FTRACE_UPDATE_TRACE_FUNC	= (1 << 2),
	FTRACE_START_FUNC_RET		= (1 << 3),
	FTRACE_STOP_FUNC_RET		= (1 << 4),
	FTRACE_MAY_SLEEP		= (1 << 5),
};


enum {
	FTRACE_UPDATE_IGNORE,
	FTRACE_UPDATE_MAKE_CALL,
	FTRACE_UPDATE_MODIFY_CALL,
	FTRACE_UPDATE_MAKE_NOP,
};

enum {
	FTRACE_ITER_FILTER	= (1 << 0),
	FTRACE_ITER_NOTRACE	= (1 << 1),
	FTRACE_ITER_PRINTALL	= (1 << 2),
	FTRACE_ITER_DO_PROBES	= (1 << 3),
	FTRACE_ITER_PROBE	= (1 << 4),
	FTRACE_ITER_MOD		= (1 << 5),
	FTRACE_ITER_ENABLED	= (1 << 6),
	FTRACE_ITER_TOUCHED	= (1 << 7),
	FTRACE_ITER_ADDRS	= (1 << 8),
};

void arch_ftrace_update_code(int command);
void arch_ftrace_update_trampoline(struct ftrace_ops *ops);
void *arch_ftrace_trampoline_func(struct ftrace_ops *ops, struct dyn_ftrace *rec);
void arch_ftrace_trampoline_free(struct ftrace_ops *ops);

struct ftrace_rec_iter;

struct ftrace_rec_iter *ftrace_rec_iter_start(void);
struct ftrace_rec_iter *ftrace_rec_iter_next(struct ftrace_rec_iter *iter);
struct dyn_ftrace *ftrace_rec_iter_record(struct ftrace_rec_iter *iter);

#define for_ftrace_rec_iter(iter)		\
	for (iter = ftrace_rec_iter_start();	\
	     iter;				\
	     iter = ftrace_rec_iter_next(iter))


int ftrace_update_record(struct dyn_ftrace *rec, bool enable);
int ftrace_test_record(struct dyn_ftrace *rec, bool enable);
void ftrace_run_stop_machine(int command);
unsigned long ftrace_location(unsigned long ip);
unsigned long ftrace_location_range(unsigned long start, unsigned long end);
unsigned long ftrace_get_addr_new(struct dyn_ftrace *rec);
unsigned long ftrace_get_addr_curr(struct dyn_ftrace *rec);

extern ftrace_func_t ftrace_trace_function;

int ftrace_regex_open(struct ftrace_ops *ops, int flag,
		  struct inode *inode, struct file *file);
ssize_t ftrace_filter_write(struct file *file, const char __user *ubuf,
			    size_t cnt, loff_t *ppos);
ssize_t ftrace_notrace_write(struct file *file, const char __user *ubuf,
			     size_t cnt, loff_t *ppos);
int ftrace_regex_release(struct inode *inode, struct file *file);

void __init
ftrace_set_early_filter(struct ftrace_ops *ops, char *buf, int enable);


extern int ftrace_dyn_arch_init(void);
extern void ftrace_replace_code(int enable);
extern int ftrace_update_ftrace_func(ftrace_func_t func);
extern void ftrace_caller(void);
extern void ftrace_regs_caller(void);
extern void ftrace_call(void);
extern void ftrace_regs_call(void);
extern void mcount_call(void);

void ftrace_modify_all_code(int command);

#ifndef FTRACE_ADDR
#define FTRACE_ADDR ((unsigned long)ftrace_caller)
#endif

#ifndef FTRACE_GRAPH_ADDR
#define FTRACE_GRAPH_ADDR ((unsigned long)ftrace_graph_caller)
#endif

#ifndef FTRACE_REGS_ADDR
#ifdef CONFIG_DYNAMIC_FTRACE_WITH_REGS
# define FTRACE_REGS_ADDR ((unsigned long)ftrace_regs_caller)
#else
# define FTRACE_REGS_ADDR FTRACE_ADDR
#endif
#endif


#ifndef FTRACE_GRAPH_TRAMP_ADDR
#define FTRACE_GRAPH_TRAMP_ADDR ((unsigned long) 0)
#endif

#ifdef CONFIG_FUNCTION_GRAPH_TRACER
extern void ftrace_graph_caller(void);
extern int ftrace_enable_ftrace_graph_caller(void);
extern int ftrace_disable_ftrace_graph_caller(void);
#else
static inline int ftrace_enable_ftrace_graph_caller(void) { return 0; }
static inline int ftrace_disable_ftrace_graph_caller(void) { return 0; }
#endif


extern int ftrace_make_nop(struct module *mod,
			   struct dyn_ftrace *rec, unsigned long addr);



#ifndef ftrace_need_init_nop
#define ftrace_need_init_nop() (!__is_defined(CC_USING_NOP_MCOUNT))
#endif


#ifndef ftrace_init_nop
static inline int ftrace_init_nop(struct module *mod, struct dyn_ftrace *rec)
{
	return ftrace_make_nop(mod, rec, MCOUNT_ADDR);
}
#endif


extern int ftrace_make_call(struct dyn_ftrace *rec, unsigned long addr);

#if defined(CONFIG_DYNAMIC_FTRACE_WITH_REGS) || \
	defined(CONFIG_DYNAMIC_FTRACE_WITH_CALL_OPS)

extern int ftrace_modify_call(struct dyn_ftrace *rec, unsigned long old_addr,
			      unsigned long addr);
#else

static inline int ftrace_modify_call(struct dyn_ftrace *rec, unsigned long old_addr,
				     unsigned long addr)
{
	return -EINVAL;
}
#endif

extern int skip_trace(unsigned long ip);
extern void ftrace_module_init(struct module *mod);
extern void ftrace_module_enable(struct module *mod);
extern void ftrace_release_mod(struct module *mod);
#else 
static inline int skip_trace(unsigned long ip) { return 0; }
static inline void ftrace_module_init(struct module *mod) { }
static inline void ftrace_module_enable(struct module *mod) { }
static inline void ftrace_release_mod(struct module *mod) { }
static inline int ftrace_text_reserved(const void *start, const void *end)
{
	return 0;
}
static inline unsigned long ftrace_location(unsigned long ip)
{
	return 0;
}


#define ftrace_regex_open(ops, flag, inod, file) ({ -ENODEV; })
#define ftrace_set_early_filter(ops, buf, enable) do { } while (0)
#define ftrace_set_filter_ip(ops, ip, remove, reset) ({ -ENODEV; })
#define ftrace_set_filter_ips(ops, ips, cnt, remove, reset) ({ -ENODEV; })
#define ftrace_set_filter(ops, buf, len, reset) ({ -ENODEV; })
#define ftrace_set_notrace(ops, buf, len, reset) ({ -ENODEV; })
#define ftrace_free_filter(ops) do { } while (0)
#define ftrace_ops_set_global_filter(ops) do { } while (0)

static inline ssize_t ftrace_filter_write(struct file *file, const char __user *ubuf,
			    size_t cnt, loff_t *ppos) { return -ENODEV; }
static inline ssize_t ftrace_notrace_write(struct file *file, const char __user *ubuf,
			     size_t cnt, loff_t *ppos) { return -ENODEV; }
static inline int
ftrace_regex_release(struct inode *inode, struct file *file) { return -ENODEV; }

static inline bool is_ftrace_trampoline(unsigned long addr)
{
	return false;
}
#endif 

#ifdef CONFIG_FUNCTION_GRAPH_TRACER
#ifndef ftrace_graph_func
#define ftrace_graph_func ftrace_stub
#define FTRACE_OPS_GRAPH_STUB FTRACE_OPS_FL_STUB
#else
#define FTRACE_OPS_GRAPH_STUB 0
#endif
#endif 


void ftrace_kill(void);

static inline void tracer_disable(void)
{
#ifdef CONFIG_FUNCTION_TRACER
	ftrace_enabled = 0;
#endif
}


static inline int __ftrace_enabled_save(void)
{
#ifdef CONFIG_FUNCTION_TRACER
	int saved_ftrace_enabled = ftrace_enabled;
	ftrace_enabled = 0;
	return saved_ftrace_enabled;
#else
	return 0;
#endif
}

static inline void __ftrace_enabled_restore(int enabled)
{
#ifdef CONFIG_FUNCTION_TRACER
	ftrace_enabled = enabled;
#endif
}


#ifndef ftrace_return_address0
# define ftrace_return_address0 __builtin_return_address(0)
#endif


#ifndef ftrace_return_address
# ifdef CONFIG_FRAME_POINTER
#  define ftrace_return_address(n) __builtin_return_address(n)
# else
#  define ftrace_return_address(n) 0UL
# endif
#endif

#define CALLER_ADDR0 ((unsigned long)ftrace_return_address0)
#define CALLER_ADDR1 ((unsigned long)ftrace_return_address(1))
#define CALLER_ADDR2 ((unsigned long)ftrace_return_address(2))
#define CALLER_ADDR3 ((unsigned long)ftrace_return_address(3))
#define CALLER_ADDR4 ((unsigned long)ftrace_return_address(4))
#define CALLER_ADDR5 ((unsigned long)ftrace_return_address(5))
#define CALLER_ADDR6 ((unsigned long)ftrace_return_address(6))

static __always_inline unsigned long get_lock_parent_ip(void)
{
	unsigned long addr = CALLER_ADDR0;

	if (!in_lock_functions(addr))
		return addr;
	addr = CALLER_ADDR1;
	if (!in_lock_functions(addr))
		return addr;
	return CALLER_ADDR2;
}

#ifdef CONFIG_TRACE_PREEMPT_TOGGLE
  extern void trace_preempt_on(unsigned long a0, unsigned long a1);
  extern void trace_preempt_off(unsigned long a0, unsigned long a1);
#else

# define trace_preempt_on(a0, a1) do { } while (0)
# define trace_preempt_off(a0, a1) do { } while (0)
#endif

#ifdef CONFIG_FTRACE_MCOUNT_RECORD
extern void ftrace_init(void);
#ifdef CC_USING_PATCHABLE_FUNCTION_ENTRY
#define FTRACE_CALLSITE_SECTION	"__patchable_function_entries"
#else
#define FTRACE_CALLSITE_SECTION	"__mcount_loc"
#endif
#else
static inline void ftrace_init(void) { }
#endif


struct ftrace_graph_ent {
	unsigned long func; 
	int depth;
} __packed;


struct ftrace_graph_ret {
	unsigned long func; 
#ifdef CONFIG_FUNCTION_GRAPH_RETVAL
	unsigned long retval;
#endif
	int depth;
	
	unsigned int overrun;
	unsigned long long calltime;
	unsigned long long rettime;
} __packed;


typedef void (*trace_func_graph_ret_t)(struct ftrace_graph_ret *); 
typedef int (*trace_func_graph_ent_t)(struct ftrace_graph_ent *); 

extern int ftrace_graph_entry_stub(struct ftrace_graph_ent *trace);

#ifdef CONFIG_FUNCTION_GRAPH_TRACER

struct fgraph_ops {
	trace_func_graph_ent_t		entryfunc;
	trace_func_graph_ret_t		retfunc;
};


struct ftrace_ret_stack {
	unsigned long ret;
	unsigned long func;
	unsigned long long calltime;
#ifdef CONFIG_FUNCTION_PROFILER
	unsigned long long subtime;
#endif
#ifdef HAVE_FUNCTION_GRAPH_FP_TEST
	unsigned long fp;
#endif
#ifdef HAVE_FUNCTION_GRAPH_RET_ADDR_PTR
	unsigned long *retp;
#endif
};


extern void return_to_handler(void);

extern int
function_graph_enter(unsigned long ret, unsigned long func,
		     unsigned long frame_pointer, unsigned long *retp);

struct ftrace_ret_stack *
ftrace_graph_get_ret_stack(struct task_struct *task, int idx);

unsigned long ftrace_graph_ret_addr(struct task_struct *task, int *idx,
				    unsigned long ret, unsigned long *retp);


#define __notrace_funcgraph		notrace

#define FTRACE_RETFUNC_DEPTH 50
#define FTRACE_RETSTACK_ALLOC_SIZE 32

extern int register_ftrace_graph(struct fgraph_ops *ops);
extern void unregister_ftrace_graph(struct fgraph_ops *ops);


DECLARE_STATIC_KEY_FALSE(kill_ftrace_graph);

static inline bool ftrace_graph_is_dead(void)
{
	return static_branch_unlikely(&kill_ftrace_graph);
}

extern void ftrace_graph_stop(void);


extern trace_func_graph_ret_t ftrace_graph_return;
extern trace_func_graph_ent_t ftrace_graph_entry;

extern void ftrace_graph_init_task(struct task_struct *t);
extern void ftrace_graph_exit_task(struct task_struct *t);
extern void ftrace_graph_init_idle_task(struct task_struct *t, int cpu);

static inline void pause_graph_tracing(void)
{
	atomic_inc(&current->tracing_graph_pause);
}

static inline void unpause_graph_tracing(void)
{
	atomic_dec(&current->tracing_graph_pause);
}
#else 

#define __notrace_funcgraph

static inline void ftrace_graph_init_task(struct task_struct *t) { }
static inline void ftrace_graph_exit_task(struct task_struct *t) { }
static inline void ftrace_graph_init_idle_task(struct task_struct *t, int cpu) { }


#define register_ftrace_graph(ops) ({ -1; })
#define unregister_ftrace_graph(ops) do { } while (0)

static inline unsigned long
ftrace_graph_ret_addr(struct task_struct *task, int *idx, unsigned long ret,
		      unsigned long *retp)
{
	return ret;
}

static inline void pause_graph_tracing(void) { }
static inline void unpause_graph_tracing(void) { }
#endif 

#ifdef CONFIG_TRACING
enum ftrace_dump_mode;

extern enum ftrace_dump_mode ftrace_dump_on_oops;
extern int tracepoint_printk;

extern void disable_trace_on_warning(void);
extern int __disable_trace_on_warning;

int tracepoint_printk_sysctl(struct ctl_table *table, int write,
			     void *buffer, size_t *lenp, loff_t *ppos);

#else 
static inline void  disable_trace_on_warning(void) { }
#endif 

#ifdef CONFIG_FTRACE_SYSCALLS

unsigned long arch_syscall_addr(int nr);

#endif 

#endif 
