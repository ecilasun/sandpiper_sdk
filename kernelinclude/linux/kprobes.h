/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef _LINUX_KPROBES_H
#define _LINUX_KPROBES_H

#include <linux/compiler.h>
#include <linux/linkage.h>
#include <linux/list.h>
#include <linux/notifier.h>
#include <linux/smp.h>
#include <linux/bug.h>
#include <linux/percpu.h>
#include <linux/spinlock.h>
#include <linux/rcupdate.h>
#include <linux/mutex.h>
#include <linux/ftrace.h>
#include <linux/refcount.h>
#include <linux/freelist.h>
#include <linux/rethook.h>
#include <asm/kprobes.h>

#ifdef CONFIG_KPROBES


#define KPROBE_HIT_ACTIVE	0x00000001
#define KPROBE_HIT_SS		0x00000002
#define KPROBE_REENTER		0x00000004
#define KPROBE_HIT_SSDONE	0x00000008

#else 
#include <asm-generic/kprobes.h>
typedef int kprobe_opcode_t;
struct arch_specific_insn {
	int dummy;
};
#endif 

struct kprobe;
struct pt_regs;
struct kretprobe;
struct kretprobe_instance;
typedef int (*kprobe_pre_handler_t) (struct kprobe *, struct pt_regs *);
typedef void (*kprobe_post_handler_t) (struct kprobe *, struct pt_regs *,
				       unsigned long flags);
typedef int (*kretprobe_handler_t) (struct kretprobe_instance *,
				    struct pt_regs *);

struct kprobe {
	struct hlist_node hlist;

	
	struct list_head list;

	
	unsigned long nmissed;

	
	kprobe_opcode_t *addr;

	
	const char *symbol_name;

	
	unsigned int offset;

	
	kprobe_pre_handler_t pre_handler;

	
	kprobe_post_handler_t post_handler;

	
	kprobe_opcode_t opcode;

	
	struct arch_specific_insn ainsn;

	
	u32 flags;
};


#define KPROBE_FLAG_GONE	1 
#define KPROBE_FLAG_DISABLED	2 
#define KPROBE_FLAG_OPTIMIZED	4 
#define KPROBE_FLAG_FTRACE	8 
#define KPROBE_FLAG_ON_FUNC_ENTRY	16 


static inline bool kprobe_gone(struct kprobe *p)
{
	return p->flags & KPROBE_FLAG_GONE;
}


static inline bool kprobe_disabled(struct kprobe *p)
{
	return p->flags & (KPROBE_FLAG_DISABLED | KPROBE_FLAG_GONE);
}


static inline bool kprobe_optimized(struct kprobe *p)
{
	return p->flags & KPROBE_FLAG_OPTIMIZED;
}


static inline bool kprobe_ftrace(struct kprobe *p)
{
	return p->flags & KPROBE_FLAG_FTRACE;
}


struct kretprobe_holder {
	struct kretprobe __rcu *rp;
	refcount_t		ref;
};

struct kretprobe {
	struct kprobe kp;
	kretprobe_handler_t handler;
	kretprobe_handler_t entry_handler;
	int maxactive;
	int nmissed;
	size_t data_size;
#ifdef CONFIG_KRETPROBE_ON_RETHOOK
	struct rethook *rh;
#else
	struct freelist_head freelist;
	struct kretprobe_holder *rph;
#endif
};

#define KRETPROBE_MAX_DATA_SIZE	4096

struct kretprobe_instance {
#ifdef CONFIG_KRETPROBE_ON_RETHOOK
	struct rethook_node node;
#else
	union {
		struct freelist_node freelist;
		struct rcu_head rcu;
	};
	struct llist_node llist;
	struct kretprobe_holder *rph;
	kprobe_opcode_t *ret_addr;
	void *fp;
#endif
	char data[];
};

struct kretprobe_blackpoint {
	const char *name;
	void *addr;
};

struct kprobe_blacklist_entry {
	struct list_head list;
	unsigned long start_addr;
	unsigned long end_addr;
};

#ifdef CONFIG_KPROBES
DECLARE_PER_CPU(struct kprobe *, current_kprobe);
DECLARE_PER_CPU(struct kprobe_ctlblk, kprobe_ctlblk);

