/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef __KVM_HOST_H
#define __KVM_HOST_H


#include <linux/types.h>
#include <linux/hardirq.h>
#include <linux/list.h>
#include <linux/mutex.h>
#include <linux/spinlock.h>
#include <linux/signal.h>
#include <linux/sched.h>
#include <linux/sched/stat.h>
#include <linux/bug.h>
#include <linux/minmax.h>
#include <linux/mm.h>
#include <linux/mmu_notifier.h>
#include <linux/preempt.h>
#include <linux/msi.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/rcupdate.h>
#include <linux/ratelimit.h>
#include <linux/err.h>
#include <linux/irqflags.h>
#include <linux/context_tracking.h>
#include <linux/irqbypass.h>
#include <linux/rcuwait.h>
#include <linux/refcount.h>
#include <linux/nospec.h>
#include <linux/notifier.h>
#include <linux/ftrace.h>
#include <linux/hashtable.h>
#include <linux/instrumentation.h>
#include <linux/interval_tree.h>
#include <linux/rbtree.h>
#include <linux/xarray.h>
#include <asm/signal.h>

#include <linux/kvm.h>
#include <linux/kvm_para.h>

#include <linux/kvm_types.h>

#include <asm/kvm_host.h>
#include <linux/kvm_dirty_ring.h>

#ifndef KVM_MAX_VCPU_IDS
#define KVM_MAX_VCPU_IDS KVM_MAX_VCPUS
#endif


#define KVM_MEMSLOT_INVALID	(1UL << 16)


#define KVM_MEMSLOT_GEN_UPDATE_IN_PROGRESS	BIT_ULL(63)


#define KVM_MAX_MMIO_FRAGMENTS	2

#ifndef KVM_ADDRESS_SPACE_NUM
#define KVM_ADDRESS_SPACE_NUM	1
#endif


#define KVM_PFN_ERR_MASK	(0x7ffULL << 52)
#define KVM_PFN_ERR_NOSLOT_MASK	(0xfffULL << 52)
#define KVM_PFN_NOSLOT		(0x1ULL << 63)

#define KVM_PFN_ERR_FAULT	(KVM_PFN_ERR_MASK)
#define KVM_PFN_ERR_HWPOISON	(KVM_PFN_ERR_MASK + 1)
#define KVM_PFN_ERR_RO_FAULT	(KVM_PFN_ERR_MASK + 2)
#define KVM_PFN_ERR_SIGPENDING	(KVM_PFN_ERR_MASK + 3)


static inline bool is_error_pfn(kvm_pfn_t pfn)
{
	return !!(pfn & KVM_PFN_ERR_MASK);
}


static inline bool is_sigpending_pfn(kvm_pfn_t pfn)
{
	return pfn == KVM_PFN_ERR_SIGPENDING;
}


static inline bool is_error_noslot_pfn(kvm_pfn_t pfn)
{
	return !!(pfn & KVM_PFN_ERR_NOSLOT_MASK);
}


static inline bool is_noslot_pfn(kvm_pfn_t pfn)
{
	return pfn == KVM_PFN_NOSLOT;
}


#ifndef KVM_HVA_ERR_BAD

#define KVM_HVA_ERR_BAD		(PAGE_OFFSET)
#define KVM_HVA_ERR_RO_BAD	(PAGE_OFFSET + PAGE_SIZE)

static inline bool kvm_is_error_hva(unsigned long addr)
{
	return addr >= PAGE_OFFSET;
}

#endif

#define KVM_ERR_PTR_BAD_PAGE	(ERR_PTR(-ENOENT))

static inline bool is_error_page(struct page *page)
{
	return IS_ERR(page);
}

#define KVM_REQUEST_MASK           GENMASK(7,0)
#define KVM_REQUEST_NO_WAKEUP      BIT(8)
#define KVM_REQUEST_WAIT           BIT(9)
#define KVM_REQUEST_NO_ACTION      BIT(10)

#define KVM_REQ_TLB_FLUSH		(0 | KVM_REQUEST_WAIT | KVM_REQUEST_NO_WAKEUP)
#define KVM_REQ_VM_DEAD			(1 | KVM_REQUEST_WAIT | KVM_REQUEST_NO_WAKEUP)
#define KVM_REQ_UNBLOCK			2
#define KVM_REQ_DIRTY_RING_SOFT_FULL	3
#define KVM_REQUEST_ARCH_BASE		8


#define KVM_REQ_OUTSIDE_GUEST_MODE	(KVM_REQUEST_NO_ACTION | KVM_REQUEST_WAIT | KVM_REQUEST_NO_WAKEUP)

#define KVM_ARCH_REQ_FLAGS(nr, flags) ({ \
	BUILD_BUG_ON((unsigned)(nr) >= (sizeof_field(struct kvm_vcpu, requests) * 8) - KVM_REQUEST_ARCH_BASE); \
	(unsigned)(((nr) + KVM_REQUEST_ARCH_BASE) | (flags)); \
})
#define KVM_ARCH_REQ(nr)           KVM_ARCH_REQ_FLAGS(nr, 0)

bool kvm_make_vcpus_request_mask(struct kvm *kvm, unsigned int req,
				 unsigned long *vcpu_bitmap);
bool kvm_make_all_cpus_request(struct kvm *kvm, unsigned int req);
bool kvm_make_all_cpus_request_except(struct kvm *kvm, unsigned int req,
				      struct kvm_vcpu *except);

#define KVM_USERSPACE_IRQ_SOURCE_ID		0
#define KVM_IRQFD_RESAMPLE_IRQ_SOURCE_ID	1

extern struct mutex kvm_lock;
extern struct list_head vm_list;

struct kvm_io_range {
	gpa_t addr;
	int len;
	struct kvm_io_device *dev;
};

#define NR_IOBUS_DEVS 1000

struct kvm_io_bus {
	int dev_count;
	int ioeventfd_count;
	struct kvm_io_range range[];
};

enum kvm_bus {
	KVM_MMIO_BUS,
	KVM_PIO_BUS,
	KVM_VIRTIO_CCW_NOTIFY_BUS,
	KVM_FAST_MMIO_BUS,
	KVM_NR_BUSES
};

int kvm_io_bus_write(struct kvm_vcpu *vcpu, enum kvm_bus bus_idx, gpa_t addr,
		     int len, const void *val);
int kvm_io_bus_write_cookie(struct kvm_vcpu *vcpu, enum kvm_bus bus_idx,
			    gpa_t addr, int len, const void *val, long cookie);
int kvm_io_bus_read(struct kvm_vcpu *vcpu, enum kvm_bus bus_idx, gpa_t addr,
		    int len, void *val);
int kvm_io_bus_register_dev(struct kvm *kvm, enum kvm_bus bus_idx, gpa_t addr,
			    int len, struct kvm_io_device *dev);
int kvm_io_bus_unregister_dev(struct kvm *kvm, enum kvm_bus bus_idx,
			      struct kvm_io_device *dev);
struct kvm_io_device *kvm_io_bus_get_dev(struct kvm *kvm, enum kvm_bus bus_idx,
					 gpa_t addr);

#ifdef CONFIG_KVM_ASYNC_PF
struct kvm_async_pf {
	struct work_struct work;
	struct list_head link;
	struct list_head queue;
	struct kvm_vcpu *vcpu;
	struct mm_struct *mm;
	gpa_t cr2_or_gpa;
	unsigned long addr;
	struct kvm_arch_async_pf arch;
	bool   wakeup_all;
	bool notpresent_injected;
};

void kvm_clear_async_pf_completion_queue(struct kvm_vcpu *vcpu);
void kvm_check_async_pf_completion(struct kvm_vcpu *vcpu);
bool kvm_setup_async_pf(struct kvm_vcpu *vcpu, gpa_t cr2_or_gpa,
			unsigned long hva, struct kvm_arch_async_pf *arch);
int kvm_async_pf_wakeup_all(struct kvm_vcpu *vcpu);
#endif

#ifdef KVM_ARCH_WANT_MMU_NOTIFIER
union kvm_mmu_notifier_arg {
	pte_t pte;
};

struct kvm_gfn_range {
	struct kvm_memory_slot *slot;
	gfn_t start;
	gfn_t end;
	union kvm_mmu_notifier_arg arg;
	bool may_block;
};
bool kvm_unmap_gfn_range(struct kvm *kvm, struct kvm_gfn_range *range);
bool kvm_age_gfn(struct kvm *kvm, struct kvm_gfn_range *range);
bool kvm_test_age_gfn(struct kvm *kvm, struct kvm_gfn_range *range);
bool kvm_set_spte_gfn(struct kvm *kvm, struct kvm_gfn_range *range);
#endif

enum {
	OUTSIDE_GUEST_MODE,
	IN_GUEST_MODE,
	EXITING_GUEST_MODE,
	READING_SHADOW_PAGE_TABLES,
};

#define KVM_UNMAPPED_PAGE	((void *) 0x500 + POISON_POINTER_DELTA)

struct kvm_host_map {
	
	struct page *page;
	void *hva;
	kvm_pfn_t pfn;
	kvm_pfn_t gfn;
};


static inline bool kvm_vcpu_mapped(struct kvm_host_map *map)
{
	return !!map->hva;
}

static inline bool kvm_vcpu_can_poll(ktime_t cur, ktime_t stop)
{
	return single_task_running() && !need_resched() && ktime_before(cur, stop);
}


