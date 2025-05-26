/* SPDX-License-Identifier: GPL-2.0 */
#ifndef INCLUDE_XEN_OPS_H
#define INCLUDE_XEN_OPS_H

#include <linux/percpu.h>
#include <linux/notifier.h>
#include <linux/efi.h>
#include <linux/virtio_anchor.h>
#include <xen/features.h>
#include <asm/xen/interface.h>
#include <xen/interface/vcpu.h>

DECLARE_PER_CPU(struct vcpu_info *, xen_vcpu);

DECLARE_PER_CPU(uint32_t, xen_vcpu_id);
static inline uint32_t xen_vcpu_nr(int cpu)
{
	return per_cpu(xen_vcpu_id, cpu);
}

#define XEN_VCPU_ID_INVALID U32_MAX

void xen_arch_pre_suspend(void);
void xen_arch_post_suspend(int suspend_cancelled);

void xen_timer_resume(void);
void xen_arch_resume(void);
void xen_arch_suspend(void);

void xen_reboot(int reason);

void xen_resume_notifier_register(struct notifier_block *nb);
void xen_resume_notifier_unregister(struct notifier_block *nb);

bool xen_vcpu_stolen(int vcpu);
void xen_setup_runstate_info(int cpu);
void xen_time_setup_guest(void);
void xen_manage_runstate_time(int action);
void xen_get_runstate_snapshot(struct vcpu_runstate_info *res);
u64 xen_steal_clock(int cpu);

int xen_setup_shutdown_event(void);

extern unsigned long *xen_contiguous_bitmap;

#if defined(CONFIG_XEN_PV)
int xen_remap_pfn(struct vm_area_struct *vma, unsigned long addr,
		  xen_pfn_t *pfn, int nr, int *err_ptr, pgprot_t prot,
		  unsigned int domid, bool no_translate);
#else
static inline int xen_remap_pfn(struct vm_area_struct *vma, unsigned long addr,
				xen_pfn_t *pfn, int nr, int *err_ptr,
				pgprot_t prot,  unsigned int domid,
				bool no_translate)
{
	BUG();
	return 0;
}
#endif

struct vm_area_struct;

#ifdef CONFIG_XEN_AUTO_XLATE
int xen_xlate_remap_gfn_array(struct vm_area_struct *vma,
			      unsigned long addr,
			      xen_pfn_t *gfn, int nr,
			      int *err_ptr, pgprot_t prot,
			      unsigned int domid,
			      struct page **pages);
int xen_xlate_unmap_gfn_range(struct vm_area_struct *vma,
			      int nr, struct page **pages);
#else

static inline int xen_xlate_remap_gfn_array(struct vm_area_struct *vma,
					    unsigned long addr,
					    xen_pfn_t *gfn, int nr,
					    int *err_ptr, pgprot_t prot,
					    unsigned int domid,
					    struct page **pages)
{
	return -EOPNOTSUPP;
}

static inline int xen_xlate_unmap_gfn_range(struct vm_area_struct *vma,
					    int nr, struct page **pages)
{
	return -EOPNOTSUPP;
}
#endif

int xen_remap_vma_range(struct vm_area_struct *vma, unsigned long addr,
			unsigned long len);


static inline int xen_remap_domain_gfn_array(struct vm_area_struct *vma,
					     unsigned long addr,
					     xen_pfn_t *gfn, int nr,
					     int *err_ptr, pgprot_t prot,
					     unsigned int domid,
					     struct page **pages)
{
	if (xen_feature(XENFEAT_auto_translated_physmap))
		return xen_xlate_remap_gfn_array(vma, addr, gfn, nr, err_ptr,
						 prot, domid, pages);

	
	BUG_ON(err_ptr == NULL);
	return xen_remap_pfn(vma, addr, gfn, nr, err_ptr, prot, domid,
			     false);
}


static inline int xen_remap_domain_mfn_array(struct vm_area_struct *vma,
					     unsigned long addr, xen_pfn_t *mfn,
					     int nr, int *err_ptr,
					     pgprot_t prot, unsigned int domid)
{
	if (xen_feature(XENFEAT_auto_translated_physmap))
		return -EOPNOTSUPP;

	return xen_remap_pfn(vma, addr, mfn, nr, err_ptr, prot, domid,
			     true);
}


static inline int xen_remap_domain_gfn_range(struct vm_area_struct *vma,
					     unsigned long addr,
					     xen_pfn_t gfn, int nr,
					     pgprot_t prot, unsigned int domid,
					     struct page **pages)
{
	if (xen_feature(XENFEAT_auto_translated_physmap))
		return -EOPNOTSUPP;

	return xen_remap_pfn(vma, addr, &gfn, nr, NULL, prot, domid, false);
}

int xen_unmap_domain_gfn_range(struct vm_area_struct *vma,
			       int numpgs, struct page **pages);

int xen_xlate_map_ballooned_pages(xen_pfn_t **pfns, void **vaddr,
				  unsigned long nr_grant_frames);

bool xen_running_on_version_or_later(unsigned int major, unsigned int minor);

void xen_efi_runtime_setup(void);


#if defined(CONFIG_XEN_PV) && !defined(CONFIG_PREEMPTION)

DECLARE_PER_CPU(bool, xen_in_preemptible_hcall);

static inline void xen_preemptible_hcall_begin(void)
{
	__this_cpu_write(xen_in_preemptible_hcall, true);
}

static inline void xen_preemptible_hcall_end(void)
{
	__this_cpu_write(xen_in_preemptible_hcall, false);
}

#else

static inline void xen_preemptible_hcall_begin(void) { }
static inline void xen_preemptible_hcall_end(void) { }

#endif 

#ifdef CONFIG_XEN_GRANT_DMA_OPS
bool xen_virtio_restricted_mem_acc(struct virtio_device *dev);
#else
struct virtio_device;

static inline bool xen_virtio_restricted_mem_acc(struct virtio_device *dev)
{
	return false;
}
#endif 

#endif 