extern void kprobe_busy_begin(void);
extern void kprobe_busy_end(void);

#ifdef CONFIG_KRETPROBES

extern int arch_trampoline_kprobe(struct kprobe *p);

#ifdef CONFIG_KRETPROBE_ON_RETHOOK
static nokprobe_inline struct kretprobe *get_kretprobe(struct kretprobe_instance *ri)
{
	
	return (struct kretprobe *)ri->node.rethook->data;
}
static nokprobe_inline unsigned long get_kretprobe_retaddr(struct kretprobe_instance *ri)
{
	return ri->node.ret_addr;
}
#else
extern void arch_prepare_kretprobe(struct kretprobe_instance *ri,
				   struct pt_regs *regs);
void arch_kretprobe_fixup_return(struct pt_regs *regs,
				 kprobe_opcode_t *correct_ret_addr);

void __kretprobe_trampoline(void);

static nokprobe_inline void *kretprobe_trampoline_addr(void)
{
	return dereference_kernel_function_descriptor(__kretprobe_trampoline);
}


unsigned long __kretprobe_trampoline_handler(struct pt_regs *regs,
					     void *frame_pointer);

static nokprobe_inline
unsigned long kretprobe_trampoline_handler(struct pt_regs *regs,
					   void *frame_pointer)
{
	unsigned long ret;
	
	kprobe_busy_begin();
	ret = __kretprobe_trampoline_handler(regs, frame_pointer);
	kprobe_busy_end();

	return ret;
}

static nokprobe_inline struct kretprobe *get_kretprobe(struct kretprobe_instance *ri)
{
	return rcu_dereference_check(ri->rph->rp, rcu_read_lock_any_held());
}

static nokprobe_inline unsigned long get_kretprobe_retaddr(struct kretprobe_instance *ri)
{
	return (unsigned long)ri->ret_addr;
}
#endif 

#else 
static inline void arch_prepare_kretprobe(struct kretprobe *rp,
					struct pt_regs *regs)
{
}
static inline int arch_trampoline_kprobe(struct kprobe *p)
{
	return 0;
}
#endif 


extern unsigned long __start_kprobe_blacklist[];
extern unsigned long __stop_kprobe_blacklist[];

extern struct kretprobe_blackpoint kretprobe_blacklist[];

#ifdef CONFIG_KPROBES_SANITY_TEST
extern int init_test_probes(void);
#else 
static inline int init_test_probes(void)
{
	return 0;
}
#endif 

extern int arch_prepare_kprobe(struct kprobe *p);
extern void arch_arm_kprobe(struct kprobe *p);
extern void arch_disarm_kprobe(struct kprobe *p);
extern int arch_init_kprobes(void);
extern void kprobes_inc_nmissed_count(struct kprobe *p);
extern bool arch_within_kprobe_blacklist(unsigned long addr);
extern int arch_populate_kprobe_blacklist(void);
extern int kprobe_on_func_entry(kprobe_opcode_t *addr, const char *sym, unsigned long offset);

extern bool within_kprobe_blacklist(unsigned long addr);
extern int kprobe_add_ksym_blacklist(unsigned long entry);
extern int kprobe_add_area_blacklist(unsigned long start, unsigned long end);

struct kprobe_insn_cache {
	struct mutex mutex;
	void *(*alloc)(void);	
	void (*free)(void *);	
	const char *sym;	
	struct list_head pages; 
	size_t insn_size;	
	int nr_garbage;
};

#ifdef __ARCH_WANT_KPROBES_INSN_SLOT
extern kprobe_opcode_t *__get_insn_slot(struct kprobe_insn_cache *c);
extern void __free_insn_slot(struct kprobe_insn_cache *c,
			     kprobe_opcode_t *slot, int dirty);

extern bool __is_insn_slot_addr(struct kprobe_insn_cache *c,
				unsigned long addr);

#define DEFINE_INSN_CACHE_OPS(__name)					\
extern struct kprobe_insn_cache kprobe_##__name##_slots;		\
									\