struct kvm_mmio_fragment {
	gpa_t gpa;
	void *data;
	unsigned len;
};

struct kvm_vcpu {
	struct kvm *kvm;
#ifdef CONFIG_PREEMPT_NOTIFIERS
	struct preempt_notifier preempt_notifier;
#endif
	int cpu;
	int vcpu_id; 
	int vcpu_idx; 
	int ____srcu_idx; 
#ifdef CONFIG_PROVE_RCU
	int srcu_depth;
#endif
	int mode;
	u64 requests;
	unsigned long guest_debug;

	struct mutex mutex;
	struct kvm_run *run;

#ifndef __KVM_HAVE_ARCH_WQP
	struct rcuwait wait;
#endif
	struct pid __rcu *pid;
	int sigset_active;
	sigset_t sigset;
	unsigned int halt_poll_ns;
	bool valid_wakeup;

#ifdef CONFIG_HAS_IOMEM
	int mmio_needed;
	int mmio_read_completed;
	int mmio_is_write;
	int mmio_cur_fragment;
	int mmio_nr_fragments;
	struct kvm_mmio_fragment mmio_fragments[KVM_MAX_MMIO_FRAGMENTS];
#endif

#ifdef CONFIG_KVM_ASYNC_PF
	struct {
		u32 queued;
		struct list_head queue;
		struct list_head done;
		spinlock_t lock;
	} async_pf;
#endif

#ifdef CONFIG_HAVE_KVM_CPU_RELAX_INTERCEPT
	
	struct {
		bool in_spin_loop;
		bool dy_eligible;
	} spin_loop;
#endif
	bool preempted;
	bool ready;
	struct kvm_vcpu_arch arch;
	struct kvm_vcpu_stat stat;
	char stats_id[KVM_STATS_NAME_SIZE];
	struct kvm_dirty_ring dirty_ring;

	
	struct kvm_memory_slot *last_used_slot;
	u64 last_used_slot_gen;
};


static __always_inline void guest_timing_enter_irqoff(void)
{
	
	instrumentation_begin();
	vtime_account_guest_enter();
	instrumentation_end();
}


static __always_inline void guest_context_enter_irqoff(void)
{
	
	if (!context_tracking_guest_enter()) {
		instrumentation_begin();
		rcu_virt_note_context_switch();
		instrumentation_end();
	}
}


static __always_inline void guest_enter_irqoff(void)
{
	guest_timing_enter_irqoff();
	guest_context_enter_irqoff();
}


static __always_inline void guest_state_enter_irqoff(void)
{
	instrumentation_begin();
	trace_hardirqs_on_prepare();
	lockdep_hardirqs_on_prepare();
	instrumentation_end();

	guest_context_enter_irqoff();
	lockdep_hardirqs_on(CALLER_ADDR0);
}


static __always_inline void guest_context_exit_irqoff(void)
{
	context_tracking_guest_exit();
}


static __always_inline void guest_timing_exit_irqoff(void)
{
	instrumentation_begin();
	
	vtime_account_guest_exit();
	instrumentation_end();
}


static __always_inline void guest_exit_irqoff(void)
{
	guest_context_exit_irqoff();
	guest_timing_exit_irqoff();
}

static inline void guest_exit(void)
{
	unsigned long flags;

	local_irq_save(flags);
	guest_exit_irqoff();
	local_irq_restore(flags);
}


static __always_inline void guest_state_exit_irqoff(void)
{
	lockdep_hardirqs_off(CALLER_ADDR0);
	guest_context_exit_irqoff();

	instrumentation_begin();
	trace_hardirqs_off_finish();
	instrumentation_end();
}

static inline int kvm_vcpu_exiting_guest_mode(struct kvm_vcpu *vcpu)
{
	
	smp_mb__before_atomic();
	return cmpxchg(&vcpu->mode, IN_GUEST_MODE, EXITING_GUEST_MODE);
}


#define KVM_MEM_MAX_NR_PAGES ((1UL << 31) - 1)


struct kvm_memory_slot {
	struct hlist_node id_node[2];
	struct interval_tree_node hva_node[2];
	struct rb_node gfn_node[2];
	gfn_t base_gfn;
	unsigned long npages;
	unsigned long *dirty_bitmap;
	struct kvm_arch_memory_slot arch;
	unsigned long userspace_addr;
	u32 flags;
	short id;
	u16 as_id;
};

static inline bool kvm_slot_dirty_track_enabled(const struct kvm_memory_slot *slot)
{
	return slot->flags & KVM_MEM_LOG_DIRTY_PAGES;
}

static inline unsigned long kvm_dirty_bitmap_bytes(struct kvm_memory_slot *memslot)
{
	return ALIGN(memslot->npages, BITS_PER_LONG) / 8;
}

static inline unsigned long *kvm_second_dirty_bitmap(struct kvm_memory_slot *memslot)
{
	unsigned long len = kvm_dirty_bitmap_bytes(memslot);

	return memslot->dirty_bitmap + len / sizeof(*memslot->dirty_bitmap);
}

#ifndef KVM_DIRTY_LOG_MANUAL_CAPS
#define KVM_DIRTY_LOG_MANUAL_CAPS KVM_DIRTY_LOG_MANUAL_PROTECT_ENABLE
#endif

struct kvm_s390_adapter_int {
	u64 ind_addr;
	u64 summary_addr;
	u64 ind_offset;
	u32 summary_offset;
	u32 adapter_id;
};

struct kvm_hv_sint {
	u32 vcpu;
	u32 sint;
};

struct kvm_xen_evtchn {
	u32 port;
	u32 vcpu_id;
	int vcpu_idx;
	u32 priority;
};

struct kvm_kernel_irq_routing_entry {
	u32 gsi;
	u32 type;
	int (*set)(struct kvm_kernel_irq_routing_entry *e,
		   struct kvm *kvm, int irq_source_id, int level,
		   bool line_status);
	union {
		struct {
			unsigned irqchip;
			unsigned pin;
		} irqchip;
		struct {
			u32 address_lo;
			u32 address_hi;
			u32 data;
			u32 flags;
			u32 devid;
		} msi;
		struct kvm_s390_adapter_int adapter;
		struct kvm_hv_sint hv_sint;
		struct kvm_xen_evtchn xen_evtchn;
	};
	struct hlist_node link;
};

#ifdef CONFIG_HAVE_KVM_IRQ_ROUTING
struct kvm_irq_routing_table {
	int chip[KVM_NR_IRQCHIPS][KVM_IRQCHIP_NUM_PINS];
	u32 nr_rt_entries;
	
	struct hlist_head map[];
};
#endif

bool kvm_arch_irqchip_in_kernel(struct kvm *kvm);

#ifndef KVM_INTERNAL_MEM_SLOTS
#define KVM_INTERNAL_MEM_SLOTS 0
#endif

#define KVM_MEM_SLOTS_NUM SHRT_MAX
#define KVM_USER_MEM_SLOTS (KVM_MEM_SLOTS_NUM - KVM_INTERNAL_MEM_SLOTS)

#ifndef __KVM_VCPU_MULTIPLE_ADDRESS_SPACE
static inline int kvm_arch_vcpu_memslots_id(struct kvm_vcpu *vcpu)
{
	return 0;
}
#endif

struct kvm_memslots {
	u64 generation;
	atomic_long_t last_used_slot;
	struct rb_root_cached hva_tree;
	struct rb_root gfn_tree;
	
	DECLARE_HASHTABLE(id_hash, 7);
	int node_idx;
};

struct kvm {
#ifdef KVM_HAVE_MMU_RWLOCK
	rwlock_t mmu_lock;
#else
	spinlock_t mmu_lock;
#endif 

	struct mutex slots_lock;

	
	struct mutex slots_arch_lock;
	struct mm_struct *mm; 
	unsigned long nr_memslot_pages;
	
	struct kvm_memslots __memslots[KVM_ADDRESS_SPACE_NUM][2];
	
	struct kvm_memslots __rcu *memslots[KVM_ADDRESS_SPACE_NUM];
	struct xarray vcpu_array;
	
	atomic_t nr_memslots_dirty_logging;

	
	spinlock_t mn_invalidate_lock;
	unsigned long mn_active_invalidate_count;
	struct rcuwait mn_memslots_update_rcuwait;

	
	spinlock_t gpc_lock;
	struct list_head gpc_list;

	
	atomic_t online_vcpus;
	int max_vcpus;
	int created_vcpus;
	int last_boosted_vcpu;
	struct list_head vm_list;
	struct mutex lock;
	struct kvm_io_bus __rcu *buses[KVM_NR_BUSES];
#ifdef CONFIG_HAVE_KVM_EVENTFD
	struct {
		spinlock_t        lock;
		struct list_head  items;
		
		struct list_head  resampler_list;
		struct mutex      resampler_lock;
	} irqfds;
	struct list_head ioeventfds;
#endif
	struct kvm_vm_stat stat;
	struct kvm_arch arch;
	refcount_t users_count;
#ifdef CONFIG_KVM_MMIO
	struct kvm_coalesced_mmio_ring *coalesced_mmio_ring;
	spinlock_t ring_lock;
	struct list_head coalesced_zones;
#endif

	struct mutex irq_lock;
#ifdef CONFIG_HAVE_KVM_IRQCHIP
	
