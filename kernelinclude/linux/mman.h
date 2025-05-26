/* SPDX-License-Identifier: GPL-2.0 */
#ifndef _LINUX_MMAN_H
#define _LINUX_MMAN_H

#include <linux/mm.h>
#include <linux/percpu_counter.h>

#include <linux/atomic.h>
#include <uapi/linux/mman.h>


#ifndef MAP_32BIT
#define MAP_32BIT 0
#endif
#ifndef MAP_ABOVE4G
#define MAP_ABOVE4G 0
#endif
#ifndef MAP_HUGE_2MB
#define MAP_HUGE_2MB 0
#endif
#ifndef MAP_HUGE_1GB
#define MAP_HUGE_1GB 0
#endif
#ifndef MAP_UNINITIALIZED
#define MAP_UNINITIALIZED 0
#endif
#ifndef MAP_SYNC
#define MAP_SYNC 0
#endif


#define LEGACY_MAP_MASK (MAP_SHARED \
		| MAP_PRIVATE \
		| MAP_FIXED \
		| MAP_ANONYMOUS \
		| MAP_DENYWRITE \
		| MAP_EXECUTABLE \
		| MAP_UNINITIALIZED \
		| MAP_GROWSDOWN \
		| MAP_LOCKED \
		| MAP_NORESERVE \
		| MAP_POPULATE \
		| MAP_NONBLOCK \
		| MAP_STACK \
		| MAP_HUGETLB \
		| MAP_32BIT \
		| MAP_ABOVE4G \
		| MAP_HUGE_2MB \
		| MAP_HUGE_1GB)

extern int sysctl_overcommit_memory;
extern int sysctl_overcommit_ratio;
extern unsigned long sysctl_overcommit_kbytes;
extern struct percpu_counter vm_committed_as;

#ifdef CONFIG_SMP
extern s32 vm_committed_as_batch;
extern void mm_compute_batch(int overcommit_policy);
#else
#define vm_committed_as_batch 0
static inline void mm_compute_batch(int overcommit_policy)
{
}
#endif

unsigned long vm_memory_committed(void);

static inline void vm_acct_memory(long pages)
{
	percpu_counter_add_batch(&vm_committed_as, pages, vm_committed_as_batch);
}

static inline void vm_unacct_memory(long pages)
{
	vm_acct_memory(-pages);
}



#ifndef arch_calc_vm_prot_bits
#define arch_calc_vm_prot_bits(prot, pkey) 0
#endif

#ifndef arch_calc_vm_flag_bits
#define arch_calc_vm_flag_bits(flags) 0
#endif

#ifndef arch_validate_prot

static inline bool arch_validate_prot(unsigned long prot, unsigned long addr)
{
	return (prot & ~(PROT_READ | PROT_WRITE | PROT_EXEC | PROT_SEM)) == 0;
}
#define arch_validate_prot arch_validate_prot
#endif

#ifndef arch_validate_flags

static inline bool arch_validate_flags(unsigned long flags)
{
	return true;
}
#define arch_validate_flags arch_validate_flags
#endif


#define _calc_vm_trans(x, bit1, bit2) \
  ((!(bit1) || !(bit2)) ? 0 : \
  ((bit1) <= (bit2) ? ((x) & (bit1)) * ((bit2) / (bit1)) \
   : ((x) & (bit1)) / ((bit1) / (bit2))))


static inline unsigned long
calc_vm_prot_bits(unsigned long prot, unsigned long pkey)
{
	return _calc_vm_trans(prot, PROT_READ,  VM_READ ) |
	       _calc_vm_trans(prot, PROT_WRITE, VM_WRITE) |
	       _calc_vm_trans(prot, PROT_EXEC,  VM_EXEC) |
	       arch_calc_vm_prot_bits(prot, pkey);
}


static inline unsigned long
calc_vm_flag_bits(unsigned long flags)
{
	return _calc_vm_trans(flags, MAP_GROWSDOWN,  VM_GROWSDOWN ) |
	       _calc_vm_trans(flags, MAP_LOCKED,     VM_LOCKED    ) |
	       _calc_vm_trans(flags, MAP_SYNC,	     VM_SYNC      ) |
	       arch_calc_vm_flag_bits(flags);
}

unsigned long vm_commit_limit(void);

#ifndef arch_memory_deny_write_exec_supported
static inline bool arch_memory_deny_write_exec_supported(void)
{
	return true;
}
#define arch_memory_deny_write_exec_supported arch_memory_deny_write_exec_supported
#endif


static inline bool map_deny_write_exec(struct vm_area_struct *vma,  unsigned long vm_flags)
{
	if (!test_bit(MMF_HAS_MDWE, &current->mm->flags))
		return false;

	if ((vm_flags & VM_EXEC) && (vm_flags & VM_WRITE))
		return true;

	if (!(vma->vm_flags & VM_EXEC) && (vm_flags & VM_EXEC))
		return true;

	return false;
}

#endif 