static inline kprobe_opcode_t *get_##__name##_slot(void)		\
{									\
	return __get_insn_slot(&kprobe_##__name##_slots);		\
}									\
									\
static inline void free_##__name##_slot(kprobe_opcode_t *slot, int dirty)\
{									\
	__free_insn_slot(&kprobe_##__name##_slots, slot, dirty);	\
}									\
									\
static inline bool is_kprobe_##__name##_slot(unsigned long addr)	\
{									\
	return __is_insn_slot_addr(&kprobe_##__name##_slots, addr);	\
}
#define KPROBE_INSN_PAGE_SYM		"kprobe_insn_page"
#define KPROBE_OPTINSN_PAGE_SYM		"kprobe_optinsn_page"
int kprobe_cache_get_kallsym(struct kprobe_insn_cache *c, unsigned int *symnum,
			     unsigned long *value, char *type, char *sym);
#else 
#define DEFINE_INSN_CACHE_OPS(__name)					\
static inline bool is_kprobe_##__name##_slot(unsigned long addr)	\
{									\
	return 0;							\
}
#endif

DEFINE_INSN_CACHE_OPS(insn);

#ifdef CONFIG_OPTPROBES

struct optimized_kprobe {
	struct kprobe kp;
	struct list_head list;	
	struct arch_optimized_insn optinsn;
};


extern int arch_prepared_optinsn(struct arch_optimized_insn *optinsn);
extern int arch_check_optimized_kprobe(struct optimized_kprobe *op);
extern int arch_prepare_optimized_kprobe(struct optimized_kprobe *op,
					 struct kprobe *orig);
extern void arch_remove_optimized_kprobe(struct optimized_kprobe *op);
extern void arch_optimize_kprobes(struct list_head *oplist);
extern void arch_unoptimize_kprobes(struct list_head *oplist,
				    struct list_head *done_list);
extern void arch_unoptimize_kprobe(struct optimized_kprobe *op);
extern int arch_within_optimized_kprobe(struct optimized_kprobe *op,
					kprobe_opcode_t *addr);

extern void opt_pre_handler(struct kprobe *p, struct pt_regs *regs);

DEFINE_INSN_CACHE_OPS(optinsn);

extern void wait_for_kprobe_optimizer(void);
bool optprobe_queued_unopt(struct optimized_kprobe *op);
bool kprobe_disarmed(struct kprobe *p);
#else 
static inline void wait_for_kprobe_optimizer(void) { }
#endif 

#ifdef CONFIG_KPROBES_ON_FTRACE
extern void kprobe_ftrace_handler(unsigned long ip, unsigned long parent_ip,
				  struct ftrace_ops *ops, struct ftrace_regs *fregs);
extern int arch_prepare_kprobe_ftrace(struct kprobe *p);

extern bool kprobe_ftrace_disabled __read_mostly;
extern void kprobe_ftrace_kill(void);
#else
static inline int arch_prepare_kprobe_ftrace(struct kprobe *p)
{
	return -EINVAL;
}
static inline void kprobe_ftrace_kill(void) {}
#endif 


struct kprobe *get_kprobe(void *addr);


static inline struct kprobe *kprobe_running(void)
{
	return __this_cpu_read(current_kprobe);
}

static inline void reset_current_kprobe(void)
{
	__this_cpu_write(current_kprobe, NULL);
}

static inline struct kprobe_ctlblk *get_kprobe_ctlblk(void)
{
	return this_cpu_ptr(&kprobe_ctlblk);
}

kprobe_opcode_t *kprobe_lookup_name(const char *name, unsigned int offset);
kprobe_opcode_t *arch_adjust_kprobe_addr(unsigned long addr, unsigned long offset, bool *on_func_entry);

int register_kprobe(struct kprobe *p);
void unregister_kprobe(struct kprobe *p);
int register_kprobes(struct kprobe **kps, int num);
void unregister_kprobes(struct kprobe **kps, int num);

int register_kretprobe(struct kretprobe *rp);
void unregister_kretprobe(struct kretprobe *rp);
int register_kretprobes(struct kretprobe **rps, int num);
void unregister_kretprobes(struct kretprobe **rps, int num);

#if defined(CONFIG_KRETPROBE_ON_RETHOOK) || !defined(CONFIG_KRETPROBES)
#define kprobe_flush_task(tk)	do {} while (0)
#else
void kprobe_flush_task(struct task_struct *tk);
#endif

void kprobe_free_init_mem(void);

int disable_kprobe(struct kprobe *kp);
int enable_kprobe(struct kprobe *kp);

void dump_kprobe(struct kprobe *kp);

void *alloc_insn_page(void);

void *alloc_optinsn_page(void);
void free_optinsn_page(void *page);

int kprobe_get_kallsym(unsigned int symnum, unsigned long *value, char *type,
		       char *sym);

int arch_kprobe_get_kallsym(unsigned int *symnum, unsigned long *value,
			    char *type, char *sym);
#else 

static inline int kprobe_fault_handler(struct pt_regs *regs, int trapnr)
{
	return 0;
}
static inline struct kprobe *get_kprobe(void *addr)
{
	return NULL;
}
static inline struct kprobe *kprobe_running(void)
{
	return NULL;
}
#define kprobe_busy_begin()	do {} while (0)
#define kprobe_busy_end()	do {} while (0)

static inline int register_kprobe(struct kprobe *p)
{
	return -EOPNOTSUPP;
}
static inline int register_kprobes(struct kprobe **kps, int num)
{
	return -EOPNOTSUPP;
}
static inline void unregister_kprobe(struct kprobe *p)
{
}
static inline void unregister_kprobes(struct kprobe **kps, int num)
{
}
static inline int register_kretprobe(struct kretprobe *rp)
{
	return -EOPNOTSUPP;
}
static inline int register_kretprobes(struct kretprobe **rps, int num)
{
	return -EOPNOTSUPP;
}
static inline void unregister_kretprobe(struct kretprobe *rp)
{
}
static inline void unregister_kretprobes(struct kretprobe **rps, int num)
{
}
static inline void kprobe_flush_task(struct task_struct *tk)
{
}
static inline void kprobe_free_init_mem(void)
{
}
static inline void kprobe_ftrace_kill(void)
{
}
static inline int disable_kprobe(struct kprobe *kp)
{
	return -EOPNOTSUPP;
}
static inline int enable_kprobe(struct kprobe *kp)
{
	return -EOPNOTSUPP;
}

static inline bool within_kprobe_blacklist(unsigned long addr)
{
	return true;
}
static inline int kprobe_get_kallsym(unsigned int symnum, unsigned long *value,
				     char *type, char *sym)
{
	return -ERANGE;
}
#endif 

static inline int disable_kretprobe(struct kretprobe *rp)
{
	return disable_kprobe(&rp->kp);
}
static inline int enable_kretprobe(struct kretprobe *rp)
{
	return enable_kprobe(&rp->kp);
}

#ifndef CONFIG_KPROBES
static inline bool is_kprobe_insn_slot(unsigned long addr)
{
	return false;
}
#endif 

#ifndef CONFIG_OPTPROBES
static inline bool is_kprobe_optinsn_slot(unsigned long addr)
{
	return false;
}
#endif 

#ifdef CONFIG_KRETPROBES
#ifdef CONFIG_KRETPROBE_ON_RETHOOK
static nokprobe_inline bool is_kretprobe_trampoline(unsigned long addr)
{
	return is_rethook_trampoline(addr);
}

static nokprobe_inline
unsigned long kretprobe_find_ret_addr(struct task_struct *tsk, void *fp,
				      struct llist_node **cur)
{
	return rethook_find_ret_addr(tsk, (unsigned long)fp, cur);
}
#else
static nokprobe_inline bool is_kretprobe_trampoline(unsigned long addr)
{
	return (void *)addr == kretprobe_trampoline_addr();
}

unsigned long kretprobe_find_ret_addr(struct task_struct *tsk, void *fp,
				      struct llist_node **cur);
#endif
#else
static nokprobe_inline bool is_kretprobe_trampoline(unsigned long addr)
{
	return false;
}

static nokprobe_inline
unsigned long kretprobe_find_ret_addr(struct task_struct *tsk, void *fp,
				      struct llist_node **cur)
{
	return 0;
}
#endif


static nokprobe_inline bool kprobe_page_fault(struct pt_regs *regs,
					      unsigned int trap)
{
	if (!IS_ENABLED(CONFIG_KPROBES))
		return false;
	if (user_mode(regs))
		return false;
	
	if (preemptible())
		return false;
	if (!kprobe_running())
		return false;
	return kprobe_fault_handler(regs, trap);
}

#endif 