	struct kvm_irq_routing_table __rcu *irq_routing;
#endif
#ifdef CONFIG_HAVE_KVM_IRQFD
	struct hlist_head irq_ack_notifier_list;
#endif

#if defined(CONFIG_MMU_NOTIFIER) && defined(KVM_ARCH_WANT_MMU_NOTIFIER)
	struct mmu_notifier mmu_notifier;
	unsigned long mmu_invalidate_seq;
	long mmu_invalidate_in_progress;
	unsigned long mmu_invalidate_range_start;
	unsigned long mmu_invalidate_range_end;
#endif
	struct list_head devices;
	u64 manual_dirty_log_protect;
	struct dentry *debugfs_dentry;
	struct kvm_stat_data **debugfs_stat_data;
	struct srcu_struct srcu;
	struct srcu_struct irq_srcu;
	pid_t userspace_pid;
	bool override_halt_poll_ns;
	unsigned int max_halt_poll_ns;
	u32 dirty_ring_size;
	bool dirty_ring_with_bitmap;
	bool vm_bugged;
	bool vm_dead;

#ifdef CONFIG_HAVE_KVM_PM_NOTIFIER
	struct notifier_block pm_notifier;
#endif
	char stats_id[KVM_STATS_NAME_SIZE];
};

#define kvm_err(fmt, ...) \
	pr_err("kvm [%i]: " fmt, task_pid_nr(current), ## __VA_ARGS__)
#define kvm_info(fmt, ...) \
	pr_info("kvm [%i]: " fmt, task_pid_nr(current), ## __VA_ARGS__)
#define kvm_debug(fmt, ...) \
	pr_debug("kvm [%i]: " fmt, task_pid_nr(current), ## __VA_ARGS__)
#define kvm_debug_ratelimited(fmt, ...) \
	pr_debug_ratelimited("kvm [%i]: " fmt, task_pid_nr(current), \
			     ## __VA_ARGS__)
#define kvm_pr_unimpl(fmt, ...) \
	pr_err_ratelimited("kvm [%i]: " fmt, \
			   task_tgid_nr(current), ## __VA_ARGS__)


#define vcpu_unimpl(vcpu, fmt, ...)					\
	kvm_pr_unimpl("vcpu%i, guest rIP: 0x%lx " fmt,			\
			(vcpu)->vcpu_id, kvm_rip_read(vcpu), ## __VA_ARGS__)

#define vcpu_debug(vcpu, fmt, ...)					\
	kvm_debug("vcpu%i " fmt, (vcpu)->vcpu_id, ## __VA_ARGS__)
#define vcpu_debug_ratelimited(vcpu, fmt, ...)				\
	kvm_debug_ratelimited("vcpu%i " fmt, (vcpu)->vcpu_id,           \
			      ## __VA_ARGS__)
#define vcpu_err(vcpu, fmt, ...)					\
	kvm_err("vcpu%i " fmt, (vcpu)->vcpu_id, ## __VA_ARGS__)

static inline void kvm_vm_dead(struct kvm *kvm)
{
	kvm->vm_dead = true;
	kvm_make_all_cpus_request(kvm, KVM_REQ_VM_DEAD);
}

static inline void kvm_vm_bugged(struct kvm *kvm)
{
	kvm->vm_bugged = true;
	kvm_vm_dead(kvm);
}


#define KVM_BUG(cond, kvm, fmt...)				\
({								\
	bool __ret = !!(cond);					\
								\
	if (WARN_ONCE(__ret && !(kvm)->vm_bugged, fmt))		\
		kvm_vm_bugged(kvm);				\
	unlikely(__ret);					\
})

#define KVM_BUG_ON(cond, kvm)					\
({								\
	bool __ret = !!(cond);					\
								\
	if (WARN_ON_ONCE(__ret && !(kvm)->vm_bugged))		\
		kvm_vm_bugged(kvm);				\
	unlikely(__ret);					\
})


#define KVM_BUG_ON_DATA_CORRUPTION(cond, kvm)			\
({								\
	bool __ret = !!(cond);					\
								\
	if (IS_ENABLED(CONFIG_BUG_ON_DATA_CORRUPTION))		\
		BUG_ON(__ret);					\
	else if (WARN_ON_ONCE(__ret && !(kvm)->vm_bugged))	\
		kvm_vm_bugged(kvm);				\
	unlikely(__ret);					\
})

static inline void kvm_vcpu_srcu_read_lock(struct kvm_vcpu *vcpu)
{
#ifdef CONFIG_PROVE_RCU
	WARN_ONCE(vcpu->srcu_depth++,
		  "KVM: Illegal vCPU srcu_idx LOCK, depth=%d", vcpu->srcu_depth - 1);
#endif
	vcpu->____srcu_idx = srcu_read_lock(&vcpu->kvm->srcu);
}

static inline void kvm_vcpu_srcu_read_unlock(struct kvm_vcpu *vcpu)
{
	srcu_read_unlock(&vcpu->kvm->srcu, vcpu->____srcu_idx);

#ifdef CONFIG_PROVE_RCU
	WARN_ONCE(--vcpu->srcu_depth,
		  "KVM: Illegal vCPU srcu_idx UNLOCK, depth=%d", vcpu->srcu_depth);
#endif
}

static inline bool kvm_dirty_log_manual_protect_and_init_set(struct kvm *kvm)
{
	return !!(kvm->manual_dirty_log_protect & KVM_DIRTY_LOG_INITIALLY_SET);
}

static inline struct kvm_io_bus *kvm_get_bus(struct kvm *kvm, enum kvm_bus idx)
{
	return srcu_dereference_check(kvm->buses[idx], &kvm->srcu,
				      lockdep_is_held(&kvm->slots_lock) ||
				      !refcount_read(&kvm->users_count));
}

static inline struct kvm_vcpu *kvm_get_vcpu(struct kvm *kvm, int i)
{
	int num_vcpus = atomic_read(&kvm->online_vcpus);
	i = array_index_nospec(i, num_vcpus);

	
	smp_rmb();
	return xa_load(&kvm->vcpu_array, i);
}

#define kvm_for_each_vcpu(idx, vcpup, kvm)		   \
	xa_for_each_range(&kvm->vcpu_array, idx, vcpup, 0, \
			  (atomic_read(&kvm->online_vcpus) - 1))

static inline struct kvm_vcpu *kvm_get_vcpu_by_id(struct kvm *kvm, int id)
{
	struct kvm_vcpu *vcpu = NULL;
	unsigned long i;

	if (id < 0)
		return NULL;
	if (id < KVM_MAX_VCPUS)
		vcpu = kvm_get_vcpu(kvm, id);
	if (vcpu && vcpu->vcpu_id == id)
		return vcpu;
	kvm_for_each_vcpu(i, vcpu, kvm)
		if (vcpu->vcpu_id == id)
			return vcpu;
	return NULL;
}

void kvm_destroy_vcpus(struct kvm *kvm);

void vcpu_load(struct kvm_vcpu *vcpu);
void vcpu_put(struct kvm_vcpu *vcpu);

#ifdef __KVM_HAVE_IOAPIC
void kvm_arch_post_irq_ack_notifier_list_update(struct kvm *kvm);
void kvm_arch_post_irq_routing_update(struct kvm *kvm);
#else
static inline void kvm_arch_post_irq_ack_notifier_list_update(struct kvm *kvm)
{
}
static inline void kvm_arch_post_irq_routing_update(struct kvm *kvm)
{
}
#endif

#ifdef CONFIG_HAVE_KVM_IRQFD
int kvm_irqfd_init(void);
void kvm_irqfd_exit(void);
#else
static inline int kvm_irqfd_init(void)
{
	return 0;
}

static inline void kvm_irqfd_exit(void)
{
}
#endif
int kvm_init(unsigned vcpu_size, unsigned vcpu_align, struct module *module);
void kvm_exit(void);

void kvm_get_kvm(struct kvm *kvm);
bool kvm_get_kvm_safe(struct kvm *kvm);
void kvm_put_kvm(struct kvm *kvm);
bool file_is_kvm(struct file *file);
void kvm_put_kvm_no_destroy(struct kvm *kvm);

static inline struct kvm_memslots *__kvm_memslots(struct kvm *kvm, int as_id)
{
	as_id = array_index_nospec(as_id, KVM_ADDRESS_SPACE_NUM);
	return srcu_dereference_check(kvm->memslots[as_id], &kvm->srcu,
			lockdep_is_held(&kvm->slots_lock) ||
			!refcount_read(&kvm->users_count));
}

static inline struct kvm_memslots *kvm_memslots(struct kvm *kvm)
{
	return __kvm_memslots(kvm, 0);
}

static inline struct kvm_memslots *kvm_vcpu_memslots(struct kvm_vcpu *vcpu)
{
	int as_id = kvm_arch_vcpu_memslots_id(vcpu);

	return __kvm_memslots(vcpu->kvm, as_id);
}

static inline bool kvm_memslots_empty(struct kvm_memslots *slots)
{
	return RB_EMPTY_ROOT(&slots->gfn_tree);
}

bool kvm_are_all_memslots_empty(struct kvm *kvm);

#define kvm_for_each_memslot(memslot, bkt, slots)			      \
	hash_for_each(slots->id_hash, bkt, memslot, id_node[slots->node_idx]) \
		if (WARN_ON_ONCE(!memslot->npages)) {			      \
		} else

static inline
struct kvm_memory_slot *id_to_memslot(struct kvm_memslots *slots, int id)
{
	struct kvm_memory_slot *slot;
	int idx = slots->node_idx;

	hash_for_each_possible(slots->id_hash, slot, id_node[idx], id) {
		if (slot->id == id)
			return slot;
	}

	return NULL;
}


struct kvm_memslot_iter {
	struct kvm_memslots *slots;
	struct rb_node *node;
	struct kvm_memory_slot *slot;
};

static inline void kvm_memslot_iter_next(struct kvm_memslot_iter *iter)
{
	iter->node = rb_next(iter->node);
	if (!iter->node)
		return;

	iter->slot = container_of(iter->node, struct kvm_memory_slot, gfn_node[iter->slots->node_idx]);
}

static inline void kvm_memslot_iter_start(struct kvm_memslot_iter *iter,
					  struct kvm_memslots *slots,
					  gfn_t start)
{
	int idx = slots->node_idx;
	struct rb_node *tmp;
	struct kvm_memory_slot *slot;

	iter->slots = slots;

	
	iter->node = NULL;
	for (tmp = slots->gfn_tree.rb_node; tmp; ) {
		slot = container_of(tmp, struct kvm_memory_slot, gfn_node[idx]);
		if (start < slot->base_gfn) {
			iter->node = tmp;
			tmp = tmp->rb_left;
		} else {
			tmp = tmp->rb_right;
		}
	}

	
	if (iter->node) {
		
		tmp = rb_prev(iter->node);
		if (tmp)
			iter->node = tmp;
	} else {
		
		iter->node = rb_last(&slots->gfn_tree);
	}

	if (iter->node) {
		iter->slot = container_of(iter->node, struct kvm_memory_slot, gfn_node[idx]);

		
		if (iter->slot->base_gfn + iter->slot->npages <= start)
			kvm_memslot_iter_next(iter);
	}
}

static inline bool kvm_memslot_iter_is_valid(struct kvm_memslot_iter *iter, gfn_t end)
{
	if (!iter->node)
		return false;

	
	return iter->slot->base_gfn < end;
}


#define kvm_for_each_memslot_in_gfn_range(iter, slots, start, end)	\
	for (kvm_memslot_iter_start(iter, slots, start);		\
	     kvm_memslot_iter_is_valid(iter, end);			\
	     kvm_memslot_iter_next(iter))


enum kvm_mr_change {
	KVM_MR_CREATE,
	KVM_MR_DELETE,
	KVM_MR_MOVE,
	KVM_MR_FLAGS_ONLY,
};

int kvm_set_memory_region(struct kvm *kvm,
			  const struct kvm_userspace_memory_region *mem);
int __kvm_set_memory_region(struct kvm *kvm,
			    const struct kvm_userspace_memory_region *mem);
void kvm_arch_free_memslot(struct kvm *kvm, struct kvm_memory_slot *slot);
void kvm_arch_memslots_updated(struct kvm *kvm, u64 gen);
int kvm_arch_prepare_memory_region(struct kvm *kvm,
				const struct kvm_memory_slot *old,
				struct kvm_memory_slot *new,
				enum kvm_mr_change change);
void kvm_arch_commit_memory_region(struct kvm *kvm,
				struct kvm_memory_slot *old,
				const struct kvm_memory_slot *new,
				enum kvm_mr_change change);

void kvm_arch_flush_shadow_all(struct kvm *kvm);

void kvm_arch_flush_shadow_memslot(struct kvm *kvm,
				   struct kvm_memory_slot *slot);

int gfn_to_page_many_atomic(struct kvm_memory_slot *slot, gfn_t gfn,
			    struct page **pages, int nr_pages);

struct page *gfn_to_page(struct kvm *kvm, gfn_t gfn);
unsigned long gfn_to_hva(struct kvm *kvm, gfn_t gfn);
unsigned long gfn_to_hva_prot(struct kvm *kvm, gfn_t gfn, bool *writable);
unsigned long gfn_to_hva_memslot(struct kvm_memory_slot *slot, gfn_t gfn);
unsigned long gfn_to_hva_memslot_prot(struct kvm_memory_slot *slot, gfn_t gfn,
				      bool *writable);
void kvm_release_page_clean(struct page *page);
void kvm_release_page_dirty(struct page *page);

kvm_pfn_t gfn_to_pfn(struct kvm *kvm, gfn_t gfn);
kvm_pfn_t gfn_to_pfn_prot(struct kvm *kvm, gfn_t gfn, bool write_fault,
		      bool *writable);
kvm_pfn_t gfn_to_pfn_memslot(const struct kvm_memory_slot *slot, gfn_t gfn);
kvm_pfn_t gfn_to_pfn_memslot_atomic(const struct kvm_memory_slot *slot, gfn_t gfn);
kvm_pfn_t __gfn_to_pfn_memslot(const struct kvm_memory_slot *slot, gfn_t gfn,
			       bool atomic, bool interruptible, bool *async,
			       bool write_fault, bool *writable, hva_t *hva);

void kvm_release_pfn_clean(kvm_pfn_t pfn);
void kvm_release_pfn_dirty(kvm_pfn_t pfn);
void kvm_set_pfn_dirty(kvm_pfn_t pfn);
void kvm_set_pfn_accessed(kvm_pfn_t pfn);

void kvm_release_pfn(kvm_pfn_t pfn, bool dirty);
int kvm_read_guest_page(struct kvm *kvm, gfn_t gfn, void *data, int offset,
			int len);
int kvm_read_guest(struct kvm *kvm, gpa_t gpa, void *data, unsigned long len);
int kvm_read_guest_cached(struct kvm *kvm, struct gfn_to_hva_cache *ghc,
			   void *data, unsigned long len);
int kvm_read_guest_offset_cached(struct kvm *kvm, struct gfn_to_hva_cache *ghc,
				 void *data, unsigned int offset,
				 unsigned long len);
int kvm_write_guest_page(struct kvm *kvm, gfn_t gfn, const void *data,
			 int offset, int len);
int kvm_write_guest(struct kvm *kvm, gpa_t gpa, const void *data,
		    unsigned long len);
int kvm_write_guest_cached(struct kvm *kvm, struct gfn_to_hva_cache *ghc,
			   void *data, unsigned long len);
int kvm_write_guest_offset_cached(struct kvm *kvm, struct gfn_to_hva_cache *ghc,
				  void *data, unsigned int offset,
				  unsigned long len);
int kvm_gfn_to_hva_cache_init(struct kvm *kvm, struct gfn_to_hva_cache *ghc,
			      gpa_t gpa, unsigned long len);

#define __kvm_get_guest(kvm, gfn, offset, v)				\
({									\
	unsigned long __addr = gfn_to_hva(kvm, gfn);			\
	typeof(v) __user *__uaddr = (typeof(__uaddr))(__addr + offset);	\
	int __ret = -EFAULT;						\
									\
	if (!kvm_is_error_hva(__addr))					\
		__ret = get_user(v, __uaddr);				\
	__ret;								\
})

#define kvm_get_guest(kvm, gpa, v)					\
({									\
	gpa_t __gpa = gpa;						\
	struct kvm *__kvm = kvm;					\
									\
	__kvm_get_guest(__kvm, __gpa >> PAGE_SHIFT,			\
			offset_in_page(__gpa), v);			\
})

#define __kvm_put_guest(kvm, gfn, offset, v)				\
({									\
	unsigned long __addr = gfn_to_hva(kvm, gfn);			\
	typeof(v) __user *__uaddr = (typeof(__uaddr))(__addr + offset);	\
	int __ret = -EFAULT;						\
									\
	if (!kvm_is_error_hva(__addr))					\
		__ret = put_user(v, __uaddr);				\
	if (!__ret)							\
		mark_page_dirty(kvm, gfn);				\
	__ret;								\
})

#define kvm_put_guest(kvm, gpa, v)					\
({									\
	gpa_t __gpa = gpa;						\
	struct kvm *__kvm = kvm;					\
									\
	__kvm_put_guest(__kvm, __gpa >> PAGE_SHIFT,			\
			offset_in_page(__gpa), v);			\
})

int kvm_clear_guest(struct kvm *kvm, gpa_t gpa, unsigned long len);
struct kvm_memory_slot *gfn_to_memslot(struct kvm *kvm, gfn_t gfn);
bool kvm_is_visible_gfn(struct kvm *kvm, gfn_t gfn);
bool kvm_vcpu_is_visible_gfn(struct kvm_vcpu *vcpu, gfn_t gfn);
unsigned long kvm_host_page_size(struct kvm_vcpu *vcpu, gfn_t gfn);
void mark_page_dirty_in_slot(struct kvm *kvm, const struct kvm_memory_slot *memslot, gfn_t gfn);
void mark_page_dirty(struct kvm *kvm, gfn_t gfn);

struct kvm_memslots *kvm_vcpu_memslots(struct kvm_vcpu *vcpu);
struct kvm_memory_slot *kvm_vcpu_gfn_to_memslot(struct kvm_vcpu *vcpu, gfn_t gfn);
kvm_pfn_t kvm_vcpu_gfn_to_pfn_atomic(struct kvm_vcpu *vcpu, gfn_t gfn);
kvm_pfn_t kvm_vcpu_gfn_to_pfn(struct kvm_vcpu *vcpu, gfn_t gfn);
int kvm_vcpu_map(struct kvm_vcpu *vcpu, gpa_t gpa, struct kvm_host_map *map);
void kvm_vcpu_unmap(struct kvm_vcpu *vcpu, struct kvm_host_map *map, bool dirty);
unsigned long kvm_vcpu_gfn_to_hva(struct kvm_vcpu *vcpu, gfn_t gfn);
unsigned long kvm_vcpu_gfn_to_hva_prot(struct kvm_vcpu *vcpu, gfn_t gfn, bool *writable);
int kvm_vcpu_read_guest_page(struct kvm_vcpu *vcpu, gfn_t gfn, void *data, int offset,
			     int len);
int kvm_vcpu_read_guest_atomic(struct kvm_vcpu *vcpu, gpa_t gpa, void *data,
			       unsigned long len);
int kvm_vcpu_read_guest(struct kvm_vcpu *vcpu, gpa_t gpa, void *data,
			unsigned long len);
int kvm_vcpu_write_guest_page(struct kvm_vcpu *vcpu, gfn_t gfn, const void *data,
			      int offset, int len);
int kvm_vcpu_write_guest(struct kvm_vcpu *vcpu, gpa_t gpa, const void *data,
			 unsigned long len);
void kvm_vcpu_mark_page_dirty(struct kvm_vcpu *vcpu, gfn_t gfn);


void kvm_gpc_init(struct gfn_to_pfn_cache *gpc, struct kvm *kvm,
		  struct kvm_vcpu *vcpu, enum pfn_cache_usage usage);


int kvm_gpc_activate(struct gfn_to_pfn_cache *gpc, gpa_t gpa, unsigned long len);


bool kvm_gpc_check(struct gfn_to_pfn_cache *gpc, unsigned long len);


int kvm_gpc_refresh(struct gfn_to_pfn_cache *gpc, unsigned long len);


void kvm_gpc_deactivate(struct gfn_to_pfn_cache *gpc);

void kvm_sigset_activate(struct kvm_vcpu *vcpu);
void kvm_sigset_deactivate(struct kvm_vcpu *vcpu);

void kvm_vcpu_halt(struct kvm_vcpu *vcpu);
bool kvm_vcpu_block(struct kvm_vcpu *vcpu);
void kvm_arch_vcpu_blocking(struct kvm_vcpu *vcpu);
void kvm_arch_vcpu_unblocking(struct kvm_vcpu *vcpu);
bool kvm_vcpu_wake_up(struct kvm_vcpu *vcpu);
void kvm_vcpu_kick(struct kvm_vcpu *vcpu);
int kvm_vcpu_yield_to(struct kvm_vcpu *target);
void kvm_vcpu_on_spin(struct kvm_vcpu *vcpu, bool yield_to_kernel_mode);

void kvm_flush_remote_tlbs(struct kvm *kvm);
void kvm_flush_remote_tlbs_range(struct kvm *kvm, gfn_t gfn, u64 nr_pages);
void kvm_flush_remote_tlbs_memslot(struct kvm *kvm,
				   const struct kvm_memory_slot *memslot);

#ifdef KVM_ARCH_NR_OBJS_PER_MEMORY_CACHE
int kvm_mmu_topup_memory_cache(struct kvm_mmu_memory_cache *mc, int min);
int __kvm_mmu_topup_memory_cache(struct kvm_mmu_memory_cache *mc, int capacity, int min);
int kvm_mmu_memory_cache_nr_free_objects(struct kvm_mmu_memory_cache *mc);
void kvm_mmu_free_memory_cache(struct kvm_mmu_memory_cache *mc);
void *kvm_mmu_memory_cache_alloc(struct kvm_mmu_memory_cache *mc);
#endif

void kvm_mmu_invalidate_begin(struct kvm *kvm, unsigned long start,
			      unsigned long end);
void kvm_mmu_invalidate_end(struct kvm *kvm, unsigned long start,
			    unsigned long end);

long kvm_arch_dev_ioctl(struct file *filp,
			unsigned int ioctl, unsigned long arg);
long kvm_arch_vcpu_ioctl(struct file *filp,
			 unsigned int ioctl, unsigned long arg);
vm_fault_t kvm_arch_vcpu_fault(struct kvm_vcpu *vcpu, struct vm_fault *vmf);

int kvm_vm_ioctl_check_extension(struct kvm *kvm, long ext);

void kvm_arch_mmu_enable_log_dirty_pt_masked(struct kvm *kvm,
					struct kvm_memory_slot *slot,
					gfn_t gfn_offset,
					unsigned long mask);
void kvm_arch_sync_dirty_log(struct kvm *kvm, struct kvm_memory_slot *memslot);

#ifndef CONFIG_KVM_GENERIC_DIRTYLOG_READ_PROTECT
int kvm_vm_ioctl_get_dirty_log(struct kvm *kvm, struct kvm_dirty_log *log);
int kvm_get_dirty_log(struct kvm *kvm, struct kvm_dirty_log *log,
		      int *is_dirty, struct kvm_memory_slot **memslot);
#endif

int kvm_vm_ioctl_irq_line(struct kvm *kvm, struct kvm_irq_level *irq_level,
			bool line_status);
int kvm_vm_ioctl_enable_cap(struct kvm *kvm,
			    struct kvm_enable_cap *cap);
int kvm_arch_vm_ioctl(struct file *filp, unsigned int ioctl, unsigned long arg);
long kvm_arch_vm_compat_ioctl(struct file *filp, unsigned int ioctl,
			      unsigned long arg);

int kvm_arch_vcpu_ioctl_get_fpu(struct kvm_vcpu *vcpu, struct kvm_fpu *fpu);
int kvm_arch_vcpu_ioctl_set_fpu(struct kvm_vcpu *vcpu, struct kvm_fpu *fpu);

int kvm_arch_vcpu_ioctl_translate(struct kvm_vcpu *vcpu,
				    struct kvm_translation *tr);

int kvm_arch_vcpu_ioctl_get_regs(struct kvm_vcpu *vcpu, struct kvm_regs *regs);
int kvm_arch_vcpu_ioctl_set_regs(struct kvm_vcpu *vcpu, struct kvm_regs *regs);
int kvm_arch_vcpu_ioctl_get_sregs(struct kvm_vcpu *vcpu,
				  struct kvm_sregs *sregs);
int kvm_arch_vcpu_ioctl_set_sregs(struct kvm_vcpu *vcpu,
				  struct kvm_sregs *sregs);
int kvm_arch_vcpu_ioctl_get_mpstate(struct kvm_vcpu *vcpu,
				    struct kvm_mp_state *mp_state);
int kvm_arch_vcpu_ioctl_set_mpstate(struct kvm_vcpu *vcpu,
				    struct kvm_mp_state *mp_state);
int kvm_arch_vcpu_ioctl_set_guest_debug(struct kvm_vcpu *vcpu,
					struct kvm_guest_debug *dbg);
int kvm_arch_vcpu_ioctl_run(struct kvm_vcpu *vcpu);

void kvm_arch_sched_in(struct kvm_vcpu *vcpu, int cpu);

void kvm_arch_vcpu_load(struct kvm_vcpu *vcpu, int cpu);
void kvm_arch_vcpu_put(struct kvm_vcpu *vcpu);
int kvm_arch_vcpu_precreate(struct kvm *kvm, unsigned int id);
int kvm_arch_vcpu_create(struct kvm_vcpu *vcpu);
void kvm_arch_vcpu_postcreate(struct kvm_vcpu *vcpu);
void kvm_arch_vcpu_destroy(struct kvm_vcpu *vcpu);

#ifdef CONFIG_HAVE_KVM_PM_NOTIFIER
int kvm_arch_pm_notifier(struct kvm *kvm, unsigned long state);
#endif

#ifdef __KVM_HAVE_ARCH_VCPU_DEBUGFS
void kvm_arch_create_vcpu_debugfs(struct kvm_vcpu *vcpu, struct dentry *debugfs_dentry);
#else
static inline void kvm_create_vcpu_debugfs(struct kvm_vcpu *vcpu) {}
#endif

#ifdef CONFIG_KVM_GENERIC_HARDWARE_ENABLING
int kvm_arch_hardware_enable(void);
void kvm_arch_hardware_disable(void);
#endif
int kvm_arch_vcpu_runnable(struct kvm_vcpu *vcpu);
bool kvm_arch_vcpu_in_kernel(struct kvm_vcpu *vcpu);
int kvm_arch_vcpu_should_kick(struct kvm_vcpu *vcpu);
bool kvm_arch_dy_runnable(struct kvm_vcpu *vcpu);
bool kvm_arch_dy_has_pending_interrupt(struct kvm_vcpu *vcpu);
int kvm_arch_post_init_vm(struct kvm *kvm);
void kvm_arch_pre_destroy_vm(struct kvm *kvm);
int kvm_arch_create_vm_debugfs(struct kvm *kvm);

#ifndef __KVM_HAVE_ARCH_VM_ALLOC

static inline struct kvm *kvm_arch_alloc_vm(void)
{
	return kzalloc(sizeof(struct kvm), GFP_KERNEL_ACCOUNT);
}
#endif

static inline void __kvm_arch_free_vm(struct kvm *kvm)
{
	kvfree(kvm);
}

#ifndef __KVM_HAVE_ARCH_VM_FREE
static inline void kvm_arch_free_vm(struct kvm *kvm)
{
	__kvm_arch_free_vm(kvm);
}
#endif

#ifndef __KVM_HAVE_ARCH_FLUSH_REMOTE_TLBS
static inline int kvm_arch_flush_remote_tlbs(struct kvm *kvm)
{
	return -ENOTSUPP;
}
#else
int kvm_arch_flush_remote_tlbs(struct kvm *kvm);
#endif

#ifndef __KVM_HAVE_ARCH_FLUSH_REMOTE_TLBS_RANGE
static inline int kvm_arch_flush_remote_tlbs_range(struct kvm *kvm,
						    gfn_t gfn, u64 nr_pages)
{
	return -EOPNOTSUPP;
}
#else
int kvm_arch_flush_remote_tlbs_range(struct kvm *kvm, gfn_t gfn, u64 nr_pages);
#endif

#ifdef __KVM_HAVE_ARCH_NONCOHERENT_DMA
void kvm_arch_register_noncoherent_dma(struct kvm *kvm);
void kvm_arch_unregister_noncoherent_dma(struct kvm *kvm);
bool kvm_arch_has_noncoherent_dma(struct kvm *kvm);
#else
static inline void kvm_arch_register_noncoherent_dma(struct kvm *kvm)
{
}

static inline void kvm_arch_unregister_noncoherent_dma(struct kvm *kvm)
{
}

static inline bool kvm_arch_has_noncoherent_dma(struct kvm *kvm)
{
	return false;
}
#endif
#ifdef __KVM_HAVE_ARCH_ASSIGNED_DEVICE
void kvm_arch_start_assignment(struct kvm *kvm);
void kvm_arch_end_assignment(struct kvm *kvm);
bool kvm_arch_has_assigned_device(struct kvm *kvm);
#else
static inline void kvm_arch_start_assignment(struct kvm *kvm)
{
}

static inline void kvm_arch_end_assignment(struct kvm *kvm)
{
}

static __always_inline bool kvm_arch_has_assigned_device(struct kvm *kvm)
{
	return false;
}
#endif

static inline struct rcuwait *kvm_arch_vcpu_get_wait(struct kvm_vcpu *vcpu)
{
#ifdef __KVM_HAVE_ARCH_WQP
	return vcpu->arch.waitp;
#else
	return &vcpu->wait;
#endif
}


static inline bool __kvm_vcpu_wake_up(struct kvm_vcpu *vcpu)
{
	return !!rcuwait_wake_up(kvm_arch_vcpu_get_wait(vcpu));
}

static inline bool kvm_vcpu_is_blocking(struct kvm_vcpu *vcpu)
{
	return rcuwait_active(kvm_arch_vcpu_get_wait(vcpu));
}

#ifdef __KVM_HAVE_ARCH_INTC_INITIALIZED

bool kvm_arch_intc_initialized(struct kvm *kvm);
#else
static inline bool kvm_arch_intc_initialized(struct kvm *kvm)
{
	return true;
}
#endif

#ifdef CONFIG_GUEST_PERF_EVENTS
unsigned long kvm_arch_vcpu_get_ip(struct kvm_vcpu *vcpu);

void kvm_register_perf_callbacks(unsigned int (*pt_intr_handler)(void));
void kvm_unregister_perf_callbacks(void);
#else
static inline void kvm_register_perf_callbacks(void *ign) {}
static inline void kvm_unregister_perf_callbacks(void) {}
#endif 

int kvm_arch_init_vm(struct kvm *kvm, unsigned long type);
void kvm_arch_destroy_vm(struct kvm *kvm);
void kvm_arch_sync_events(struct kvm *kvm);

int kvm_cpu_has_pending_timer(struct kvm_vcpu *vcpu);

struct page *kvm_pfn_to_refcounted_page(kvm_pfn_t pfn);
bool kvm_is_zone_device_page(struct page *page);

struct kvm_irq_ack_notifier {
	struct hlist_node link;
	unsigned gsi;
	void (*irq_acked)(struct kvm_irq_ack_notifier *kian);
};

int kvm_irq_map_gsi(struct kvm *kvm,
		    struct kvm_kernel_irq_routing_entry *entries, int gsi);
int kvm_irq_map_chip_pin(struct kvm *kvm, unsigned irqchip, unsigned pin);

int kvm_set_irq(struct kvm *kvm, int irq_source_id, u32 irq, int level,
		bool line_status);
int kvm_set_msi(struct kvm_kernel_irq_routing_entry *irq_entry, struct kvm *kvm,
		int irq_source_id, int level, bool line_status);
int kvm_arch_set_irq_inatomic(struct kvm_kernel_irq_routing_entry *e,
			       struct kvm *kvm, int irq_source_id,
			       int level, bool line_status);
bool kvm_irq_has_notifier(struct kvm *kvm, unsigned irqchip, unsigned pin);
void kvm_notify_acked_gsi(struct kvm *kvm, int gsi);
void kvm_notify_acked_irq(struct kvm *kvm, unsigned irqchip, unsigned pin);
void kvm_register_irq_ack_notifier(struct kvm *kvm,
				   struct kvm_irq_ack_notifier *kian);
void kvm_unregister_irq_ack_notifier(struct kvm *kvm,
				   struct kvm_irq_ack_notifier *kian);
int kvm_request_irq_source_id(struct kvm *kvm);
void kvm_free_irq_source_id(struct kvm *kvm, int irq_source_id);
bool kvm_arch_irqfd_allowed(struct kvm *kvm, struct kvm_irqfd *args);


static inline struct kvm_memory_slot *
try_get_memslot(struct kvm_memory_slot *slot, gfn_t gfn)
{
	if (!slot)
		return NULL;

	if (gfn >= slot->base_gfn && gfn < slot->base_gfn + slot->npages)
		return slot;
	else
		return NULL;
}


static inline struct kvm_memory_slot *
search_memslots(struct kvm_memslots *slots, gfn_t gfn, bool approx)
{
	struct kvm_memory_slot *slot;
	struct rb_node *node;
	int idx = slots->node_idx;

	slot = NULL;
	for (node = slots->gfn_tree.rb_node; node; ) {
		slot = container_of(node, struct kvm_memory_slot, gfn_node[idx]);
		if (gfn >= slot->base_gfn) {
			if (gfn < slot->base_gfn + slot->npages)
				return slot;
			node = node->rb_right;
		} else
			node = node->rb_left;
	}

	return approx ? slot : NULL;
}

static inline struct kvm_memory_slot *
____gfn_to_memslot(struct kvm_memslots *slots, gfn_t gfn, bool approx)
{
	struct kvm_memory_slot *slot;

	slot = (struct kvm_memory_slot *)atomic_long_read(&slots->last_used_slot);
	slot = try_get_memslot(slot, gfn);
	if (slot)
		return slot;

	slot = search_memslots(slots, gfn, approx);
	if (slot) {
		atomic_long_set(&slots->last_used_slot, (unsigned long)slot);
		return slot;
	}

	return NULL;
}


static inline struct kvm_memory_slot *
__gfn_to_memslot(struct kvm_memslots *slots, gfn_t gfn)
{
	return ____gfn_to_memslot(slots, gfn, false);
}

static inline unsigned long
__gfn_to_hva_memslot(const struct kvm_memory_slot *slot, gfn_t gfn)
{
	
	unsigned long offset = gfn - slot->base_gfn;
	offset = array_index_nospec(offset, slot->npages);
	return slot->userspace_addr + offset * PAGE_SIZE;
}

static inline int memslot_id(struct kvm *kvm, gfn_t gfn)
{
	return gfn_to_memslot(kvm, gfn)->id;
}

static inline gfn_t
hva_to_gfn_memslot(unsigned long hva, struct kvm_memory_slot *slot)
{
	gfn_t gfn_offset = (hva - slot->userspace_addr) >> PAGE_SHIFT;

	return slot->base_gfn + gfn_offset;
}

static inline gpa_t gfn_to_gpa(gfn_t gfn)
{
	return (gpa_t)gfn << PAGE_SHIFT;
}

static inline gfn_t gpa_to_gfn(gpa_t gpa)
{
	return (gfn_t)(gpa >> PAGE_SHIFT);
}

static inline hpa_t pfn_to_hpa(kvm_pfn_t pfn)
{
	return (hpa_t)pfn << PAGE_SHIFT;
}

static inline bool kvm_is_error_gpa(struct kvm *kvm, gpa_t gpa)
{
	unsigned long hva = gfn_to_hva(kvm, gpa_to_gfn(gpa));

	return kvm_is_error_hva(hva);
}

enum kvm_stat_kind {
	KVM_STAT_VM,
	KVM_STAT_VCPU,
};

struct kvm_stat_data {
	struct kvm *kvm;
	const struct _kvm_stats_desc *desc;
	enum kvm_stat_kind kind;
};

struct _kvm_stats_desc {
	struct kvm_stats_desc desc;
	char name[KVM_STATS_NAME_SIZE];
};

#define STATS_DESC_COMMON(type, unit, base, exp, sz, bsz)		       \
	.flags = type | unit | base |					       \
		 BUILD_BUG_ON_ZERO(type & ~KVM_STATS_TYPE_MASK) |	       \
		 BUILD_BUG_ON_ZERO(unit & ~KVM_STATS_UNIT_MASK) |	       \
		 BUILD_BUG_ON_ZERO(base & ~KVM_STATS_BASE_MASK),	       \
	.exponent = exp,						       \
	.size = sz,							       \
	.bucket_size = bsz

#define VM_GENERIC_STATS_DESC(stat, type, unit, base, exp, sz, bsz)	       \
	{								       \
		{							       \
			STATS_DESC_COMMON(type, unit, base, exp, sz, bsz),     \
			.offset = offsetof(struct kvm_vm_stat, generic.stat)   \
		},							       \
		.name = #stat,						       \
	}
#define VCPU_GENERIC_STATS_DESC(stat, type, unit, base, exp, sz, bsz)	       \
	{								       \
		{							       \
			STATS_DESC_COMMON(type, unit, base, exp, sz, bsz),     \
			.offset = offsetof(struct kvm_vcpu_stat, generic.stat) \
		},							       \
		.name = #stat,						       \
	}
#define VM_STATS_DESC(stat, type, unit, base, exp, sz, bsz)		       \
	{								       \
		{							       \
			STATS_DESC_COMMON(type, unit, base, exp, sz, bsz),     \
			.offset = offsetof(struct kvm_vm_stat, stat)	       \
		},							       \
		.name = #stat,						       \
	}
#define VCPU_STATS_DESC(stat, type, unit, base, exp, sz, bsz)		       \
	{								       \
		{							       \
			STATS_DESC_COMMON(type, unit, base, exp, sz, bsz),     \
			.offset = offsetof(struct kvm_vcpu_stat, stat)	       \
		},							       \
		.name = #stat,						       \
	}

#define STATS_DESC(SCOPE, stat, type, unit, base, exp, sz, bsz)		       \
	SCOPE##_STATS_DESC(stat, type, unit, base, exp, sz, bsz)

#define STATS_DESC_CUMULATIVE(SCOPE, name, unit, base, exponent)	       \
	STATS_DESC(SCOPE, name, KVM_STATS_TYPE_CUMULATIVE,		       \
		unit, base, exponent, 1, 0)
#define STATS_DESC_INSTANT(SCOPE, name, unit, base, exponent)		       \
	STATS_DESC(SCOPE, name, KVM_STATS_TYPE_INSTANT,			       \
		unit, base, exponent, 1, 0)
#define STATS_DESC_PEAK(SCOPE, name, unit, base, exponent)		       \
	STATS_DESC(SCOPE, name, KVM_STATS_TYPE_PEAK,			       \
		unit, base, exponent, 1, 0)
#define STATS_DESC_LINEAR_HIST(SCOPE, name, unit, base, exponent, sz, bsz)     \
	STATS_DESC(SCOPE, name, KVM_STATS_TYPE_LINEAR_HIST,		       \
		unit, base, exponent, sz, bsz)
#define STATS_DESC_LOG_HIST(SCOPE, name, unit, base, exponent, sz)	       \
	STATS_DESC(SCOPE, name, KVM_STATS_TYPE_LOG_HIST,		       \
		unit, base, exponent, sz, 0)


#define STATS_DESC_COUNTER(SCOPE, name)					       \
	STATS_DESC_CUMULATIVE(SCOPE, name, KVM_STATS_UNIT_NONE,		       \
		KVM_STATS_BASE_POW10, 0)

#define STATS_DESC_ICOUNTER(SCOPE, name)				       \
	STATS_DESC_INSTANT(SCOPE, name, KVM_STATS_UNIT_NONE,		       \
		KVM_STATS_BASE_POW10, 0)

#define STATS_DESC_PCOUNTER(SCOPE, name)				       \
	STATS_DESC_PEAK(SCOPE, name, KVM_STATS_UNIT_NONE,		       \
		KVM_STATS_BASE_POW10, 0)


#define STATS_DESC_IBOOLEAN(SCOPE, name)				       \
	STATS_DESC_INSTANT(SCOPE, name, KVM_STATS_UNIT_BOOLEAN,		       \
		KVM_STATS_BASE_POW10, 0)

#define STATS_DESC_PBOOLEAN(SCOPE, name)				       \
	STATS_DESC_PEAK(SCOPE, name, KVM_STATS_UNIT_BOOLEAN,		       \
		KVM_STATS_BASE_POW10, 0)


#define STATS_DESC_TIME_NSEC(SCOPE, name)				       \
	STATS_DESC_CUMULATIVE(SCOPE, name, KVM_STATS_UNIT_SECONDS,	       \
		KVM_STATS_BASE_POW10, -9)

#define STATS_DESC_LINHIST_TIME_NSEC(SCOPE, name, sz, bsz)		       \
	STATS_DESC_LINEAR_HIST(SCOPE, name, KVM_STATS_UNIT_SECONDS,	       \
		KVM_STATS_BASE_POW10, -9, sz, bsz)

#define STATS_DESC_LOGHIST_TIME_NSEC(SCOPE, name, sz)			       \
	STATS_DESC_LOG_HIST(SCOPE, name, KVM_STATS_UNIT_SECONDS,	       \
		KVM_STATS_BASE_POW10, -9, sz)

#define KVM_GENERIC_VM_STATS()						       \
	STATS_DESC_COUNTER(VM_GENERIC, remote_tlb_flush),		       \
	STATS_DESC_COUNTER(VM_GENERIC, remote_tlb_flush_requests)

#define KVM_GENERIC_VCPU_STATS()					       \
	STATS_DESC_COUNTER(VCPU_GENERIC, halt_successful_poll),		       \
	STATS_DESC_COUNTER(VCPU_GENERIC, halt_attempted_poll),		       \
	STATS_DESC_COUNTER(VCPU_GENERIC, halt_poll_invalid),		       \
	STATS_DESC_COUNTER(VCPU_GENERIC, halt_wakeup),			       \
	STATS_DESC_TIME_NSEC(VCPU_GENERIC, halt_poll_success_ns),	       \
	STATS_DESC_TIME_NSEC(VCPU_GENERIC, halt_poll_fail_ns),		       \
	STATS_DESC_TIME_NSEC(VCPU_GENERIC, halt_wait_ns),		       \
	STATS_DESC_LOGHIST_TIME_NSEC(VCPU_GENERIC, halt_poll_success_hist,     \
			HALT_POLL_HIST_COUNT),				       \
	STATS_DESC_LOGHIST_TIME_NSEC(VCPU_GENERIC, halt_poll_fail_hist,	       \
			HALT_POLL_HIST_COUNT),				       \
	STATS_DESC_LOGHIST_TIME_NSEC(VCPU_GENERIC, halt_wait_hist,	       \
			HALT_POLL_HIST_COUNT),				       \
	STATS_DESC_IBOOLEAN(VCPU_GENERIC, blocking)

extern struct dentry *kvm_debugfs_dir;

ssize_t kvm_stats_read(char *id, const struct kvm_stats_header *header,
		       const struct _kvm_stats_desc *desc,
		       void *stats, size_t size_stats,
		       char __user *user_buffer, size_t size, loff_t *offset);


static inline void kvm_stats_linear_hist_update(u64 *data, size_t size,
						u64 value, size_t bucket_size)
{
	size_t index = div64_u64(value, bucket_size);

	index = min(index, size - 1);
	++data[index];
}


static inline void kvm_stats_log_hist_update(u64 *data, size_t size, u64 value)
{
	size_t index = fls64(value);

	index = min(index, size - 1);
	++data[index];
}

#define KVM_STATS_LINEAR_HIST_UPDATE(array, value, bsize)		       \
	kvm_stats_linear_hist_update(array, ARRAY_SIZE(array), value, bsize)
#define KVM_STATS_LOG_HIST_UPDATE(array, value)				       \
	kvm_stats_log_hist_update(array, ARRAY_SIZE(array), value)


extern const struct kvm_stats_header kvm_vm_stats_header;
extern const struct _kvm_stats_desc kvm_vm_stats_desc[];
extern const struct kvm_stats_header kvm_vcpu_stats_header;
extern const struct _kvm_stats_desc kvm_vcpu_stats_desc[];

#if defined(CONFIG_MMU_NOTIFIER) && defined(KVM_ARCH_WANT_MMU_NOTIFIER)
static inline int mmu_invalidate_retry(struct kvm *kvm, unsigned long mmu_seq)
{
	if (unlikely(kvm->mmu_invalidate_in_progress))
		return 1;
	
	smp_rmb();
	if (kvm->mmu_invalidate_seq != mmu_seq)
		return 1;
	return 0;
}

static inline int mmu_invalidate_retry_hva(struct kvm *kvm,
					   unsigned long mmu_seq,
					   unsigned long hva)
{
	lockdep_assert_held(&kvm->mmu_lock);
	
	if (unlikely(kvm->mmu_invalidate_in_progress) &&
	    hva >= kvm->mmu_invalidate_range_start &&
	    hva < kvm->mmu_invalidate_range_end)
		return 1;
	if (kvm->mmu_invalidate_seq != mmu_seq)
		return 1;
	return 0;
}
#endif

#ifdef CONFIG_HAVE_KVM_IRQ_ROUTING

#define KVM_MAX_IRQ_ROUTES 4096 

bool kvm_arch_can_set_irq_routing(struct kvm *kvm);
int kvm_set_irq_routing(struct kvm *kvm,
			const struct kvm_irq_routing_entry *entries,
			unsigned nr,
			unsigned flags);
int kvm_set_routing_entry(struct kvm *kvm,
			  struct kvm_kernel_irq_routing_entry *e,
			  const struct kvm_irq_routing_entry *ue);
void kvm_free_irq_routing(struct kvm *kvm);

#else

static inline void kvm_free_irq_routing(struct kvm *kvm) {}

#endif

int kvm_send_userspace_msi(struct kvm *kvm, struct kvm_msi *msi);

#ifdef CONFIG_HAVE_KVM_EVENTFD

void kvm_eventfd_init(struct kvm *kvm);
int kvm_ioeventfd(struct kvm *kvm, struct kvm_ioeventfd *args);

#ifdef CONFIG_HAVE_KVM_IRQFD
int kvm_irqfd(struct kvm *kvm, struct kvm_irqfd *args);
void kvm_irqfd_release(struct kvm *kvm);
bool kvm_notify_irqfd_resampler(struct kvm *kvm,
				unsigned int irqchip,
				unsigned int pin);
void kvm_irq_routing_update(struct kvm *);
#else
static inline int kvm_irqfd(struct kvm *kvm, struct kvm_irqfd *args)
{
	return -EINVAL;
}

static inline void kvm_irqfd_release(struct kvm *kvm) {}

static inline bool kvm_notify_irqfd_resampler(struct kvm *kvm,
					      unsigned int irqchip,
					      unsigned int pin)
{
	return false;
}
#endif

#else

static inline void kvm_eventfd_init(struct kvm *kvm) {}

static inline int kvm_irqfd(struct kvm *kvm, struct kvm_irqfd *args)
{
	return -EINVAL;
}

static inline void kvm_irqfd_release(struct kvm *kvm) {}

#ifdef CONFIG_HAVE_KVM_IRQCHIP
static inline void kvm_irq_routing_update(struct kvm *kvm)
{
}
#endif

static inline int kvm_ioeventfd(struct kvm *kvm, struct kvm_ioeventfd *args)
{
	return -ENOSYS;
}

#endif 

void kvm_arch_irq_routing_update(struct kvm *kvm);

static inline void __kvm_make_request(int req, struct kvm_vcpu *vcpu)
{
	
	smp_wmb();
	set_bit(req & KVM_REQUEST_MASK, (void *)&vcpu->requests);
}

static __always_inline void kvm_make_request(int req, struct kvm_vcpu *vcpu)
{
	
	BUILD_BUG_ON(!__builtin_constant_p(req) ||
		     (req & KVM_REQUEST_NO_ACTION));

	__kvm_make_request(req, vcpu);
}

static inline bool kvm_request_pending(struct kvm_vcpu *vcpu)
{
	return READ_ONCE(vcpu->requests);
}

static inline bool kvm_test_request(int req, struct kvm_vcpu *vcpu)
{
	return test_bit(req & KVM_REQUEST_MASK, (void *)&vcpu->requests);
}

static inline void kvm_clear_request(int req, struct kvm_vcpu *vcpu)
{
	clear_bit(req & KVM_REQUEST_MASK, (void *)&vcpu->requests);
}

static inline bool kvm_check_request(int req, struct kvm_vcpu *vcpu)
{
	if (kvm_test_request(req, vcpu)) {
		kvm_clear_request(req, vcpu);

		
		smp_mb__after_atomic();
		return true;
	} else {
		return false;
	}
}

#ifdef CONFIG_KVM_GENERIC_HARDWARE_ENABLING
extern bool kvm_rebooting;
#endif

extern unsigned int halt_poll_ns;
extern unsigned int halt_poll_ns_grow;
extern unsigned int halt_poll_ns_grow_start;
extern unsigned int halt_poll_ns_shrink;

struct kvm_device {
	const struct kvm_device_ops *ops;
	struct kvm *kvm;
	void *private;
	struct list_head vm_node;
};


struct kvm_device_ops {
	const char *name;

	
	int (*create)(struct kvm_device *dev, u32 type);

	
	void (*init)(struct kvm_device *dev);

	
	void (*destroy)(struct kvm_device *dev);

	
	void (*release)(struct kvm_device *dev);

	int (*set_attr)(struct kvm_device *dev, struct kvm_device_attr *attr);
	int (*get_attr)(struct kvm_device *dev, struct kvm_device_attr *attr);
	int (*has_attr)(struct kvm_device *dev, struct kvm_device_attr *attr);
	long (*ioctl)(struct kvm_device *dev, unsigned int ioctl,
		      unsigned long arg);
	int (*mmap)(struct kvm_device *dev, struct vm_area_struct *vma);
};

struct kvm_device *kvm_device_from_filp(struct file *filp);
int kvm_register_device_ops(const struct kvm_device_ops *ops, u32 type);
void kvm_unregister_device_ops(u32 type);

extern struct kvm_device_ops kvm_mpic_ops;
extern struct kvm_device_ops kvm_arm_vgic_v2_ops;
extern struct kvm_device_ops kvm_arm_vgic_v3_ops;

#ifdef CONFIG_HAVE_KVM_CPU_RELAX_INTERCEPT

static inline void kvm_vcpu_set_in_spin_loop(struct kvm_vcpu *vcpu, bool val)
{
	vcpu->spin_loop.in_spin_loop = val;
}
static inline void kvm_vcpu_set_dy_eligible(struct kvm_vcpu *vcpu, bool val)
{
	vcpu->spin_loop.dy_eligible = val;
}

#else 

static inline void kvm_vcpu_set_in_spin_loop(struct kvm_vcpu *vcpu, bool val)
{
}

static inline void kvm_vcpu_set_dy_eligible(struct kvm_vcpu *vcpu, bool val)
{
}
#endif 

static inline bool kvm_is_visible_memslot(struct kvm_memory_slot *memslot)
{
	return (memslot && memslot->id < KVM_USER_MEM_SLOTS &&
		!(memslot->flags & KVM_MEMSLOT_INVALID));
}

struct kvm_vcpu *kvm_get_running_vcpu(void);
struct kvm_vcpu * __percpu *kvm_get_running_vcpus(void);

#ifdef CONFIG_HAVE_KVM_IRQ_BYPASS
bool kvm_arch_has_irq_bypass(void);
int kvm_arch_irq_bypass_add_producer(struct irq_bypass_consumer *,
			   struct irq_bypass_producer *);
void kvm_arch_irq_bypass_del_producer(struct irq_bypass_consumer *,
			   struct irq_bypass_producer *);
void kvm_arch_irq_bypass_stop(struct irq_bypass_consumer *);
void kvm_arch_irq_bypass_start(struct irq_bypass_consumer *);
int kvm_arch_update_irqfd_routing(struct kvm *kvm, unsigned int host_irq,
				  uint32_t guest_irq, bool set);
bool kvm_arch_irqfd_route_changed(struct kvm_kernel_irq_routing_entry *,
				  struct kvm_kernel_irq_routing_entry *);
#endif 

#ifdef CONFIG_HAVE_KVM_INVALID_WAKEUPS

static inline bool vcpu_valid_wakeup(struct kvm_vcpu *vcpu)
{
	return vcpu->valid_wakeup;
}

#else
static inline bool vcpu_valid_wakeup(struct kvm_vcpu *vcpu)
{
	return true;
}
#endif 

#ifdef CONFIG_HAVE_KVM_NO_POLL

bool kvm_arch_no_poll(struct kvm_vcpu *vcpu);
#else
static inline bool kvm_arch_no_poll(struct kvm_vcpu *vcpu)
{
	return false;
}
#endif 

#ifdef CONFIG_HAVE_KVM_VCPU_ASYNC_IOCTL
long kvm_arch_vcpu_async_ioctl(struct file *filp,
			       unsigned int ioctl, unsigned long arg);
#else
static inline long kvm_arch_vcpu_async_ioctl(struct file *filp,
					     unsigned int ioctl,
					     unsigned long arg)
{
	return -ENOIOCTLCMD;
}
#endif 

void kvm_arch_guest_memory_reclaimed(struct kvm *kvm);

#ifdef CONFIG_HAVE_KVM_VCPU_RUN_PID_CHANGE
int kvm_arch_vcpu_run_pid_change(struct kvm_vcpu *vcpu);
#else
static inline int kvm_arch_vcpu_run_pid_change(struct kvm_vcpu *vcpu)
{
	return 0;
}
#endif 

typedef int (*kvm_vm_thread_fn_t)(struct kvm *kvm, uintptr_t data);

int kvm_vm_create_worker_thread(struct kvm *kvm, kvm_vm_thread_fn_t thread_fn,
				uintptr_t data, const char *name,
				struct task_struct **thread_ptr);

#ifdef CONFIG_KVM_XFER_TO_GUEST_WORK
static inline void kvm_handle_signal_exit(struct kvm_vcpu *vcpu)
{
	vcpu->run->exit_reason = KVM_EXIT_INTR;
	vcpu->stat.signal_exits++;
}
#endif 


static inline void kvm_account_pgtable_pages(void *virt, int nr)
{
	mod_lruvec_page_state(virt_to_page(virt), NR_SECONDARY_PAGETABLE, nr);
}


#define  KVM_DIRTY_RING_RSVD_ENTRIES  64


#define  KVM_DIRTY_RING_MAX_ENTRIES  65536

#endif
